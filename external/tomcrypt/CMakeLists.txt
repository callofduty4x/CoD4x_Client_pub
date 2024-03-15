cmake_minimum_required(VERSION 3.5)
project(tomcrypt VERSION 1.0.0 LANGUAGES C)
include_guard()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(tomcrypt STATIC)

file(GLOB SRC_HASHES ${PROJECT_SOURCE_DIR}/src/hashes/*.c)
file(GLOB SRC_HMAC ${PROJECT_SOURCE_DIR}/src/mac/hmac/*.c)
file(GLOB SRC_MATH ${PROJECT_SOURCE_DIR}/src/math/*.c)
file(GLOB SRC_MISC_CRYPT ${PROJECT_SOURCE_DIR}/src/misc/crypt/*.c)
file(GLOB SRC_MISC_BASE64 ${PROJECT_SOURCE_DIR}/src/misc/base64/*.c)
file(GLOB SRC_MISC_PKCS5 ${PROJECT_SOURCE_DIR}/src/misc/pkcs5/*.c)
file(GLOB SRC_MISC ${PROJECT_SOURCE_DIR}/src/misc/*.c)
file(GLOB SRC_PK_ASN1_DER_SEQUENCE ${PROJECT_SOURCE_DIR}/src/pk/asn1/der/sequence/*.c)
file(GLOB SRC_PK_ASN1_DER_BIT ${PROJECT_SOURCE_DIR}/src/pk/asn1/der/bit/*.c)
file(GLOB SRC_PK_ASN1_DER_BOOLEAN ${PROJECT_SOURCE_DIR}/src/pk/asn1/der/boolean/*.c)
file(GLOB SRC_PK_ASN1_DER_CHOICE ${PROJECT_SOURCE_DIR}/src/pk/asn1/der/choice/*.c)
file(GLOB SRC_PK_ASN1_DER_IA5 ${PROJECT_SOURCE_DIR}/src/pk/asn1/der/ia5/*.c)
file(GLOB SRC_PK_ASN1_DER_INTEGER ${PROJECT_SOURCE_DIR}/src/pk/asn1/der/integer/*.c)
file(GLOB SRC_PK_ASN1_DER_OBJECT_IDENTIFIER ${PROJECT_SOURCE_DIR}/src/pk/asn1/der/object_identifier/*.c)
file(GLOB SRC_PK_ASN1_DER_OCTET ${PROJECT_SOURCE_DIR}/src/pk/asn1/der/octet/*.c)
file(GLOB SRC_PK_ASN1_DER_PRINTABLE_STRING ${PROJECT_SOURCE_DIR}/src/pk/asn1/der/printable_string/*.c)
file(GLOB SRC_PK_ASN1_DER_SHORT_INTEGER ${PROJECT_SOURCE_DIR}/src/pk/asn1/der/short_integer/*.c)
file(GLOB SRC_PK_ASN1_DER_UTCTIME ${PROJECT_SOURCE_DIR}/src/pk/asn1/der/utctime/*.c)
file(GLOB SRC_PK_ASN1_DER_UTF8 ${PROJECT_SOURCE_DIR}/src/pk/asn1/der/utf8/*.c)
file(GLOB SRC_PK_ASN1_DER_SET ${PROJECT_SOURCE_DIR}/src/pk/asn1/der/set/*.c)
file(GLOB SRC_RSA ${PROJECT_SOURCE_DIR}/src/pk/rsa/*.c)
file(GLOB SRC_PKCS1 ${PROJECT_SOURCE_DIR}/src/pk/pkcs1/*.c)

target_sources(tomcrypt PRIVATE
    src/ciphers/aes/aes.c
    ${SRC_HASHES}
    ${SRC_HMAC}
    ${SRC_MATH}
    ${SRC_MISC_CRYPT}
    ${SRC_MISC_BASE64}
    ${SRC_MISC_PKCS5}
    ${SRC_MISC}
    ${SRC_PK_ASN1_DER_SEQUENCE}
    ${SRC_PK_ASN1_DER_BIT}
    ${SRC_PK_ASN1_DER_BOOLEAN}
    ${SRC_PK_ASN1_DER_CHOICE}
    ${SRC_PK_ASN1_DER_IA5}
    ${SRC_PK_ASN1_DER_INTEGER}
    ${SRC_PK_ASN1_DER_OBJECT_IDENTIFIER}
    ${SRC_PK_ASN1_DER_OCTET}
    ${SRC_PK_ASN1_DER_PRINTABLE_STRING}
    ${SRC_PK_ASN1_DER_SHORT_INTEGER}
    ${SRC_PK_ASN1_DER_UTCTIME}
    ${SRC_PK_ASN1_DER_UTF8}
    ${SRC_PK_ASN1_DER_SET}
    ${SRC_RSA}
    ${SRC_PKCS1}
)

target_include_directories(tomcrypt
PUBLIC
    src/math
    include
PRIVATE
    include/tomcrypt
)

target_compile_definitions(tomcrypt PRIVATE
    LTC_NO_ROLC
    LTC_SOURCE
    LTC_NO_TEST
)
target_compile_options(tomcrypt PRIVATE -m32 -std=c11 -Os)
target_link_options(tomcrypt PRIVATE -m32)

#set_source_files_properties(${SRC_MATH} PROPERTIES COMPILE_DEFINITIONS -Imath -ULTC_NO_TEST)
