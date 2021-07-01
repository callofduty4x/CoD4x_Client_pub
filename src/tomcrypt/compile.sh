#!/bin/sh

CXX=i686-w64-mingw32-gcc
FLAGS="-m32 -Wall -Os -s -c -I. -DLTC_NO_ROLC -DLTC_SOURCE -DLTC_NO_TEST"
MATHFLAGS="-m32 -Wall -Os -s -c -I. -Imath -DLTC_NO_ROLC -DLTC_SOURCE"

# -Wsign-compare -W -Wshadow -Wno-unused-parameter-Wsystem-headers -Wdeclaration-after-statement -Wbad-function-cast -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wpointer-arith
$CXX $FLAGS ciphers/aes/aes.c || exit 1
$CXX $FLAGS hashes/*.c || exit 1
$CXX $FLAGS misc/crypt/*.c || exit 1
$CXX $FLAGS misc/base64/*.c || exit 1
$CXX $FLAGS misc/pkcs5/*.c || exit 1
$CXX $FLAGS misc/*.c || exit 1
$CXX $FLAGS mac/hmac/*.c || exit 1
$CXX $FLAGS pk/asn1/der/sequence/*.c || exit 1
$CXX $FLAGS pk/asn1/der/bit/*.c || exit 1
$CXX $FLAGS pk/asn1/der/boolean/*.c || exit 1
$CXX $FLAGS pk/asn1/der/choice/*.c || exit 1
$CXX $FLAGS pk/asn1/der/ia5/*.c || exit 1
$CXX $FLAGS pk/asn1/der/integer/*.c || exit 1
$CXX $FLAGS pk/asn1/der/object_identifier/*.c || exit 1
$CXX $FLAGS pk/asn1/der/octet/*.c || exit 1
$CXX $FLAGS pk/asn1/der/printable_string/*.c || exit 1
$CXX $FLAGS pk/asn1/der/sequence/*.c || exit 1
$CXX $FLAGS pk/asn1/der/short_integer/*.c || exit 1
$CXX $FLAGS pk/asn1/der/utctime/*.c || exit 1
$CXX $FLAGS pk/asn1/der/utf8/*.c || exit 1
$CXX $FLAGS pk/asn1/der/set/*.c || exit 1
$CXX $FLAGS pk/rsa/*.c || exit 1
$CXX $FLAGS pk/pkcs1/*.c || exit 1
$CXX $MATHFLAGS math/*.c || exit 1

i686-w64-mingw32-ar cr ../../lib/libtomcrypt.a *.o || exit 1

rm -f *.o

