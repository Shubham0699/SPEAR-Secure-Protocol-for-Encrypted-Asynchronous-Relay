#include "../include/types.hpp"
#include "../include/utils.hpp"
#include "../include/key_management.hpp"
#include "../include/key_exchange.hpp"
#include "../include/symmetric_crypto.hpp"
#include "../include/signing.hpp"
#include "../include/streaming.hpp"
#include <iostream>
#include <cassert>

using namespace spear::crypto;

int tests_passed = 0;
int tests_failed = 0;

void test_pass(const char* test_name) {
    std::cout << "[PASS] " << test_name << std::endl;
    tests_passed++;
}

void test_fail(const char* test_name) {
    std::cout << "[FAIL] " << test_name << std::endl;
    tests_failed++;
}

void test_utils() {
    std::cout << "\n=== Testing Utils Module ===" << std::endl;
    
    uint8_t buffer[32];
    if (utils::random_bytes(buffer, 32)) {
        test_pass("random_bytes generation");
    } else {
        test_fail("random_bytes generation");
    }
    
    Nonce nonce = utils::random_nonce();
    test_pass("random_nonce generation");
    
    const uint8_t test_data[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f};
    std::string hex = utils::to_hex(test_data, 5);
    if (hex == "48656c6c6f") {
        test_pass("to_hex encoding");
    } else {
        test_fail("to_hex encoding");
    }
    
    ByteVector decoded = utils::from_hex("48656c6c6f");
    if (decoded.size() == 5 && decoded[0] == 0x48) {
        test_pass("from_hex decoding");
    } else {
        test_fail("from_hex decoding");
    }
    
    std::string b64 = utils::to_base64(test_data, 5);
    ByteVector b64_decoded = utils::from_base64(b64);
    if (b64_decoded.size() == 5 && b64_decoded[0] == 0x48) {
        test_pass("base64 encode/decode");
    } else {
        test_fail("base64 encode/decode");
    }
}

void test_key_management() {
    std::cout << "\n=== Testing Key Management Module ===" << std::endl;
    
    auto kp = KeyManagement::generate_keypair();
    if (kp.has_value()) {
        test_pass("generate_keypair");
    } else {
        test_fail("generate_keypair");
        return;
    }
    
    ByteVector pub_serialized = KeyManagement::serialize_public_key(kp->public_key);
    if (pub_serialized.size() == PUBLIC_KEY_SIZE) {
        test_pass("serialize_public_key");
    } else {
        test_fail("serialize_public_key");
    }
    
    auto pub_deserialized = KeyManagement::deserialize_public_key(pub_serialized);
    if (pub_deserialized.has_value() && *pub_deserialized == kp->public_key) {
        test_pass("deserialize_public_key");
    } else {
        test_fail("deserialize_public_key");
    }
    
    auto signing_kp = KeyManagement::generate_signing_keypair();
    if (signing_kp.has_value()) {
        test_pass("generate_signing_keypair");
    } else {
        test_fail("generate_signing_keypair");
    }
}

void test_key_exchange() {
    std::cout << "\n=== Testing Key Exchange Module ===" << std::endl;
    
    auto kp1 = KeyManagement::generate_keypair();
    auto kp2 = KeyManagement::generate_keypair();
    
    if (!kp1 || !kp2) {
        test_fail("keypair generation for key exchange");
        return;
    }
    
    auto secret1 = KeyExchange::derive_shared_secret(kp1->secret_key, kp2->public_key);
    auto secret2 = KeyExchange::derive_shared_secret(kp2->secret_key, kp1->public_key);
    
    if (secret1 && secret2) {
        test_pass("derive_shared_secret");
    } else {
        test_fail("derive_shared_secret");
        return;
    }
    
    if (*secret1 == *secret2) {
        test_pass("shared_secrets match");
    } else {
        test_fail("shared_secrets match");
    }
    
    ByteVector session_key = KeyExchange::derive_session_key(*secret1, "test-context");
    if (session_key.size() == SYMMETRIC_KEY_SIZE) {
        test_pass("derive_session_key");
    } else {
        test_fail("derive_session_key");
    }
}

void test_symmetric_crypto() {
    std::cout << "\n=== Testing Symmetric Crypto Module ===" << std::endl;
    
    SymmetricKey key;
    utils::random_bytes(key.data(), key.size());
    Nonce nonce = utils::random_nonce();
    
    ByteVector plaintext = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'};
    
    auto ciphertext = SymmetricCrypto::encrypt_aead(plaintext, key, nonce);
    if (ciphertext.has_value()) {
        test_pass("encrypt_aead");
    } else {
        test_fail("encrypt_aead");
        return;
    }
    
    auto decrypted = SymmetricCrypto::decrypt_aead(*ciphertext, key, nonce);
    if (decrypted.has_value()) {
        test_pass("decrypt_aead");
    } else {
        test_fail("decrypt_aead");
        return;
    }
    
    if (*decrypted == plaintext) {
        test_pass("decrypted matches plaintext");
    } else {
        test_fail("decrypted matches plaintext");
    }
    
    ByteVector aad = {'c', 'o', 'n', 't', 'e', 'x', 't'};
    auto ciphertext_aad = SymmetricCrypto::encrypt_aead(plaintext, key, nonce, aad);
    auto decrypted_aad = SymmetricCrypto::decrypt_aead(*ciphertext_aad, key, nonce, aad);
    
    if (decrypted_aad && *decrypted_aad == plaintext) {
        test_pass("encrypt/decrypt with AAD");
    } else {
        test_fail("encrypt/decrypt with AAD");
    }
    
    NonceManager nm;
    auto nonce1 = nm.next_nonce();
    auto nonce2 = nm.next_nonce();
    if (nonce1 && nonce2 && *nonce1 != *nonce2) {
        test_pass("NonceManager generates unique nonces");
    } else {
        test_fail("NonceManager generates unique nonces");
    }
}

void test_signing() {
    std::cout << "\n=== Testing Signing Module ===" << std::endl;
    
    auto kp = KeyManagement::generate_signing_keypair();
    if (!kp) {
        test_fail("generate signing keypair");
        return;
    }
    
    ByteVector message = {'T', 'e', 's', 't', ' ', 'm', 'e', 's', 's', 'a', 'g', 'e'};
    
    auto signature = Signing::sign_message(message, kp->secret_key);
    if (signature.has_value()) {
        test_pass("sign_message");
    } else {
        test_fail("sign_message");
        return;
    }
    
    bool valid = Signing::verify_signature(message, *signature, kp->public_key);
    if (valid) {
        test_pass("verify_signature (valid)");
    } else {
        test_fail("verify_signature (valid)");
    }
    
    message[0] = 'X';
    valid = Signing::verify_signature(message, *signature, kp->public_key);
    if (!valid) {
        test_pass("verify_signature (invalid - tampered message)");
    } else {
        test_fail("verify_signature (invalid - tampered message)");
    }
}

void test_streaming() {
    std::cout << "\n=== Testing Streaming Module ===" << std::endl;
    
    SymmetricKey key;
    utils::random_bytes(key.data(), key.size());
    Nonce nonce = utils::random_nonce();
    
    StreamingEncryption enc(key, nonce, 1024);
    StreamingDecryption dec(key, nonce);
    
    ByteVector chunk1 = {'C', 'h', 'u', 'n', 'k', ' ', '1'};
    ByteVector chunk2 = {'C', 'h', 'u', 'n', 'k', ' ', '2'};
    
    auto encrypted1 = enc.encrypt_chunk(chunk1, false);
    auto encrypted2 = enc.encrypt_chunk(chunk2, true);
    
    if (encrypted1 && encrypted2) {
        test_pass("encrypt_chunk");
    } else {
        test_fail("encrypt_chunk");
        return;
    }
    
    auto decrypted1 = dec.decrypt_chunk(*encrypted1);
    auto decrypted2 = dec.decrypt_chunk(*encrypted2);
    
    if (decrypted1 && decrypted2) {
        test_pass("decrypt_chunk");
    } else {
        test_fail("decrypt_chunk");
        return;
    }
    
    if (*decrypted1 == chunk1 && *decrypted2 == chunk2) {
        test_pass("decrypted chunks match original");
    } else {
        test_fail("decrypted chunks match original");
    }
    
    if (dec.is_complete()) {
        test_pass("streaming detected final chunk");
    } else {
        test_fail("streaming detected final chunk");
    }
}

int main() {
    if (!utils::initialize()) {
        std::cerr << "Failed to initialize crypto library" << std::endl;
        return 1;
    }
    
    std::cout << "SPEAR Crypto Core Unit Tests" << std::endl;
    std::cout << "=============================" << std::endl;
    
    test_utils();
    test_key_management();
    test_key_exchange();
    test_symmetric_crypto();
    test_signing();
    test_streaming();
    
    std::cout << "\n=============================" << std::endl;
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\nAll tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "\nSome tests failed!" << std::endl;
        return 1;
    }
}