const native = require("./native.so.node");

native.registerCallback(() => {
  console.log("got event from native module");
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
