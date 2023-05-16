# Project 1

## Roadmap

### Basic Feature Implementation (50%)

- [ ] Develop code for card encryption using the designated encryption algorithm and internal secret key.
- [ ] Implement the functionality to receive plaintext cards, encrypt them with the internal secret key, and export the encrypted version.
- [ ] Integrate the client identifier into the encrypted card, ensuring it is stored securely.
- [ ] Design and implement the validation logic to receive client answers and encrypted cards, and validate the answers.
- [ ] Include timestamp validation by comparing the provided timestamp with the existing log entries in the encrypted card.
- [ ] Implement the functionality to generate and update a validation log within the encrypted card.
- [ ] Create the capability to retrieve a cleartext log of past validation operations based on an encrypted card.

### Advanced Feature Implementation (20%)

- [ ] Develop a mechanism for enclave upgrades, allowing the service to support its own upgrade process.
- [ ] Add a plaintext label, such as the enclave version, to the encrypted card for compatibility and migration purposes.
- [ ] Implement the migration operation to enable seamless migration of all cards to newer versions of the enclave.
- [ ] Enhance communication security by encrypting all requests made to the enclave using the enclave's public key or a symmetric key.
- [ ] Integrate a signing mechanism using the enclave's private key to sign all enclave responses.

### Documentation and Reporting (30%)

- [ ] Explain the design choices made, the encryption algorithm used, and the security measures implemented.
- [ ] Describe the code implementation of the basic and advanced features.
- [ ] Include diagrams and illustrations to aid understanding of the system architecture and data flows.