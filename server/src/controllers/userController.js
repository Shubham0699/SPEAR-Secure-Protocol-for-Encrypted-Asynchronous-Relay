const db = require('../models/database');

exports.registerUser = (req, res) => {
  try {
    const { username, publicKey, signingPublicKey } = req.body;

    if (!username || !publicKey || !signingPublicKey) {
      return res.status(400).json({ error: 'Missing required fields' });
    }

    const publicKeyBuffer = Buffer.from(publicKey, 'base64');
    const signingPublicKeyBuffer = Buffer.from(signingPublicKey, 'base64');

    if (publicKeyBuffer.length !== 32 || signingPublicKeyBuffer.length !== 32) {
      return res.status(400).json({ error: 'Invalid key sizes' });
    }

    const stmt = db.prepare(
      'INSERT INTO users (username, public_key, signing_public_key) VALUES (?, ?, ?)'
    );

    const result = stmt.run(username, publicKeyBuffer, signingPublicKeyBuffer);

    res.status(201).json({
      id: result.lastInsertRowid,
      username: username,
      message: 'User registered successfully'
    });
  } catch (error) {
    if (error.code === 'SQLITE_CONSTRAINT') {
      return res.status(409).json({ error: 'Username already exists' });
    }
    console.error('Registration error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
};

exports.getUser = (req, res) => {
  try {
    const { username } = req.params;

    const stmt = db.prepare(
      'SELECT id, username, public_key, signing_public_key, created_at FROM users WHERE username = ?'
    );

    const user = stmt.get(username);

    if (!user) {
      return res.status(404).json({ error: 'User not found' });
    }

    res.json({
      id: user.id,
      username: user.username,
      publicKey: user.public_key.toString('base64'),
      signingPublicKey: user.signing_public_key.toString('base64'),
      createdAt: user.created_at
    });
  } catch (error) {
    console.error('Get user error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
};

exports.listUsers = (req, res) => {
  try {
    const stmt = db.prepare('SELECT id, username, created_at FROM users ORDER BY created_at DESC');
    const users = stmt.all();

    res.json({ users });
  } catch (error) {
    console.error('List users error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
};