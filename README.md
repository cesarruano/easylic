# easylic
A simple way of licensing your C/C++ applications offline, using signed activation files.

## Main features
Define the set of requirements for your license 
- Tied to machine UUID
- Expiration time
- Tied to a provided license id file

Generate the activation files. They will be cryptographically signed and information regarding the license will not be visible.

Verify given activation files. Decrypt and verify their signature from your deployed application.

## Usage
Typically you would compile an activator application, to sign the data and put it all in an AES encrypted file.
And you would then compile your application using the verification api.
In the example under /test, both actions are done, an activation file is created and then validated.
It is important to use the define EASYLIC_PRIVATE when compiling the activator, otherwise, all private key handling is removed from the compilation.
