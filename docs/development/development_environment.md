# Development environment

Cppinecone ships with a fully-instrumented development environment for VSCode driven by
[Devcontainers](https://code.visualstudio.com/docs/devcontainers/containers). If you have the
[devcontainers extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)
installed, VSCode should prompt you to open the project in a devcontainer when you open the repository in a workspace.

## Tasks

Pre-configured [tasks](https://code.visualstudio.com/docs/editor/tasks) for commonly used development operations:

* `build`: builds the library. Can be manually executed via `run_build.sh`
* `test`: runs tests for the library. Note that Pinecone API keys must be provided (trying to run the tests without a
  key will display an error message with details). Can be manually executed via `run_tests.sh`
* `docs`: builds documentation and runs the mkdocs site, including Doxygen generation and hosting. Port 8000 is
  automatically forwarded to your local machine for ease of browsing. Can be manually executed via `run_docs.sh`

## Intellisense, linting, auto-formatting

Standard editor functionality is provided via the [clangd
extension](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd) which is driven
by the `compile_commands.json` compilation database. This file is generated by the build, so it is necessary to build
the library at least once before editor tooling will function properly. An initial can be run using the default build
task (`Ctrl-Shift-B`).