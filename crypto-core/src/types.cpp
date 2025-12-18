#include "types.hpp"
#include <sodium.h>
#include <cstring>

namespace spear {
namespace crypto {

KeyPair::~KeyPair() {
    clear();
}

KeyPair::KeyPair(KeyPair&& other) noexcept
    : public_key(other.public_key), secret_key(other.secret_key) {
    sodium_memzero(other.public_key.data(), other.public_key.size());
    sodium_memzero(other.secret_key.data(), other.secret_key.size());
}

KeyPair& KeyPair::operator=(KeyPair&& other) noexcept {
    if (this != &other) {
        clear();
        public_key = other.public_key;
        secret_key = other.secret_key;
        sodium_memzero(other.public_key.data(), other.public_key.size());
        sodium_memzero(other.secret_key.data(), other.secret_key.size());
    }
    return *this;
}

void KeyPair::clear() {
    sodium_memzero(public_key.data(), public_key.size());
    sodium_memzero(secret_key.data(), secret_key.size());
}

SigningKeyPair::~SigningKeyPair() {
    clear();
}

SigningKeyPair::SigningKeyPair(SigningKeyPair&& other) noexcept
    : public_key(other.public_key), secret_key(other.secret_key) {
    sodium_memzero(other.public_key.data(), other.public_key.size());
    sodium_memzero(other.secret_key.data(), other.secret_key.size());
}

SigningKeyPair& SigningKeyPair::operator=(SigningKeyPair&& other) noexcept {
    if (this != &other) {
        clear();
        public_key = other.public_key;
        secret_key = other.secret_key;
        sodium_memzero(other.public_key.data(), other.public_key.size());
        sodium_memzero(other.secret_key.data(), other.secret_key.size());
    }
    return *this;
}

void SigningKeyPair::clear() {
    sodium_memzero(public_key.data(), public_key.size());
    sodium_memzero(secret_key.data(), secret_key.size());
}

} // namespace crypto
} // namespace spear