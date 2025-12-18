#ifndef SPEAR_CRYPTO_TYPES_HPP
#define SPEAR_CRYPTO_TYPES_HPP

#include <array>
#include <vector>
#include <cstdint>
#include <string>

namespace spear {
namespace crypto {

// Key sizes (using libsodium conventions)
constexpr size_t PUBLIC_KEY_SIZE = 32;
constexpr size_t SECRET_KEY_SIZE = 32;
constexpr size_t SIGNING_PUBLIC_KEY_SIZE = 32;
constexpr size_t SIGNING_SECRET_KEY_SIZE = 64;
constexpr size_t SHARED_SECRET_SIZE = 32;
constexpr size_t SYMMETRIC_KEY_SIZE = 32;
constexpr size_t NONCE_SIZE = 24;
constexpr size_t MAC_SIZE = 16;
constexpr size_t SIGNATURE_SIZE = 64;

// Type aliases
using PublicKey = std::array<uint8_t, PUBLIC_KEY_SIZE>;
using SecretKey = std::array<uint8_t, SECRET_KEY_SIZE>;
using SigningPublicKey = std::array<uint8_t, SIGNING_PUBLIC_KEY_SIZE>;
using SigningSecretKey = std::array<uint8_t, SIGNING_SECRET_KEY_SIZE>;
using SharedSecret = std::array<uint8_t, SHARED_SECRET_SIZE>;
using SymmetricKey = std::array<uint8_t, SYMMETRIC_KEY_SIZE>;
using Nonce = std::array<uint8_t, NONCE_SIZE>;
using Signature = std::array<uint8_t, SIGNATURE_SIZE>;
using ByteVector = std::vector<uint8_t>;

// Key pair for encryption (X25519)
struct KeyPair {
    PublicKey public_key;
    SecretKey secret_key;
    
    KeyPair() = default;
    ~KeyPair();
    
    KeyPair(const KeyPair&) = delete;
    KeyPair& operator=(const KeyPair&) = delete;
    KeyPair(KeyPair&& other) noexcept;
    KeyPair& operator=(KeyPair&& other) noexcept;
    
    void clear();
};

// Signing key pair (Ed25519)
struct SigningKeyPair {
    SigningPublicKey public_key;
    SigningSecretKey secret_key;
    
    SigningKeyPair() = default;
    ~SigningKeyPair();
    
    SigningKeyPair(const SigningKeyPair&) = delete;
    SigningKeyPair& operator=(const SigningKeyPair&) = delete;
    SigningKeyPair(SigningKeyPair&& other) noexcept;
    SigningKeyPair& operator=(SigningKeyPair&& other) noexcept;
    
    void clear();
};

} // namespace crypto
} // namespace spear

#endif // SPEAR_CRYPTO_TYPES_HPP