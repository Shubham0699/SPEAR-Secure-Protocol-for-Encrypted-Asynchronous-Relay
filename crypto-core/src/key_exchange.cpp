#include "key_exchange.hpp"
#include <sodium.h>

namespace spear {
namespace crypto {

std::optional<SharedSecret> KeyExchange::derive_shared_secret(
    const SecretKey& local_secret_key,
    const PublicKey& remote_public_key) {
    
    SharedSecret shared_secret;
    
    if (crypto_scalarmult(shared_secret.data(),
                         local_secret_key.data(),
                         remote_public_key.data()) != 0) {
        return std::nullopt;
    }
    
    return shared_secret;
}

ByteVector KeyExchange::derive_session_key(
    const SharedSecret& shared_secret,
    const std::string& context,
    size_t key_size) {
    
    ByteVector session_key(key_size);
    
    crypto_kdf_derive_from_key(
        session_key.data(),
        key_size,
        1,
        context.c_str(),
        shared_secret.data()
    );
    
    return session_key;
}

} // namespace crypto
} // namespace spear