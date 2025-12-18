#ifndef SPEAR_CRYPTO_SIGNING_HPP
#define SPEAR_CRYPTO_SIGNING_HPP

#include "types.hpp"
#include <optional>

namespace spear {
namespace crypto {

class Signing {
public:
    static std::optional<Signature> sign_message(
        const ByteVector& message,
        const SigningSecretKey& secret_key
    );
    
    static bool verify_signature(
        const ByteVector& message,
        const Signature& signature,
        const SigningPublicKey& public_key
    );
};

} // namespace crypto
} // namespace spear

#endif // SPEAR_CRYPTO_SIGNING_HPP