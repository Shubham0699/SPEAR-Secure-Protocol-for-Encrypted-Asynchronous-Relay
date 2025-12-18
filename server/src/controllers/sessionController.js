const db = require('../models/database');

exports.getOrCreateSession = (req, res) => {
  try {
    const { username1, username2 } = req.body;

    if (!username1 || !username2) {
      return res.status(400).json({ error: 'Missing required fields' });
    }

    const user1 = db.prepare('SELECT id FROM users WHERE username = ?').get(username1);
    const user2 = db.prepare('SELECT id FROM users WHERE username = ?').get(username2);

    if (!user1 || !user2) {
      return res.status(404).json({ error: 'User not found' });
    }

    const minId = Math.min(user1.id, user2.id);
    const maxId = Math.max(user1.id, user2.id);

    let session = db.prepare(
      'SELECT * FROM sessions WHERE user1_id = ? AND user2_id = ?'
    ).get(minId, maxId);

    if (!session) {
      const stmt = db.prepare(
        'INSERT INTO sessions (user1_id, user2_id) VALUES (?, ?)'
      );
      const result = stmt.run(minId, maxId);
      
      session = db.prepare('SELECT * FROM sessions WHERE id = ?').get(result.lastInsertRowid);
    }

    res.json({
      sessionId: session.id,
      lastCounterUser1: session.last_counter_user1,
      lastCounterUser2: session.last_counter_user2,
      rotationThreshold: session.rotation_threshold,
      needsRotation: false
    });
  } catch (error) {
    console.error('Session error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
};

exports.updateCounter = (req, res) => {
  try {
    const { username1, username2, counter, fromUser } = req.body;

    if (!username1 || !username2 || counter === undefined || !fromUser) {
      return res.status(400).json({ error: 'Missing required fields' });
    }

    const user1 = db.prepare('SELECT id FROM users WHERE username = ?').get(username1);
    const user2 = db.prepare('SELECT id FROM users WHERE username = ?').get(username2);

    if (!user1 || !user2) {
      return res.status(404).json({ error: 'User not found' });
    }

    const minId = Math.min(user1.id, user2.id);
    const maxId = Math.max(user1.id, user2.id);

    const session = db.prepare(
      'SELECT * FROM sessions WHERE user1_id = ? AND user2_id = ?'
    ).get(minId, maxId);

    if (!session) {
      return res.status(404).json({ error: 'Session not found' });
    }

    const isUser1 = fromUser === username1 && user1.id === minId;
    const counterField = isUser1 ? 'last_counter_user1' : 'last_counter_user2';
    const lastCounter = isUser1 ? session.last_counter_user1 : session.last_counter_user2;

    if (counter <= lastCounter) {
      return res.status(400).json({ 
        error: 'Replay attack detected',
        expectedCounter: lastCounter + 1,
        receivedCounter: counter
      });
    }

    const stmt = db.prepare(
      `UPDATE sessions SET ${counterField} = ? WHERE id = ?`
    );
    stmt.run(counter, session.id);

    const needsRotation = counter >= session.rotation_threshold;

    res.json({
      success: true,
      counter: counter,
      needsRotation: needsRotation,
      rotationThreshold: session.rotation_threshold
    });
  } catch (error) {
    console.error('Update counter error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
};