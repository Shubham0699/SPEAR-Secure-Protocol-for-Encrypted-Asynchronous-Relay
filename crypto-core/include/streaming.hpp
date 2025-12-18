#ifndef SPEAR_CRYPTO_STREAMING_HPP
#define SPEAR_CRYPTO_STREAMING_HPP

#include "types.hpp"
#include <optional>
#include <memory>

namespace spear {
namespace crypto {

class StreamingEncryption {
public:
    static constexpr size_t DEFAULT_CHUNK_SIZE = 64 * 1024;
    
    StreamingEncryption(const SymmetricKey& key, 
                       const Nonce& base_nonce,
                       size_t chunk_size = DEFAULT_CHUNK_SIZE);
    ~StreamingEncryption();
    
    StreamingEncryption(const StreamingEncryption&) = delete;
    StreamingEncryption& operator=(const StreamingEncryption&) = delete;
    
    std::optional<ByteVector> encrypt_chunk(const ByteVector& chunk, bool is_final);
    uint64_t current_chunk() const { return chunk_counter_; }
    void reset(const Nonce& new_base_nonce);

private:
    SymmetricKey key_;
    Nonce base_nonce_;
    size_t chunk_size_;
    uint64_t chunk_counter_;
};

class StreamingDecryption {
public:
    StreamingDecryption(const SymmetricKey& key, const Nonce& base_nonce);
    ~StreamingDecryption();
    
    StreamingDecryption(const StreamingDecryption&) = delete;
    StreamingDecryption& operator=(const StreamingDecryption&) = delete;
    
    std::optional<ByteVector> decrypt_chunk(const ByteVector& encrypted_chunk);
    bool is_complete() const { return received_final_; }
    uint64_t expected_chunk() const { return expected_chunk_counter_; }
    void reset(const Nonce& new_base_nonce);

private:
    SymmetricKey key_;
    Nonce base_nonce_;
    uint64_t expected_chunk_counter_;
    bool received_final_;
};

} // namespace crypto
} // namespace spear

#endif // SPEAR_CRYPTO_STREAMING_HPP