cp ../*.c .
rm main_nix.c
for f in *.c; do mv "$f" "${f%.c}.ino"; done
cp ../*.h .