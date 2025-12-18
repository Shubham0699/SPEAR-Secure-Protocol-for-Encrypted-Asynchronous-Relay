const Database = require('better-sqlite3');
const path = require('path');

const dbPath = path.join(__dirname, '../../spear.db');
const db = new Database(dbPath);

db.exec(`
  CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    public_key BLOB NOT NULL,
    signing_public_key BLOB NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
  );

  CREATE TABLE IF NOT EXISTS sessions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user1_id INTEGER NOT NULL,
    user2_id INTEGER NOT NULL,
    last_counter_user1 INTEGER DEFAULT 0,
    last_counter_user2 INTEGER DEFAULT 0,
    rotation_threshold INTEGER DEFAULT 100,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    last_rotated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user1_id) REFERENCES users(id),
    FOREIGN KEY (user2_id) REFERENCES users(id),
    UNIQUE(user1_id, user2_id)
  );

  CREATE TABLE IF NOT EXISTS messages (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    from_user_id INTEGER NOT NULL,
    to_user_id INTEGER NOT NULL,
    encrypted_content BLOB NOT NULL,
    nonce BLOB NOT NULL,
    signature BLOB NOT NULL,
    counter INTEGER NOT NULL,
    delivered BOOLEAN DEFAULT 0,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (from_user_id) REFERENCES users(id),
    FOREIGN KEY (to_user_id) REFERENCES users(id)
  );

  CREATE INDEX IF NOT EXISTS idx_messages_to_user ON messages(to_user_id, delivered);
  CREATE INDEX IF NOT EXISTS idx_sessions_users ON sessions(user1_id, user2_id);
`);

module.exports = db;