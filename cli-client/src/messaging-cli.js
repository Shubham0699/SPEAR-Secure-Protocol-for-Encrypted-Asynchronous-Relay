#!/usr/bin/env node

const { Command } = require('commander');
const fs = require('fs');
const path = require('path');
const spear = require('../spear_addon.node');

const SERVER_URL = process.env.SPEAR_SERVER || 'http://localhost:3000';

const program = new Command();

program
  .name('spear-msg')
  .description('SPEAR - Secure messaging CLI')
  .version('0.1.0');

program
  .command('register')
  .description('Register a new user with the server')
  .requiredOption('-u, --username <name>', 'Username')
  .requiredOption('-k, --keydir <path>', 'Directory to store keys')
  .action(async (options) => {
    try {
      console.log('Generating keypairs...');
      const keypair = spear.generateKeypair();
      const signingKeypair = spear.generateSigningKeypair();

      if (!fs.existsSync(options.keydir)) {
        fs.mkdirSync(options.keydir, { recursive: true });
      }

      fs.writeFileSync(path.join(options.keydir, 'public.key'), keypair.publicKey);
      fs.writeFileSync(path.join(options.keydir, 'secret.key'), keypair.secretKey);
      fs.writeFileSync(path.join(options.keydir, 'signing_public.key'), signingKeypair.publicKey);
      fs.writeFileSync(path.join(options.keydir, 'signing_secret.key'), signingKeypair.secretKey);

      console.log('Registering with server...');
      const response = await fetch(`${SERVER_URL}/api/register`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          username: options.username,
          publicKey: keypair.publicKey.toString('base64'),
          signingPublicKey: signingKeypair.publicKey.toString('base64')
        })
      });

      const data = await response.json();

      if (response.ok) {
        console.log(`Registration successful! User ID: ${data.id}`);
        console.log(`Keys saved to: ${options.keydir}`);
      } else {
        console.error('Registration failed:', data.error);
        process.exit(1);
      }
    } catch (error) {
      console.error('Error:', error.message);
      process.exit(1);
    }
  });

program
  .command('send')
  .description('Send an encrypted message')
  .requiredOption('-f, --from <username>', 'Your username')
  .requiredOption('-t, --to <username>', 'Recipient username')
  .requiredOption('-m, --message <text>', 'Message to send')
  .requiredOption('-k, --keydir <path>', 'Your key directory')
  .action(async (options) => {
    try {
      console.log(`Fetching public key for ${options.to}...`);
      const userResponse = await fetch(`${SERVER_URL}/api/users/${options.to}`);
      if (!userResponse.ok) {
        console.error('User not found');
        process.exit(1);
      }

      const userData = await userResponse.json();
      const recipientPublicKey = Buffer.from(userData.publicKey, 'base64');

      const secretKey = fs.readFileSync(path.join(options.keydir, 'secret.key'));
      const signingSecretKey = fs.readFileSync(path.join(options.keydir, 'signing_secret.key'));

      console.log('Deriving shared secret...');
      const sharedSecret = spear.deriveSharedSecret(secretKey, recipientPublicKey);
      
      const key = Buffer.alloc(32);
      sharedSecret.copy(key, 0, 0, 32);

      const nonce = Buffer.alloc(24);
      require('crypto').randomFillSync(nonce);

      const plaintext = Buffer.from(options.message, 'utf8');

      console.log('Encrypting message...');
      const ciphertext = spear.encrypt(plaintext, key, nonce);

      console.log('Signing message...');
      const signature = spear.sign(ciphertext, signingSecretKey);

      console.log('Sending to server...');
      const response = await fetch(`${SERVER_URL}/api/messages`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          fromUsername: options.from,
          toUsername: options.to,
          encryptedContent: ciphertext.toString('base64'),
          nonce: nonce.toString('base64'),
          signature: signature.toString('base64'),
          counter: 1
        })
      });

      const data = await response.json();

      if (response.ok) {
        console.log('Message sent successfully!');
      } else {
        console.error('Failed to send message:', data.error);
        process.exit(1);
      }
    } catch (error) {
      console.error('Error:', error.message);
      process.exit(1);
    }
  });

program
  .command('receive')
  .description('Receive and decrypt messages')
  .requiredOption('-u, --username <name>', 'Your username')
  .requiredOption('-k, --keydir <path>', 'Your key directory')
  .action(async (options) => {
    try {
      console.log('Fetching messages...');
      const response = await fetch(`${SERVER_URL}/api/messages/${options.username}`);
      
      if (!response.ok) {
        console.error('Failed to fetch messages');
        process.exit(1);
      }

      const data = await response.json();

      if (data.messages.length === 0) {
        console.log('No new messages');
        return;
      }

      const secretKey = fs.readFileSync(path.join(options.keydir, 'secret.key'));

      console.log(`\nYou have ${data.messages.length} new message(s):\n`);

      for (const msg of data.messages) {
        console.log(`--- Message from ${msg.fromUsername} ---`);

        const senderResponse = await fetch(`${SERVER_URL}/api/users/${msg.fromUsername}`);
        const senderData = await senderResponse.json();
        const senderPublicKey = Buffer.from(senderData.publicKey, 'base64');
        const senderSigningPublicKey = Buffer.from(senderData.signingPublicKey, 'base64');

        const sharedSecret = spear.deriveSharedSecret(secretKey, senderPublicKey);
        const key = Buffer.alloc(32);
        sharedSecret.copy(key, 0, 0, 32);

        const ciphertext = Buffer.from(msg.encryptedContent, 'base64');
        const nonce = Buffer.from(msg.nonce, 'base64');
        const signature = Buffer.from(msg.signature, 'base64');

        const isValid = spear.verify(ciphertext, signature, senderSigningPublicKey);
        if (!isValid) {
          console.log('WARNING: Invalid signature!');
          continue;
        }

        const plaintext = spear.decrypt(ciphertext, key, nonce);
        console.log('Message:', plaintext.toString('utf8'));
        console.log('Timestamp:', msg.createdAt);
        console.log('Counter:', msg.counter);
        console.log();

        await fetch(`${SERVER_URL}/api/messages/${msg.id}`, { method: 'DELETE' });
      }

      console.log('All messages processed');
    } catch (error) {
      console.error('Error:', error.message);
      process.exit(1);
    }
  });

program.parse();