@compute @workgroup_size(WORKGROUP_SIZE)
fn multiply(@builtin(global_invocation_id) gid: vec3u) {
    let i = gid.x;
    if (i >= arrayLength(&source)) { return; }
    if (i % 2u == 0u) {
        dest[i] = source[i] * params.multiplier + params.offset;
    }
}
