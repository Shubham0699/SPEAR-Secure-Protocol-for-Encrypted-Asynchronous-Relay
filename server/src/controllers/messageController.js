const db = require('../models/database');

exports.sendMessage = (req, res) => {
  try {
    const { fromUsername, toUsername, encryptedContent, nonce, signature, counter } = req.body;

    if (!fromUsername || !toUsername || !encryptedContent || !nonce || !signature || counter === undefined) {
      return res.status(400).json({ error: 'Missing required fields' });
    }

    const fromUser = db.prepare('SELECT id FROM users WHERE username = ?').get(fromUsername);
    const toUser = db.prepare('SELECT id FROM users WHERE username = ?').get(toUsername);

    if (!fromUser || !toUser) {
      return res.status(404).json({ error: 'User not found' });
    }

    const encryptedBuffer = Buffer.from(encryptedContent, 'base64');
    const nonceBuffer = Buffer.from(nonce, 'base64');
    const signatureBuffer = Buffer.from(signature, 'base64');

    const stmt = db.prepare(
      'INSERT INTO messages (from_user_id, to_user_id, encrypted_content, nonce, signature, counter) VALUES (?, ?, ?, ?, ?, ?)'
    );

    const result = stmt.run(
      fromUser.id,
      toUser.id,
      encryptedBuffer,
      nonceBuffer,
      signatureBuffer,
      counter
    );

    res.status(201).json({
      id: result.lastInsertRowid,
      message: 'Message sent successfully'
    });
  } catch (error) {
    console.error('Send message error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
};

exports.getMessages = (req, res) => {
  try {
    const { username } = req.params;

    const user = db.prepare('SELECT id FROM users WHERE username = ?').get(username);

    if (!user) {
      return res.status(404).json({ error: 'User not found' });
    }

    const stmt = db.prepare(`
      SELECT 
        m.id,
        u.username as from_username,
        m.encrypted_content,
        m.nonce,
        m.signature,
        m.counter,
        m.created_at
      FROM messages m
      JOIN users u ON m.from_user_id = u.id
      WHERE m.to_user_id = ? AND m.delivered = 0
      ORDER BY m.created_at ASC
    `);

    const messages = stmt.all(user.id);

    const formattedMessages = messages.map(msg => ({
      id: msg.id,
      fromUsername: msg.from_username,
      encryptedContent: msg.encrypted_content.toString('base64'),
      nonce: msg.nonce.toString('base64'),
      signature: msg.signature.toString('base64'),
      counter: msg.counter,
      createdAt: msg.created_at
    }));

    res.json({ messages: formattedMessages });
  } catch (error) {
    console.error('Get messages error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
};

exports.acknowledgeMessage = (req, res) => {
  try {
    const { id } = req.params;

    const stmt = db.prepare('UPDATE messages SET delivered = 1 WHERE id = ?');
    const result = stmt.run(id);

    if (result.changes === 0) {
      return res.status(404).json({ error: 'Message not found' });
    }

    res.json({ message: 'Message acknowledged' });
  } catch (error) {
    console.error('Acknowledge message error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
};