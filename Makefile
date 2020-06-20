build: main.o

run: build
	./main.o

main.o: main.c aes.o aes-intrinsics.o
	gcc main.c aes.o aes-intrinsics.o -o main.o -O2

aes-intrinsics.o: aes-intrinsics.c aes-intrinsics.h
	gcc aes-intrinsics.c -o aes-intrinsics.o -O2 -c -maes -mavx

aes.o: aes.c aes.h
	gcc aes.c -o aes.o -O2 -c

clean:
	rm *.o

.PHONY: build run clean