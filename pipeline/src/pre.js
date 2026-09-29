// Before main() runs: fetch the document named by ?doc=... (default
// examples/multiply.json, from the serve root), fetch each pass's "wgsl" files
// (relative to the document) and concatenate them in order into "source", and
// leave the parsed result on Module for from_js.hpp. JS reads the text; C++ never parses JSON.
//
// A failure lands in Module['pipelineError'] and main() reports it, so a bad
// path says so instead of hanging the run.
Module['preRun'] = Module['preRun'] || [];
Module['preRun'].push(() => {
  addRunDependency('pipeline-document');
  const named = new URLSearchParams(location.search).get('doc') ?? 'examples/multiply.json';
  const url = new URL(named, location.origin + '/');

  const text = async (u) => {
    const response = await fetch(u, { cache: 'no-store' });
    if (!response.ok) {
      throw new Error(`${u.pathname}: ${response.status} ${response.statusText}`);
    }
    return response.text();
  };

  (async () => {
    const doc = JSON.parse(await text(url));
    for (const pass of doc['passes'] ?? []) {
      const files = pass['wgsl'];
      if (Array.isArray(files)) {
        const texts = [];
        for (const file of files) {
          texts.push(await text(new URL(file, url)));
        }
        pass['source'] = texts.join('\n');
      }
    }
    Module['pipelineDocument'] = doc;
  })()
    .catch((e) => { Module['pipelineError'] = String(e && e.message || e); })
    .finally(() => removeRunDependency('pipeline-document'));
});
