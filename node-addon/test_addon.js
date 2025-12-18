const spear = require('./build/Release/spear_addon.node');

console.log('=== SPEAR Node Addon Test ===\n');

console.log('1. Testing key generation...');
const keypair1 = spear.generateKeypair();
const keypair2 = spear.generateKeypair();
console.log('   Generated keypair 1 - Public key:', keypair1.publicKey.length, 'bytes');
console.log('   Generated keypair 2 - Public key:', keypair2.publicKey.length, 'bytes');

console.log('\n2. Testing key exchange...');
const sharedSecret1 = spear.deriveSharedSecret(keypair1.secretKey, keypair2.publicKey);
const sharedSecret2 = spear.deriveSharedSecret(keypair2.secretKey, keypair1.publicKey);
console.log('   Shared secret 1:', sharedSecret1.length, 'bytes');
console.log('   Shared secret 2:', sharedSecret2.length, 'bytes');
console.log('   Secrets match:', sharedSecret1.equals(sharedSecret2));

console.log('\n3. Testing encryption/decryption...');
const plaintext = Buffer.from('Hello SPEAR from Node.js!');
const key = Buffer.alloc(32);
key.fill(0x42);
const nonce = Buffer.alloc(24);
nonce.fill(0x01);

const ciphertext = spear.encrypt(plaintext, key, nonce);
console.log('   Plaintext:', plaintext.length, 'bytes');
console.log('   Ciphertext:', ciphertext.length, 'bytes');

const decrypted = spear.decrypt(ciphertext, key, nonce);
console.log('   Decrypted:', decrypted.toString());
console.log('   Match:', decrypted.equals(plaintext));

console.log('\n4. Testing signing/verification...');
const signingKeypair = spear.generateSigningKeypair();
const message = Buffer.from('Test message for signing');

const signature = spear.sign(message, signingKeypair.secretKey);
console.log('   Signature:', signature.length, 'bytes');

const valid = spear.verify(message, signature, signingKeypair.publicKey);
console.log('   Signature valid:', valid);

const tamperedMessage = Buffer.from('Tampered message');
const invalid = spear.verify(tamperedMessage, signature, signingKeypair.publicKey);
console.log('   Tampered message valid:', invalid);

console.log('\n=== All tests completed! ===');