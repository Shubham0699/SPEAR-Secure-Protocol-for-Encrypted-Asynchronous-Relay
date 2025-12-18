#include "crypto-core/include/types.hpp"
#include "crypto-core/include/utils.hpp"
#include "crypto-core/include/key_management.hpp"
#include "crypto-core/include/key_exchange.hpp"
#include "crypto-core/include/symmetric_crypto.hpp"
#include "crypto-core/include/signing.hpp"
#include <iostream>

using namespace spear::crypto;

int main() {
    if (!utils::initialize()) {
        std::cerr << "Failed to initialize crypto library" << std::endl;
        return 1;
    }
    
    std::cout << "=== SPEAR Crypto Library Test ===" << std::endl;
    
    std::cout << "\n1. Testing key generation..." << std::endl;
    auto kp1 = KeyManagement::generate_keypair();
    auto kp2 = KeyManagement::generate_keypair();
    if (!kp1 || !kp2) {
        std::cerr << "Key generation failed" << std::endl;
        return 1;
    }
    std::cout << "   Generated 2 keypairs successfully" << std::endl;
    
    std::cout << "\n2. Testing key exchange..." << std::endl;
    auto secret1 = KeyExchange::derive_shared_secret(kp1->secret_key, kp2->public_key);
    auto secret2 = KeyExchange::derive_shared_secret(kp2->secret_key, kp1->public_key);
    if (!secret1 || !secret2) {
        std::cerr << "Key exchange failed" << std::endl;
        return 1;
    }
    if (*secret1 == *secret2) {
        std::cout << "   Shared secrets match!" << std::endl;
    } else {
        std::cerr << "   Shared secrets don't match!" << std::endl;
        return 1;
    }
    
    std::cout << "\n3. Testing symmetric encryption..." << std::endl;
    auto session_key_bytes = KeyExchange::derive_session_key(*secret1, "test-session");
    SymmetricKey session_key;
    std::copy(session_key_bytes.begin(), session_key_bytes.end(), session_key.begin());
    
    Nonce nonce = utils::random_nonce();
    ByteVector plaintext = {'H', 'e', 'l', 'l', 'o', ' ', 'S', 'P', 'E', 'A', 'R', '!'};
    
    auto ciphertext = SymmetricCrypto::encrypt_aead(plaintext, session_key, nonce);
    if (!ciphertext) {
        std::cerr << "Encryption failed" << std::endl;
        return 1;
    }
    std::cout << "   Encrypted " << plaintext.size() << " bytes -> " << ciphertext->size() << " bytes" << std::endl;
    
    auto decrypted = SymmetricCrypto::decrypt_aead(*ciphertext, session_key, nonce);
    if (!decrypted) {
        std::cerr << "Decryption failed" << std::endl;
        return 1;
    }
    
    if (*decrypted == plaintext) {
        std::cout << "   Decryption successful!" << std::endl;
    } else {
        std::cerr << "   Decrypted data doesn't match!" << std::endl;
        return 1;
    }
    
    std::cout << "\n4. Testing signing..." << std::endl;
    auto signing_kp = KeyManagement::generate_signing_keypair();
    if (!signing_kp) {
        std::cerr << "Signing keypair generation failed" << std::endl;
        return 1;
    }
    
    ByteVector message = {'T', 'e', 's', 't', ' ', 'm', 'e', 's', 's', 'a', 'g', 'e'};
    auto signature = Signing::sign_message(message, signing_kp->secret_key);
    if (!signature) {
        std::cerr << "Signing failed" << std::endl;
        return 1;
    }
    std::cout << "   Message signed" << std::endl;
    
    bool valid = Signing::verify_signature(message, *signature, signing_kp->public_key);
    if (valid) {
        std::cout << "   Signature verified!" << std::endl;
    } else {
        std::cerr << "   Signature verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "\n=== All tests passed! ===" << std::endl;
    return 0;
}