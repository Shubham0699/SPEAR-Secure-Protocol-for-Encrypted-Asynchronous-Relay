#ifndef SPEAR_CRYPTO_KEY_MANAGEMENT_HPP
#define SPEAR_CRYPTO_KEY_MANAGEMENT_HPP

#include "types.hpp"
#include <optional>

namespace spear {
namespace crypto {

class KeyManagement {
public:
    static std::optional<KeyPair> generate_keypair();
    static std::optional<SigningKeyPair> generate_signing_keypair();
    
    static ByteVector serialize_public_key(const PublicKey& key);
    static std::optional<PublicKey> deserialize_public_key(const ByteVector& data);
    
    static ByteVector serialize_secret_key(const SecretKey& key);
    static std::optional<SecretKey> deserialize_secret_key(const ByteVector& data);
};

} // namespace crypto
} // namespace spear

#endif // SPEAR_CRYPTO_KEY_MANAGEMENT_HPP