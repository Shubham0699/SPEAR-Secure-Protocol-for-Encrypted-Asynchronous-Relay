#!/usr/bin/env node

const { Command } = require('commander');
const fs = require('fs');
const path = require('path');
const spear = require('../spear_addon.node');

const program = new Command();

program
  .name('spear-cli')
  .description('SPEAR - Secure file encryption CLI')
  .version('0.1.0');

program
  .command('keygen')
  .description('Generate a new keypair')
  .option('-o, --output <path>', 'Output directory for keys', '.')
  .action((options) => {
    console.log('Generating keypair...');
    const keypair = spear.generateKeypair();
    
    if (!fs.existsSync(options.output)) {
      fs.mkdirSync(options.output, { recursive: true });
    }
    
    const publicKeyPath = path.join(options.output, 'public.key');
    const secretKeyPath = path.join(options.output, 'secret.key');
    
    fs.writeFileSync(publicKeyPath, keypair.publicKey);
    fs.writeFileSync(secretKeyPath, keypair.secretKey);
    
    console.log(`Public key saved to: ${publicKeyPath}`);
    console.log(`Secret key saved to: ${secretKeyPath}`);
    console.log('Keep your secret key safe!');
  });

program
  .command('encrypt')
  .description('Encrypt a file')
  .requiredOption('-i, --input <file>', 'Input file to encrypt')
  .requiredOption('-o, --output <file>', 'Output encrypted file')
  .requiredOption('-k, --key <file>', 'Secret key file')
  .requiredOption('-p, --peer <file>', 'Peer public key file')
  .action((options) => {
    console.log('Encrypting file...');
    
    const plaintext = fs.readFileSync(options.input);
    const secretKey = fs.readFileSync(options.key);
    const peerPublicKey = fs.readFileSync(options.peer);
    
    const sharedSecret = spear.deriveSharedSecret(secretKey, peerPublicKey);
    
    const key = Buffer.alloc(32);
    sharedSecret.copy(key, 0, 0, 32);
    
    const nonce = Buffer.alloc(24);
    const crypto = require('crypto');
    crypto.randomFillSync(nonce);
    
    const ciphertext = spear.encrypt(plaintext, key, nonce);
    
    const output = Buffer.concat([nonce, ciphertext]);
    fs.writeFileSync(options.output, output);
    
    console.log(`File encrypted: ${options.output}`);
    console.log(`Original size: ${plaintext.length} bytes`);
    console.log(`Encrypted size: ${output.length} bytes`);
  });

program
  .command('decrypt')
  .description('Decrypt a file')
  .requiredOption('-i, --input <file>', 'Input encrypted file')
  .requiredOption('-o, --output <file>', 'Output decrypted file')
  .requiredOption('-k, --key <file>', 'Secret key file')
  .requiredOption('-p, --peer <file>', 'Peer public key file')
  .action((options) => {
    console.log('Decrypting file...');
    
    const encrypted = fs.readFileSync(options.input);
    const secretKey = fs.readFileSync(options.key);
    const peerPublicKey = fs.readFileSync(options.peer);
    
    const nonce = encrypted.subarray(0, 24);
    const ciphertext = encrypted.subarray(24);
    
    const sharedSecret = spear.deriveSharedSecret(secretKey, peerPublicKey);
    
    const key = Buffer.alloc(32);
    sharedSecret.copy(key, 0, 0, 32);
    
    const plaintext = spear.decrypt(ciphertext, key, nonce);
    
    fs.writeFileSync(options.output, plaintext);
    
    console.log(`File decrypted: ${options.output}`);
    console.log(`Encrypted size: ${encrypted.length} bytes`);
    console.log(`Decrypted size: ${plaintext.length} bytes`);
  });

program.parse();