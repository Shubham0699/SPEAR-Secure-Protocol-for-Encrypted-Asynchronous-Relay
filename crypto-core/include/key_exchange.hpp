#ifndef SPEAR_CRYPTO_KEY_EXCHANGE_HPP
#define SPEAR_CRYPTO_KEY_EXCHANGE_HPP

#include "types.hpp"
#include <optional>

namespace spear {
namespace crypto {

class KeyExchange {
public:
    static std::optional<SharedSecret> derive_shared_secret(
        const SecretKey& local_secret_key,
        const PublicKey& remote_public_key
    );
    
    static ByteVector derive_session_key(
        const SharedSecret& shared_secret,
        const std::string& context,
        size_t key_size = SYMMETRIC_KEY_SIZE
    );
};

} // namespace crypto
} // namespace spear

#endif // SPEAR_CRYPTO_KEY_EXCHANGE_HPP