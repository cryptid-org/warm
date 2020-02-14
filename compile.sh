#!/bin/bash

echo "Compiling mpn"

cd mpn

rm -rf *.o

# -D__GMP_WITHIN_GMP -I$(top_srcdir) -DOPERATION_`echo $* | sed 's/_$$//'`

for filename in *.c; do
    name=`basename $filename .c`

    gcc -DHAVE_CONFIG_H -D__GMP_WITHIN_GMP -I.. -DOPERATION_$name -c -o "$name".o "$name".c

done

cd ../mpz/

echo "Compiling mpz"

rm -rf *.o

for filename in *.c; do
    name=`basename $filename .c`

    gcc -DHAVE_CONFIG_H -D__GMP_WITHIN_GMP -I.. -c -o "$name".o "$name".c

done

cd ..

echo "Top level"

rm -rf *.o

declare -a topLevelSources=("assert.c" "errno.c" "extract-dbl.c" "memory.c" "mp_bpl.c"
                            "mp_clz_tab.c" "mp_dv_tab.c" "mp_get_fns.c"
                            "mp_minv_tab.c" "mp_set_fns.c" "nextprime.c" "tal-reent.c")

for filename in "${topLevelSources[@]}"; do
    name=`basename $filename .c`

    gcc -DHAVE_CONFIG_H -D__GMP_WITHIN_GMP -I.. -c -o "$name".o "$name".c

done

ar crs libgmp.a *.o mpz/*.o mpn/*.o
# gcc -I . ./mpn/*.c ./mpz/*.c assert.c errno.c extract-dbl.c memory.c mp_bpl.c \
# mp_clz_tab.c mp_dv_tab.c mp_get_fns.c \
# mp_minv_tab.c mp_set_fns.c nextprime.c tal-notreent.c tal-reent.c -c