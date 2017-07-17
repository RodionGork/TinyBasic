cp ../core/*.c .
for f in *.c; do mv "$f" "${f%.c}.cpp"; done
cp ../core/*.h .