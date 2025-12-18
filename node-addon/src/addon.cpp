#include <napi.h>
#include "types.hpp"
#include "utils.hpp"
#include "key_management.hpp"
#include "key_exchange.hpp"
#include "symmetric_crypto.hpp"
#include "signing.hpp"

using namespace spear::crypto;

Napi::Object GenerateKeypair(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    auto keypair = KeyManagement::generate_keypair();
    if (!keypair) {
        Napi::Error::New(env, "Failed to generate keypair").ThrowAsJavaScriptException();
        return Napi::Object::New(env);
    }
    
    Napi::Object result = Napi::Object::New(env);
    result.Set("publicKey", 
        Napi::Buffer<uint8_t>::Copy(env, keypair->public_key.data(), PUBLIC_KEY_SIZE));
    result.Set("secretKey", 
        Napi::Buffer<uint8_t>::Copy(env, keypair->secret_key.data(), SECRET_KEY_SIZE));
    
    return result;
}

Napi::Object GenerateSigningKeypair(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    auto keypair = KeyManagement::generate_signing_keypair();
    if (!keypair) {
        Napi::Error::New(env, "Failed to generate signing keypair").ThrowAsJavaScriptException();
        return Napi::Object::New(env);
    }
    
    Napi::Object result = Napi::Object::New(env);
    result.Set("publicKey", 
        Napi::Buffer<uint8_t>::Copy(env, keypair->public_key.data(), SIGNING_PUBLIC_KEY_SIZE));
    result.Set("secretKey", 
        Napi::Buffer<uint8_t>::Copy(env, keypair->secret_key.data(), SIGNING_SECRET_KEY_SIZE));
    
    return result;
}

Napi::Value DeriveSharedSecret(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2 || !info[0].IsBuffer() || !info[1].IsBuffer()) {
        Napi::TypeError::New(env, "Expected two buffers (secretKey, publicKey)").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    Napi::Buffer<uint8_t> secret_buf = info[0].As<Napi::Buffer<uint8_t>>();
    Napi::Buffer<uint8_t> public_buf = info[1].As<Napi::Buffer<uint8_t>>();
    
    if (secret_buf.Length() != SECRET_KEY_SIZE || public_buf.Length() != PUBLIC_KEY_SIZE) {
        Napi::TypeError::New(env, "Invalid key sizes").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    SecretKey secret_key;
    PublicKey public_key;
    std::copy(secret_buf.Data(), secret_buf.Data() + SECRET_KEY_SIZE, secret_key.begin());
    std::copy(public_buf.Data(), public_buf.Data() + PUBLIC_KEY_SIZE, public_key.begin());
    
    auto shared_secret = KeyExchange::derive_shared_secret(secret_key, public_key);
    if (!shared_secret) {
        Napi::Error::New(env, "Key exchange failed").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    return Napi::Buffer<uint8_t>::Copy(env, shared_secret->data(), SHARED_SECRET_SIZE);
}

Napi::Value Encrypt(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 3 || !info[0].IsBuffer() || !info[1].IsBuffer() || !info[2].IsBuffer()) {
        Napi::TypeError::New(env, "Expected three buffers (plaintext, key, nonce)").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    Napi::Buffer<uint8_t> plaintext_buf = info[0].As<Napi::Buffer<uint8_t>>();
    Napi::Buffer<uint8_t> key_buf = info[1].As<Napi::Buffer<uint8_t>>();
    Napi::Buffer<uint8_t> nonce_buf = info[2].As<Napi::Buffer<uint8_t>>();
    
    if (key_buf.Length() != SYMMETRIC_KEY_SIZE || nonce_buf.Length() != NONCE_SIZE) {
        Napi::TypeError::New(env, "Invalid key or nonce size").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    ByteVector plaintext(plaintext_buf.Data(), plaintext_buf.Data() + plaintext_buf.Length());
    SymmetricKey key;
    Nonce nonce;
    std::copy(key_buf.Data(), key_buf.Data() + SYMMETRIC_KEY_SIZE, key.begin());
    std::copy(nonce_buf.Data(), nonce_buf.Data() + NONCE_SIZE, nonce.begin());
    
    auto ciphertext = SymmetricCrypto::encrypt_aead(plaintext, key, nonce);
    if (!ciphertext) {
        Napi::Error::New(env, "Encryption failed").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    return Napi::Buffer<uint8_t>::Copy(env, ciphertext->data(), ciphertext->size());
}

Napi::Value Decrypt(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 3 || !info[0].IsBuffer() || !info[1].IsBuffer() || !info[2].IsBuffer()) {
        Napi::TypeError::New(env, "Expected three buffers (ciphertext, key, nonce)").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    Napi::Buffer<uint8_t> ciphertext_buf = info[0].As<Napi::Buffer<uint8_t>>();
    Napi::Buffer<uint8_t> key_buf = info[1].As<Napi::Buffer<uint8_t>>();
    Napi::Buffer<uint8_t> nonce_buf = info[2].As<Napi::Buffer<uint8_t>>();
    
    if (key_buf.Length() != SYMMETRIC_KEY_SIZE || nonce_buf.Length() != NONCE_SIZE) {
        Napi::TypeError::New(env, "Invalid key or nonce size").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    ByteVector ciphertext(ciphertext_buf.Data(), ciphertext_buf.Data() + ciphertext_buf.Length());
    SymmetricKey key;
    Nonce nonce;
    std::copy(key_buf.Data(), key_buf.Data() + SYMMETRIC_KEY_SIZE, key.begin());
    std::copy(nonce_buf.Data(), nonce_buf.Data() + NONCE_SIZE, nonce.begin());
    
    auto plaintext = SymmetricCrypto::decrypt_aead(ciphertext, key, nonce);
    if (!plaintext) {
        Napi::Error::New(env, "Decryption failed").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    return Napi::Buffer<uint8_t>::Copy(env, plaintext->data(), plaintext->size());
}

Napi::Value Sign(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2 || !info[0].IsBuffer() || !info[1].IsBuffer()) {
        Napi::TypeError::New(env, "Expected two buffers (message, secretKey)").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    Napi::Buffer<uint8_t> message_buf = info[0].As<Napi::Buffer<uint8_t>>();
    Napi::Buffer<uint8_t> key_buf = info[1].As<Napi::Buffer<uint8_t>>();
    
    if (key_buf.Length() != SIGNING_SECRET_KEY_SIZE) {
        Napi::TypeError::New(env, "Invalid secret key size").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    ByteVector message(message_buf.Data(), message_buf.Data() + message_buf.Length());
    SigningSecretKey secret_key;
    std::copy(key_buf.Data(), key_buf.Data() + SIGNING_SECRET_KEY_SIZE, secret_key.begin());
    
    auto signature = Signing::sign_message(message, secret_key);
    if (!signature) {
        Napi::Error::New(env, "Signing failed").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    return Napi::Buffer<uint8_t>::Copy(env, signature->data(), SIGNATURE_SIZE);
}

Napi::Value Verify(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 3 || !info[0].IsBuffer() || !info[1].IsBuffer() || !info[2].IsBuffer()) {
        Napi::TypeError::New(env, "Expected three buffers (message, signature, publicKey)").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    Napi::Buffer<uint8_t> message_buf = info[0].As<Napi::Buffer<uint8_t>>();
    Napi::Buffer<uint8_t> sig_buf = info[1].As<Napi::Buffer<uint8_t>>();
    Napi::Buffer<uint8_t> key_buf = info[2].As<Napi::Buffer<uint8_t>>();
    
    if (sig_buf.Length() != SIGNATURE_SIZE || key_buf.Length() != SIGNING_PUBLIC_KEY_SIZE) {
        Napi::TypeError::New(env, "Invalid signature or key size").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    ByteVector message(message_buf.Data(), message_buf.Data() + message_buf.Length());
    Signature signature;
    SigningPublicKey public_key;
    std::copy(sig_buf.Data(), sig_buf.Data() + SIGNATURE_SIZE, signature.begin());
    std::copy(key_buf.Data(), key_buf.Data() + SIGNING_PUBLIC_KEY_SIZE, public_key.begin());
    
    bool valid = Signing::verify_signature(message, signature, public_key);
    return Napi::Boolean::New(env, valid);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    if (!utils::initialize()) {
        Napi::Error::New(env, "Failed to initialize crypto library").ThrowAsJavaScriptException();
        return exports;
    }
    
    exports.Set("generateKeypair", Napi::Function::New(env, GenerateKeypair));
    exports.Set("generateSigningKeypair", Napi::Function::New(env, GenerateSigningKeypair));
    exports.Set("deriveSharedSecret", Napi::Function::New(env, DeriveSharedSecret));
    exports.Set("encrypt", Napi::Function::New(env, Encrypt));
    exports.Set("decrypt", Napi::Function::New(env, Decrypt));
    exports.Set("sign", Napi::Function::New(env, Sign));
    exports.Set("verify", Napi::Function::New(env, Verify));
    
    return exports;
}

NODE_API_MODULE(spear_addon, Init)