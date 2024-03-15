cmake_minimum_required(VERSION 3.10)
project(mbedtls VERSION 1.0.0 LANGUAGES C)
include_guard()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SRC_CRYPTO
    src/aes.c
    src/aesni.c
    src/arc4.c
    src/asn1parse.c
    src/asn1write.c
    src/base64.c
    src/bignum.c
    src/blowfish.c
    src/camellia.c
    src/ccm.c
    src/cipher.c
    src/cipher_wrap.c
    src/ctr_drbg.c
    src/des.c
    src/dhm.c
    src/ecdh.c
    src/ecdsa.c
    src/ecjpake.c
    src/ecp.c
    src/ecp_curves.c
    src/entropy.c
    src/entropy_poll.c
    src/error.c
    src/gcm.c
    src/havege.c
    src/hmac_drbg.c
    src/md.c
    src/md2.c
    src/md4.c
    src/md5.c
    src/md_wrap.c
    src/memory_buffer_alloc.c
    src/oid.c
    src/padlock.c
    src/pem.c
    src/pk.c
    src/pk_wrap.c
    src/pkcs12.c
    src/pkcs5.c
    src/pkparse.c
    src/pkwrite.c
    src/platform.c
    src/ripemd160.c
    src/rsa.c
    src/sha1.c
    src/sha256.c
    src/sha512.c
    src/threading.c
    src/timing.c
    src/version.c
    src/version_features.c
    src/xtea.c
)

set(SRC_X509
    src/certs.c
    src/pkcs11.c
    src/x509.c
    src/x509_create.c
    src/x509_crl.c
    src/x509_crt.c
    src/x509_csr.c
    src/x509write_crt.c
    src/x509write_csr.c
)

set(SRC_TLS
    src/debug.c
    src/net.c
    src/ssl_cache.c
    src/ssl_ciphersuites.c
    src/ssl_cli.c
    src/ssl_cookie.c
    src/ssl_srv.c
    src/ssl_ticket.c
    src/ssl_tls.c
)

add_library(mbedtls STATIC
    ${SRC_CRYPTO}
    ${SRC_X509}
    ${SRC_TLS}
)

target_include_directories(mbedtls PUBLIC
    include
)

target_compile_options(mbedtls PRIVATE -m32 -std=c11)
target_link_options(mbedtls PRIVATE -m32)

