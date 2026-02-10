
Emscripten represents WebGPU objects in C++ as opaque handles (integers) that map to actual JavaScript WebGPU objects. You can pass these handles back and forth across the JS/C++ boundary.

**Key header:** `<webgpu/webgpu_cpp.h>` (Dawn's C++ API) or the raw C API via `<webgpu/webgpu.h>`

---

## Example: Write to buffer in C++, render in JS

### 1. C++ side — create and write to a buffer

```cpp
#include <emscripten/html5_webgpu.h>
#include <webgpu/webgpu_cpp.h>

extern "C" {

// Returns a WGPUBuffer handle (castable to a JS object via emscripten)
WGPUBuffer create_and_fill_buffer() {
    wgpu::Device device = wgpu::Device::Acquire(emscripten_webgpu_get_device());

    wgpu::BufferDescriptor desc{};
    desc.size = 256;
    desc.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
    desc.mappedAtCreation = true;

    wgpu::Buffer buffer = device.CreateBuffer(&desc);

    float* data = static_cast<float*>(buffer.GetMappedRange());
    // fill with triangle vertices, etc.
    data[0] = 0.0f; data[1] = 0.5f;   // top
    data[2] = -0.5f; data[3] = -0.5f; // bottom-left
    data[4] = 0.5f; data[5] = -0.5f;  // bottom-right
    buffer.Unmap();

    // Return the raw handle — JS will receive this as an object reference
    return buffer.MoveToCHandle();
}

} // extern "C"
```

### 2. JavaScript side — use the buffer for rendering

```javascript
const { instance } = await WebAssembly.instantiateStreaming(fetch('mymodule.wasm'), imports);

// Get the device Emscripten set up
const device = navigator.gpu.getDevice(); // or however you initialized it

// Call C++ to get a buffer handle
const bufferHandle = instance.exports.create_and_fill_buffer();

// Emscripten exposes a table to unwrap the handle into a real JS GPUBuffer
const buffer = WebGPU.mgrBuffer.get(bufferHandle);

// Now use it normally in a render pass
const encoder = device.createCommandEncoder();
const pass = encoder.beginRenderPass(renderPassDescriptor);
pass.setPipeline(pipeline);
pass.setVertexBuffer(0, buffer); // <-- the buffer from C++!
pass.draw(3);
pass.end();
device.queue.submit([encoder.finish()]);
```

---

## The Key API

| What you need | API |
|---|---|
| Get the WebGPU device in C++ | `emscripten_webgpu_get_device()` |
| Unwrap a handle to JS object | `WebGPU.mgrBuffer.get(handle)` |
| Wrap a JS object into a handle | `WebGPU.mgrBuffer.create(jsObject)` |
| Same pattern for other types | `mgrTexture`, `mgrQueue`, `mgrRenderPipeline`, etc. |

---

## Important Caveats

**Build flags** — you need to compile with:
```sh
emcc -s USE_WEBGPU=1 -s ASYNCIFY=1 your_file.cpp -o out.js
```

**Ownership/lifetime** — be careful about who owns the object. If C++ releases a `WGPUBuffer`, the JS-side reference becomes invalid. Using `MoveToCHandle()` vs `Release()` matters here.

**The `WebGPU` manager object** — this is an internal Emscripten JS object injected into the module's JS glue code. It's not part of the standard WebGPU API but is reliable within Emscripten's output.

**Initialization order** — typically you'd initialize the WebGPU device in JS first (via `navigator.gpu.requestAdapter()` etc.), then pass it to Emscripten via `emscripten_webgpu_init_device()` before the C++ code runs.

This pattern works well for use cases like: physics/simulation in C++ writing to a GPU buffer, then JS consuming it for rendering — exactly your use case.
