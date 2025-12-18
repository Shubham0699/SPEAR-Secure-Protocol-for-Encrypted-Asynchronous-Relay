# TODO

## Current Status: 3/8 Milestones Complete (37.5%)

### ✅ Completed
- [x] Milestone 1: C++ Crypto Library + Unit Tests
- [x] Milestone 2: Node.js N-API Addon
- [x] Milestone 3: CLI Client for File Encryption

### 🚧 Next Steps

#### Milestone 4: Server Implementation
- [ ] Setup Node.js Express server
- [ ] User registration endpoint (POST /api/register)
- [ ] Public key storage (SQLite or JSON file)
- [ ] Key retrieval endpoint (GET /api/users/:id/publickey)
- [ ] Authentication middleware
- [ ] Basic health check endpoint

#### Milestone 5: Session Management
- [ ] Session creation protocol
- [ ] Message sending endpoint (POST /api/messages)
- [ ] Session key derivation on client
- [ ] Message format specification

#### Milestone 6: Message Queue System
- [ ] Message storage (encrypted blobs)
- [ ] Queue management (per-user queues)
- [ ] Message polling endpoint (GET /api/messages)
- [ ] Message acknowledgment (DELETE /api/messages/:id)
- [ ] Offline delivery support

#### Milestone 7: Advanced Security Features
- [ ] Session counter tracking
- [ ] Replay detection
- [ ] Automatic key rotation
- [ ] Forward secrecy implementation

#### Milestone 8: Web Dashboard
- [ ] Basic HTML/CSS/JS frontend
- [ ] User registration form
- [ ] Message composition UI
- [ ] Inbox/message list
- [ ] Integration with server API

### Future Enhancements (Post-MVP)
- [ ] WebSocket support for real-time messaging
- [ ] Multi-device support
- [ ] Group messaging
- [ ] File attachment support
- [ ] Message history persistence
- [ ] Key verification/fingerprints
- [ ] Docker deployment
- [ ] Comprehensive integration tests