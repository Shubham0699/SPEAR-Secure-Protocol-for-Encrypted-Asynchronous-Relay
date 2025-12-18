#include "streaming.hpp"
#include "symmetric_crypto.hpp"
#include <sodium.h>
#include <cstring>

namespace spear {
namespace crypto {

StreamingEncryption::StreamingEncryption(
    const SymmetricKey& key,
    const Nonce& base_nonce,
    size_t chunk_size)
    : key_(key), base_nonce_(base_nonce), chunk_size_(chunk_size), chunk_counter_(0) {
}

StreamingEncryption::~StreamingEncryption() {
    sodium_memzero(key_.data(), key_.size());
}

std::optional<ByteVector> StreamingEncryption::encrypt_chunk(
    const ByteVector& chunk,
    bool is_final) {
    
    Nonce nonce = base_nonce_;
    for (size_t i = 0; i < 8 && i < nonce.size(); ++i) {
        nonce[i] = static_cast<uint8_t>((chunk_counter_ >> (i * 8)) & 0xFF);
    }
    
    ByteVector header(9);
    std::memcpy(header.data(), &chunk_counter_, 8);
    header[8] = is_final ? 1 : 0;
    
    auto encrypted = SymmetricCrypto::encrypt_aead(chunk, key_, nonce, header);
    if (!encrypted) {
        return std::nullopt;
    }
    
    ByteVector result;
    result.reserve(header.size() + encrypted->size());
    result.insert(result.end(), header.begin(), header.end());
    result.insert(result.end(), encrypted->begin(), encrypted->end());
    
    chunk_counter_++;
    return result;
}

void StreamingEncryption::reset(const Nonce& new_base_nonce) {
    base_nonce_ = new_base_nonce;
    chunk_counter_ = 0;
}

StreamingDecryption::StreamingDecryption(
    const SymmetricKey& key,
    const Nonce& base_nonce)
    : key_(key), base_nonce_(base_nonce), expected_chunk_counter_(0), received_final_(false) {
}

StreamingDecryption::~StreamingDecryption() {
    sodium_memzero(key_.data(), key_.size());
}

std::optional<ByteVector> StreamingDecryption::decrypt_chunk(
    const ByteVector& encrypted_chunk) {
    
    if (encrypted_chunk.size() < 9) {
        return std::nullopt;
    }
    
    uint64_t chunk_counter;
    std::memcpy(&chunk_counter, encrypted_chunk.data(), 8);
    
    if (chunk_counter != expected_chunk_counter_) {
        return std::nullopt;
    }
    
    bool is_final = encrypted_chunk[8] != 0;
    
    ByteVector header(encrypted_chunk.begin(), encrypted_chunk.begin() + 9);
    ByteVector ciphertext(encrypted_chunk.begin() + 9, encrypted_chunk.end());
    
    Nonce nonce = base_nonce_;
    for (size_t i = 0; i < 8 && i < nonce.size(); ++i) {
        nonce[i] = static_cast<uint8_t>((chunk_counter >> (i * 8)) & 0xFF);
    }
    
    auto decrypted = SymmetricCrypto::decrypt_aead(ciphertext, key_, nonce, header);
    if (!decrypted) {
        return std::nullopt;
    }
    
    expected_chunk_counter_++;
    if (is_final) {
        received_final_ = true;
    }
    
    return decrypted;
}

void StreamingDecryption::reset(const Nonce& new_base_nonce) {
    base_nonce_ = new_base_nonce;
    expected_chunk_counter_ = 0;
    received_final_ = false;
}

} // namespace crypto
} // namespace spear