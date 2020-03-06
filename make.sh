#!/usr/bin/env bash

if [ $# -ne 3 ]
  then
    echo "Usage: ./make.sh CC CFLAGS AR"
	exit 1
fi

echo "Making mpn"

cd mpn

rm -rf *.o

for filename in *.c; do
    name=`basename $filename .c`

    $1 \
        --target=wasm32-unknown-wasi \
        $2 \
        -DHAVE_CONFIG_H -D__GMP_WITHIN_GMP -DOPERATION_$name \
        -I.. \
        -c -o "$name".o \
        "$name".c
done

cd ../mpz/

echo "Making mpz"

rm -rf *.o

for filename in *.c; do
    name=`basename $filename .c`

    $1 \
        --target=wasm32-unknown-wasi \
        $2 \
        -DHAVE_CONFIG_H -D__GMP_WITHIN_GMP -DOPERATION_$name \
        -I.. \
        -c -o "$name".o \
        "$name".c
done

cd ..

echo "Making in top level"

rm -rf *.o

declare -a topLevelSources=("assert.c" "errno.c" "extract-dbl.c" "memory.c" "mp_bpl.c"
                            "mp_clz_tab.c" "mp_dv_tab.c" "mp_get_fns.c"
                            "mp_minv_tab.c" "mp_set_fns.c" "nextprime.c" "tal-reent.c")

for filename in "${topLevelSources[@]}"; do
    name=`basename $filename .c`

    $1 \
        --target=wasm32-unknown-wasi \
        $2 \
        -DHAVE_CONFIG_H -D__GMP_WITHIN_GMP \
        -I. \
        -c -o "$name".o \
        "$name".c
done

echo "Making archive file"

$3 crs libwarm.a *.o mpz/*.o mpn/*.o
