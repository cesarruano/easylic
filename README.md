# easylic
An intuitive approach to licensing your C/C++ applications offline using signed activation files.

## Main Features
With easylic, you can define a customized set of licensing requirements. Choose from the following optional features to suit your needs:
- Bind to a specific machine UUID (optional)
- Set an expiration time for the license (optional)
- Associate with a provided license ID file (optional)

Generate activation files which are cryptographically signed. The details related to the license will remain concealed.

Verify the provided activation files. Decrypt them and authenticate their signature from within your deployed application.

## Usage
You would typically compile an activator application to sign the data and encapsulate it in an AES encrypted file. Subsequently, compile your primary application using the verification API. In the example found under `/test`, both these operations are performed: an activation file is created and then verified. Ensure you use the `EASYLIC_PRIVATE` define when compiling the activator; if omitted, all private key handling will be excluded from the compilation.
