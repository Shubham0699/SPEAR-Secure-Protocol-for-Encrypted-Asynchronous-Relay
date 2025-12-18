cmake_minimum_required(VERSION 3.15)

# Crypto Core Library
add_library(spear_crypto
    src/types.cpp
    src/utils.cpp
    src/key_management.cpp
    src/key_exchange.cpp
    src/symmetric_crypto.cpp
    src/signing.cpp
    src/streaming.cpp
)

target_include_directories(spear_crypto
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${SODIUM_INCLUDE_DIRS}
)

target_link_libraries(spear_crypto
    PUBLIC
        ${SODIUM_LIBRARIES}
)