# easylic
A simple way of licensing your C/C++ applications offline, using signed activation files.

## Main features
Define the set of requirements for your license 
- Tied to machine UUID
- Expiration time
- Tied to a provided license id file

Generate the activation files. They will be cryptographically signed and information regarding the license will not be visible.

Verify given activation files. Decrypt and verify their signature from your deployed application.
