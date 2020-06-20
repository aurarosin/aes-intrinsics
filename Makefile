run: aes-intrinsics.o
	./aes-intrinsics.o

aes-intrinsics.o: aes-intrinsics.c
	gcc -o aes-intrinsics.o aes-intrinsics.c -maes

.PHONY: run