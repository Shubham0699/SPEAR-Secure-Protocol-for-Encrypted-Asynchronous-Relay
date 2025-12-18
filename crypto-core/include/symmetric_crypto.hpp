#ifndef SPEAR_CRYPTO_SYMMETRIC_HPP
#define SPEAR_CRYPTO_SYMMETRIC_HPP

#include "types.hpp"
#include <optional>

namespace spear {
namespace crypto {

class SymmetricCrypto {
public:
    static std::optional<ByteVector> encrypt_aead(
        const ByteVector& plaintext,
        const SymmetricKey& key,
        const Nonce& nonce,
        const ByteVector& aad = {}
    );
    
    static std::optional<ByteVector> decrypt_aead(
        const ByteVector& ciphertext,
        const SymmetricKey& key,
        const Nonce& nonce,
        const ByteVector& aad = {}
    );
};

class NonceManager {
public:
    NonceManager();
    std::optional<Nonce> next_nonce();
    uint64_t current_counter() const { return counter_; }
    void set_counter(uint64_t counter) { counter_ = counter; }
    void reset();

private:
    Nonce base_nonce_;
    uint64_t counter_;
};

} // namespace crypto
} // namespace spear

#endif // SPEAR_CRYPTO_SYMMETRIC_HPP