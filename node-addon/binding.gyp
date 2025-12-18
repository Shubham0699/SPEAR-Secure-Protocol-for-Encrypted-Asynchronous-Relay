{
  "targets": [
    {
      "target_name": "spear_addon",
      "sources": [
        "src/addon.cpp"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "../crypto-core/include"
      ],
      "libraries": [
        "/home/shubh/C++-project/SPEAR/build/crypto-core/libspear_crypto.a",
        "-lsodium"
      ],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "cflags_cc": [
        "-std=c++17"
      ]
    }
  ]
}