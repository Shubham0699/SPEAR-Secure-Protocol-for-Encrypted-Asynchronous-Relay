#include "utils.hpp"
#include <sodium.h>
#include <sstream>
#include <iomanip>

namespace spear {
namespace crypto {
namespace utils {

bool initialize() {
    return sodium_init() >= 0;
}

bool random_bytes(uint8_t* buffer, size_t size) {
    randombytes_buf(buffer, size);
    return true;
}

Nonce random_nonce() {
    Nonce nonce;
    randombytes_buf(nonce.data(), nonce.size());
    return nonce;
}

void secure_memzero(void* ptr, size_t size) {
    sodium_memzero(ptr, size);
}

std::string to_hex(const uint8_t* data, size_t size) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < size; ++i) {
        oss << std::setw(2) << static_cast<int>(data[i]);
    }
    return oss.str();
}

ByteVector from_hex(const std::string& hex) {
    if (hex.length() % 2 != 0) {
        return {};
    }
    
    ByteVector result;
    result.reserve(hex.length() / 2);
    
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byte_str = hex.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byte_str, nullptr, 16));
        result.push_back(byte);
    }
    
    return result;
}

std::string to_base64(const uint8_t* data, size_t size) {
    size_t b64_len = sodium_base64_encoded_len(size, sodium_base64_VARIANT_ORIGINAL);
    std::string result(b64_len, '\0');
    
    sodium_bin2base64(result.data(), b64_len, data, size, sodium_base64_VARIANT_ORIGINAL);
    
    if (!result.empty() && result.back() == '\0') {
        result.pop_back();
    }
    
    return result;
}

ByteVector from_base64(const std::string& base64) {
    ByteVector result(base64.length());
    size_t bin_len;
    
    if (sodium_base642bin(result.data(), result.size(),
                          base64.c_str(), base64.length(),
                          nullptr, &bin_len, nullptr,
                          sodium_base64_VARIANT_ORIGINAL) != 0) {
        return {};
    }
    
    result.resize(bin_len);
    return result;
}

} // namespace utils
} // namespace crypto
} // namespace spear