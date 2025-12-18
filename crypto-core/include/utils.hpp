#ifndef SPEAR_CRYPTO_UTILS_HPP
#define SPEAR_CRYPTO_UTILS_HPP

#include "types.hpp"
#include <string>

namespace spear {
namespace crypto {
namespace utils {

// Initialize the crypto library (call before any crypto operations)
bool initialize();

// Generate random bytes
bool random_bytes(uint8_t* buffer, size_t size);
Nonce random_nonce();

// Secure memory operations
void secure_memzero(void* ptr, size_t size);

// Encoding functions
std::string to_hex(const uint8_t* data, size_t size);
ByteVector from_hex(const std::string& hex);
std::string to_base64(const uint8_t* data, size_t size);
ByteVector from_base64(const std::string& base64);

} // namespace utils
} // namespace crypto
} // namespace spear

#endif // SPEAR_CRYPTO_UTILS_HPP