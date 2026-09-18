# A pipeline DSL — design notes, 2026-09-17

**Nothing here is built.** This is a design conversation written down so picking it
up later starts from a document rather than from scratch. It lives in
`dawn_wrapper` because this library is both the closest prior art and the clearest
example of the problem.

## The problem, as stated

Working with — and learning — WebGPU, fonts and SDFs takes enormous boilerplate and
is a wobbly house of cards. Every attempt to reduce the instability introduces
restrictions that make the raw, dangerous version hard to recreate. Rinse and
repeat: motivation drains, and new ideas need hiding rather than experimenting.

The goal is a way to declare pipelines that eliminates the boilerplate **without**
taking away the ability to abuse the system when needed.

## Why existing solutions do not fit

Shader DSLs are concerned with the shader. Pipelines, bind groups and buffers are
fixed, or expressed outside the DSL in the host language. The boilerplate that
hurts is exactly the part they leave out — and it is where the three-places-must-
agree problem lives: the layout, the bind group, and the shader text all restate
the same facts.

**This library is the evidence.** `dawn_wrapper` is ~2,400 lines that removed
boilerplate by fixing what it could not generalise:

- `render_wrapper::pimpl`'s constructor compiles a hardcoded vertex shader
  (`@vertex fn vertexMain(@location(0) p: vec2f)`) and allocates its own vertex and
  index buffers for a full-screen triangle.
- `dawn_utils::make_render_pipeline` hardcodes the vertex entry point, one
  `Float32x2` attribute at location 0, a `BGRA8Unorm` target, no blend state, and
  default topology.

Which is right for driftype, a fragment-shader engine drawing one triangle — and
is precisely the pattern the DSL is meant to escape. A particle demo (vertex
reading a storage buffer, additive blending, no vertex buffer) cannot be built on
this wrapper without changing it.

## Principles

**Graded safety, not a wall.** The model is `std::vector::data()`: you get the raw
pointer *and* the container keeps working, with documented invalidation rules. Most
DSL escape hatches are cliffs — drop to raw and you lose the layer's bookkeeping,
so nobody uses the hatch and everyone designs around the abstraction instead.

**The backdoor is two-way.** Exporting handles lets you escape. *Adopting* an
existing buffer or pipeline lets the DSL be used inside a system it did not create,
which is what makes it adoptable incrementally rather than all at once.

**The IR is the artifact, not the syntax.** A serialisable description of a
pipeline — resources, kinds, access, sizes, body, workgroup shape — with the C++
API as one frontend. A text DSL or a JS/Rust host then consumes the same
description without re-litigating the design.

**Do not invent a second expression language.** The script body stays literal WGSL.
Then "full expressiveness" is free and the compiler shrinks to a declaration parser
and a code generator.

## The notation, as sketched

Resources are anonymous objects; names are given where they are bound, so a name is
stated once and the expression refers to the object:

```cpp
Array<float> s;
Array<float> d;
Uniform u;
Member m = u.add_member<float>("multiplier");
m.set(4);

Pipeline p;
p.read(s, "source_name", 0);
p.write(d, "dest_name", 0);
p.uniform(u, "uniform_name", 0);

Index id("id");
p.script("if (id % 2 == 0) {" + Wgsl(d[id] = s[id] * m) + "}");
```

**The hybrid body is the best idea in it.** Typed fragments where naming and types
help; raw WGSL for control flow and anything else. Safety is graded by which form
you reach for, and neither locks you out of the other — the `data()` principle
applied to the shader body.

Host side, roughly:

```cpp
var p = c(workgroup_size);
p.source_name.write(&data, data_size);
p.uniform_name.multiplier = 4;      // needs codegen or a typed handle in plain C++
p.execute(data_size / workgroup_size, []{ p.dest_name.read([](data){ }); });
```

## Decisions taken

- **Access is declared per pipeline** (`p.read` / `p.write`), not on the resource,
  so one buffer can be read by one pass and written by another.
- **Unsized destinations default to the source's size,** with other policies
  available. Note the failure mode: WebGPU bounds-checks storage writes, so an
  undersized destination silently drops them rather than crashing.
- **Control flow is raw WGSL** in the string, not overloaded constructs. Add
  `If`/`For` only if the strings actually hurt.
- **Recompilation is dirty-flagged.** Split it three ways or a resize will trigger a
  shader compile: recompile (script or workgroup changed), rebind (buffer resized or
  swapped), re-encode (dispatch count changed).
- **Mapped writes are a separate call** from queued writes.

## Open questions

- **Lifetime.** Completion and readback callbacks outlive the call. Settle it as
  policy: the instantiation is a shared handle and callbacks retain it. (Exactly the
  bug fixed in `shader_base.hpp` on 2026-09-17, where the compile callback outlived
  its caller and had to own its request on the heap.)
- **Statement sequencing.** `Wgsl(...)` records one statement; two need a collector.
  It shapes every operator's return type, so decide early.
- **Builtins.** What `Index` maps to, and the rule that nothing appears in generated
  WGSL whose name is not owned by an object on the C++ side.
- **Binding identity.** Positional indices shift when a resource is inserted. driftype
  hit this with uniform lanes: a global assignment renumbered 20 of 22 knobs when one
  preset was added, and the fix was per-owner fixed blocks.
- **`execute` submits or encodes.** Per-call submit is simple; encoding into a frame's
  buffer is how many passes stay cheap. Measured in this repo: 1.83 µs per pass, free
  pipeline switching, field sources composite additively.
- **Async ergonomics.** Nested callbacks are deep at two levels. Futures or coroutines
  for dependency chains; a bus only for frame-level events.
- **Pipeline state beyond bindings:** blend, target format, topology, depth. No place
  in the declaration yet.

## What to build first, and what not to

**First: the compute case only.** The multiply-by-four example, against Dawn directly,
no render pipeline anywhere near it. It tests everything that matters — names stated
once, typed fragments beside raw WGSL, the backdoor, and `p.wgsl()` returning the
generated source. If that does not feel liberating to write, nothing downstream will.

**Not first, and not on this wrapper:** the render case. `dawn_wrapper` fixes the
vertex stage, so it would have to change before it could host the target below.

**The target, when there is something to aim at.** GPU particles, one compute pass
and one draw, in place:

- `particles`: storage array of `{ pos: vec2f, vel: vec2f }`, N = 100k
- `sim` uniform (per frame) and `view` uniform (per resize) — different lifetimes
- compute at `@workgroup_size(64)`, dispatch `ceil(N/64)`: gravity, integrate, bounce
- draw 6 vertices × N instances, no vertex buffer, centre from `particles[instance]`,
  `speed` passed to the fragment stage, round point via `discard`, additive blending

It stresses six things at once: one buffer with two access modes across two
pipelines, two uniforms with different lifetimes, two derivations of one size,
interstage IO, pipeline state that is not bindings, and ordering between passes.

## The plan, and where it stops

Three steps, each leaving something that works:

1. **Create the pipeline by hand, on `dawn_wrapper`.** The compute side is general
   enough to host this: `compute_wrapper` takes your script and entry point, you
   build the bind group layouts, and dispatch is explicit. The fixed vertex shader
   and target format are `render_wrapper`'s, and the compute POC never touches it.
2. **Automate the pipeline creation.** This is the actual experiment: the
   declaration generating what step 1 wrote by hand.
3. **Remove `dawn_wrapper`.** Expect this to be more than pipelines — it also owns
   instance/adapter/device acquisition, surface configuration, the device-lost and
   uncaptured-error hooks that reach the page, and the buffer, encoder and bind
   group wrappers. Mostly dull, genuinely needed.

### Stop conditions, written down cold

Set in advance, because in the middle of it, with conviction running, they are
impossible to judge. Any one of these and the answer is stop, having spent two
days:

- **It needs a parser after all.** If inference over the WGSL body turns out to be
  required rather than optional, the thing being built is a compiler, and that is a
  different project with a different budget.
- **The escape hatch is a cliff.** If reaching for the raw handle means everything
  above it stops working — no bookkeeping, no reuse, no diagnostics — then the
  central claim is false and the result is a wrapper with extra steps.
- **The multiply-by-four example is not visibly nicer** than the raw WebGPU version
  sitting beside it. Not shorter: nicer. Fewer places where one fact is stated
  twice, and a mistake that gets caught earlier.

A fourth, softer one: if after two days there is no runnable thing, the scope grew
again while nobody was looking.

## Prior art worth reviewing

From memory, so treat as leads. In the order worth spending a day on:

- **`wgsl_to_wgpu` / `wgsl_bindgen`** (Rust) — parse WGSL, generate typed bindings.
  The closest existing analogue to the binding half.
- **`sokol-shdc`** (C) — compiles shaders and emits a header of uniform structs and
  bind slots. A model for what generated bindings should look like.
- **Frostbite FrameGraph (2017 talk)** — declaring reads and writes, deriving order
  and transitions. Why a frame beats a pipeline as the unit.
- **Granite** (open Vulkan render graph) and **Falcor** (passes wired in Python,
  shaders in Slang) — real graphs in code.
- **Slang** — `ParameterBlock` and its reflection API, the serious version of
  generating host bindings.
- **TypeGPU**, **Shadeup** — WebGPU-era attempts; see where they stop.
- **Halide** (algorithm/schedule split) and **Taichi** (declared data layout,
  kernels against logical indices) — both embedded DSLs, no parser, which is the
  precedent for doing this in C++ rather than inventing syntax.

**What to look for in each:** where the declaration stops and the host takes over,
what is inferred versus stated, and what escape hatch exists when the generated
thing is not enough. The third question is where most of them disappoint, and it is
the one this design is actually about.
