project(tomcrypt)

file(GLOB SRC 
    ciphers/aes/aes.c
    hashes/*.c
    misc/crypt/*.c
    misc/base64/*.c
    misc/pkcs5/*.c
    misc/*.c
    mac/hmac/*.c
    pk/asn1/der/sequence/*.c
    pk/asn1/der/bit/*.c
    pk/asn1/der/boolean/*.c
    pk/asn1/der/choice/*.c
    pk/asn1/der/ia5/*.c
    pk/asn1/der/integer/*.c
    pk/asn1/der/object_identifier/*.c
    pk/asn1/der/octet/*.c
    pk/asn1/der/printable_string/*.c
    pk/asn1/der/sequence/*.c
    pk/asn1/der/short_integer/*.c
    pk/asn1/der/utctime/*.c
    pk/asn1/der/utf8/*.c
    pk/asn1/der/set/*.c
    pk/rsa/*.c
    pk/pkcs1/*.c
)

add_library(${PROJECT_NAME} STATIC ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE .)
target_compile_definitions(${PROJECT_NAME} PRIVATE LTC_NO_ROLC LTC_SOURCE LTC_NO_TEST)

###############################################################################
project(tommath)

file(GLOB SRC math/*.c)

add_library(${PROJECT_NAME} STATIC ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE . math)
target_compile_definitions(${PROJECT_NAME} PRIVATE LTC_NO_ROLC LTC_SOURCE)
target_link_libraries(${PROJECT_NAME} tomcrypt)
