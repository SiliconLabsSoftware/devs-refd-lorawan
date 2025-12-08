## Update autogen folder for the build system
### Prerequisites
- [Git](https://git-scm.com/downloads) installed on your machine.
- [slc-cli](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-tools-slc-cli/02-installation) installed and configured.
- Simplicity Studio SDK v2025.6.0 installed in your Simplicity Studio 5 environment.
### Steps
1. Trust the Simplicity Studio SDKs by running the following command in your terminal:
```bash
slc signature trust --sdk "path_to_your_sdk"
```
2. Trust the LoraWan extension by running the following command:
```bash
slc signature trust -extpath "path_to_your_lorawan_extension"
```
3. Modify the `projects/lorawan_extension/examples/main_porting_tests/main_porting_tests.slcp` file to install components/update configurations as needed. We should reflex the changes on this file to all another `.slcp` files in the example projects.
4. cd to the root directory of the cloned repository:
```bash
cd <path_to_your_local_repo>
```
5. Run the following command to update the autogen folder:
```bash
./projects/lorawan_extension/scripts/update_autogen.sh
```
6. Commit & push the changes.
