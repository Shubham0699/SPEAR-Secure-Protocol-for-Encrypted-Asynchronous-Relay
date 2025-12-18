#include "signing.hpp"
#include <sodium.h>

namespace spear {
namespace crypto {

std::optional<Signature> Signing::sign_message(
    const ByteVector& message,
    const SigningSecretKey& secret_key) {
    
    Signature signature;
    
    if (crypto_sign_detached(
            signature.data(),
            nullptr,
            message.data(),
            message.size(),
            secret_key.data()) != 0) {
        return std::nullopt;
    }
    
    return signature;
}

bool Signing::verify_signature(
    const ByteVector& message,
    const Signature& signature,
    const SigningPublicKey& public_key) {
    
    return crypto_sign_verify_detached(
        signature.data(),
        message.data(),
        message.size(),
        public_key.data()
    ) == 0;
}

} // namespace crypto
} // namespace spear