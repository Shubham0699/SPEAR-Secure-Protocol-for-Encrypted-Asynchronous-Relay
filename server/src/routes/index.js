const express = require('express');
const router = express.Router();
const userController = require('../controllers/userController');
const messageController = require('../controllers/messageController');
const sessionController = require('../controllers/sessionController');

router.post('/api/register', userController.registerUser);
router.get('/api/users', userController.listUsers);
router.get('/api/users/:username', userController.getUser);

router.post('/api/sessions', sessionController.getOrCreateSession);
router.post('/api/sessions/counter', sessionController.updateCounter);

router.post('/api/messages', messageController.sendMessage);
router.get('/api/messages/:username', messageController.getMessages);
router.delete('/api/messages/:id', messageController.acknowledgeMessage);

router.get('/health', (req, res) => {
  res.json({ status: 'ok', timestamp: new Date().toISOString() });
});

module.exports = router;