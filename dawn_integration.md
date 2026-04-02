# Dawn Integration

## Current Version

- Branch: `chromium/7767`
- Checked in wholesale under `lib/dawn/` including all third-party dependencies

## How to Update Dawn

Follow these steps in order. Do not skip step 3 — leaving Dawn's `.gitignore` in place will cause dependencies to be silently excluded from the commit.

1. Clone the Dawn repo at the desired branch or commit:
   ```bash
   git clone https://dawn.googlesource.com/dawn lib/dawn --branch chromium/XXXX --depth 1
   ```

2. Remove Dawn's git metadata so it doesn't conflict with the parent repo:
   ```bash
   rm -rf lib/dawn/.git
   rm -f lib/dawn/.gitignore
   ```

3. Fetch all third-party dependencies using Dawn's fetch script:
   ```bash
   cd lib/dawn
   python tools/fetch_dawn_dependencies.py
   ```

4. Verify nothing is missing — the output should show shallow clones for all dependencies with no errors. Key dependencies to confirm:
   - `third_party/abseil-cpp`
   - `third_party/glfw3/src`
   - `third_party/spirv-tools/src`
   - `third_party/webgpu-headers/src`

5. Check total size before committing (1.8GB is expected):
   ```bash
   du -sh lib/dawn
   ```

6. Verify git status is clean — only your own untracked files should appear:
   ```bash
   git status --short
   ```

7. Commit everything:
   ```bash
   git add lib/dawn
   git commit -m "update dawn to chromium/XXXX"
   ```

## CMake Configuration

Dawn is checked in wholesale so dependency fetching is disabled:

```cmake
set(DAWN_FETCH_DEPENDENCIES OFF)
```

Only the Metal backend is enabled on macOS. Vulkan, D3D, and OpenGL backends are off:

```cmake
set(TINT_BUILD_TESTS FALSE)
set(BUILD_SAMPLES OFF)
set(TINT_BUILD_SPV_READER OFF)
set(BUILD_TESTS OFF)
```

## Emscripten / Browser Build

The browser build uses `emdawnwebgpu` instead of Dawn's native backend. This is Dawn's maintained fork of Emscripten's WebGPU bindings, kept in sync with Dawn's API.

The `emdawnwebgpu_pkg` zip is checked in alongside Dawn and referenced via local port:

```cmake
if (EMSCRIPTEN)
    target_compile_options(Example PRIVATE --use-port=emdawnwebgpu)
    target_link_options(Example PRIVATE
        --use-port=emdawnwebgpu
        --closure=1
        --emrun
    )
endif()
```

`--closure=1` is required for release builds to reduce code size. `--use-port` must appear on both compile and link steps.

## Browser Compatibility

| Browser | Status | Notes |
|---|---|---|
| Chrome | Supported | Primary target |
| Edge | Supported | Chromium-based, behaves like Chrome |
| Safari | Broken | Memory misalignment in SDF output. Dawn update did not resolve. Under investigation |
| Firefox | Not supported | Dawn's WebGPU implementation not compatible. Out of scope |

## Known Issues

- **Safari memory misalignment** — SDF buffer output is corrupted on Safari. Suspected WebGPU implementation bug on Safari's side rather than a Dawn issue. Updating Dawn to chromium/7767 did not resolve it.
- **Headless browser test exit** — browser build does not exit cleanly without `emscripten_force_exit(0)` called explicitly after the compute callback completes.

## API Changes in chromium/7767

Several Dawn API types were renamed in this version. If porting code from an older Dawn, apply these substitutions:

| Old name | New name |
|---|---|
| `ImageCopyTexture` | `TexelCopyTextureInfo` |
| `TextureDataLayout` | `TexelCopyBufferLayout` |
| `ShaderModuleWGSLDescriptor` | `ShaderSourceWGSL` |
| `RequiredLimits` | `Limits` |
| `SurfaceDescriptorFromCanvasHTMLSelector` | `EmscriptenSurfaceSourceCanvasHTMLSelector` |
| `WGPURequestAdapterStatus_Success` | `RequestAdapterStatus::Success` |
| `WGPURequestDeviceStatus_Success` | `RequestDeviceStatus::Success` |
| `WGPUBufferMapAsyncStatus_Success` | `MapAsyncStatus::Success` |
| `WGPUCompilationMessageType_Error` | `CompilationMessageType::Error` |

Callback signatures also changed — `auto` parameters no longer work, explicit types are required. `CallbackMode::AllowSpontaneous` must be passed before the callback in most async calls. `Device::Acquire()` and `Adapter::Acquire()` are gone — assign directly.

Emscripten's bundled `webgpu.h` still uses the old names. If building with Emscripten use `emdawnwebgpu` (see above) which is kept in sync with Dawn's naming.
