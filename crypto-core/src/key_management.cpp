#include "key_management.hpp"
#include "utils.hpp"
#include <sodium.h>

namespace spear {
namespace crypto {

std::optional<KeyPair> KeyManagement::generate_keypair() {
    KeyPair kp;
    
    if (crypto_box_keypair(kp.public_key.data(), kp.secret_key.data()) != 0) {
        return std::nullopt;
    }
    
    return kp;
}

std::optional<SigningKeyPair> KeyManagement::generate_signing_keypair() {
    SigningKeyPair kp;
    
    if (crypto_sign_keypair(kp.public_key.data(), kp.secret_key.data()) != 0) {
        return std::nullopt;
    }
    
    return kp;
}

ByteVector KeyManagement::serialize_public_key(const PublicKey& key) {
    return ByteVector(key.begin(), key.end());
}

std::optional<PublicKey> KeyManagement::deserialize_public_key(const ByteVector& data) {
    if (data.size() != PUBLIC_KEY_SIZE) {
        return std::nullopt;
    }
    
    PublicKey key;
    std::copy(data.begin(), data.end(), key.begin());
    return key;
}

ByteVector KeyManagement::serialize_secret_key(const SecretKey& key) {
    return ByteVector(key.begin(), key.end());
}

std::optional<SecretKey> KeyManagement::deserialize_secret_key(const ByteVector& data) {
    if (data.size() != SECRET_KEY_SIZE) {
        return std::nullopt;
    }
    
    SecretKey key;
    std::copy(data.begin(), data.end(), key.begin());
    return key;
}

} // namespace crypto
} // namespace spear