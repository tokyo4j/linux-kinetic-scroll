// const asyncHooks = require("async_hooks");
// asyncHooks
//   .createHook({
//     init(asyncId, type, triggerAsyncId, resource) {
//       fs.writeFileSync(
//         1,
//         `${triggerAsyncId} -> ${asyncId}: ${type} [${Object.keys(resource)}] \n`
//       );
//     },
//     destroy(asyncId) {
//       fs.writeFileSync(1, `${asyncId} destroyed\n`);
//     },
//   })
//   .enable();

const fs = require("fs");
const native = require("./native.so.node");

native.registerCallback(() => {
  fs.writeFileSync(1, "DEBUG: got event from native module\n");
});

// const vscode = require("vscode");

// /**
//  * @param {vscode.ExtensionContext} context
//  */
// function activate(context) {
//   console.log("Congratulations, your extension is now active!");

//   native.registerCallback(() => {
//     console.log("callback from libinput");
//   });

//   let disposable = vscode.commands.registerCommand(
//     "linux-kinetic-scroll.helloworld",
//     function () {
//       vscode.window.showInformationMessage("Hello World!!!");
//       console.log("Hello World command executed");
//     }
//   );

//   context.subscriptions.push(disposable);
// }

// // This method is called when your extension is deactivated
// function deactivate() {}

// module.exports = {
//   activate,
//   deactivate,
// };
