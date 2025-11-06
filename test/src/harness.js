const createModule = require('../web-build/Example.js');

const output = [];
createModule({
  print: (text) => console.log(text),
  printErr: (text) => console.error(text)
}).then(Module => {
  Module._main();
});