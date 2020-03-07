# WARM - WebAssembly Arbitrary Precision Math
A simplified version of the GMP library.

## How to build it?
By using the **make** script. \
`./make.sh CC CFLAGS AR` \
What does this mean?
1. CC - the C compiler
2. CFLAGS - the C flags you want to use (e.g., the path to your sysroot or the optimization flags). This must be given between quotation marks and separated by spaces.
3. AR - the archiver, because the script aims to produce a single archive library

Example: `./make.sh clang "--sysroot=/home/botond/Asztal/gmp/wasi-libc/sysroot -O2 -flto" llvm-ar`