#include "symmetric_crypto.hpp"
#include "utils.hpp"
#include <sodium.h>

namespace spear {
namespace crypto {

std::optional<ByteVector> SymmetricCrypto::encrypt_aead(
    const ByteVector& plaintext,
    const SymmetricKey& key,
    const Nonce& nonce,
    const ByteVector& aad) {
    
    ByteVector ciphertext(plaintext.size() + crypto_aead_chacha20poly1305_ietf_ABYTES);
    unsigned long long ciphertext_len;
    
    if (crypto_aead_chacha20poly1305_ietf_encrypt(
            ciphertext.data(), &ciphertext_len,
            plaintext.data(), plaintext.size(),
            aad.empty() ? nullptr : aad.data(), aad.size(),
            nullptr,
            nonce.data(),
            key.data()) != 0) {
        return std::nullopt;
    }
    
    ciphertext.resize(ciphertext_len);
    return ciphertext;
}

std::optional<ByteVector> SymmetricCrypto::decrypt_aead(
    const ByteVector& ciphertext,
    const SymmetricKey& key,
    const Nonce& nonce,
    const ByteVector& aad) {
    
    if (ciphertext.size() < crypto_aead_chacha20poly1305_ietf_ABYTES) {
        return std::nullopt;
    }
    
    ByteVector plaintext(ciphertext.size() - crypto_aead_chacha20poly1305_ietf_ABYTES);
    unsigned long long plaintext_len;
    
    if (crypto_aead_chacha20poly1305_ietf_decrypt(
            plaintext.data(), &plaintext_len,
            nullptr,
            ciphertext.data(), ciphertext.size(),
            aad.empty() ? nullptr : aad.data(), aad.size(),
            nonce.data(),
            key.data()) != 0) {
        return std::nullopt;
    }
    
    plaintext.resize(plaintext_len);
    return plaintext;
}

NonceManager::NonceManager() : counter_(0) {
    utils::random_bytes(base_nonce_.data(), base_nonce_.size());
}

std::optional<Nonce> NonceManager::next_nonce() {
    if (counter_ == UINT64_MAX) {
        return std::nullopt;
    }
    
    Nonce nonce = base_nonce_;
    
    for (size_t i = 0; i < 8 && i < nonce.size(); ++i) {
        nonce[i] = static_cast<uint8_t>((counter_ >> (i * 8)) & 0xFF);
    }
    
    counter_++;
    return nonce;
}

void NonceManager::reset() {
    counter_ = 0;
    utils::random_bytes(base_nonce_.data(), base_nonce_.size());
}

} // namespace crypto
} // namespace spear