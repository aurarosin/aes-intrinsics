PROJ=AES
SOURCES=lib/aes-intrinsics.o lib/utils.o lib/aes.o lib/ccm.o
CFLAGS=-O2 -Iinclude

build: $(PROJ)

run: build
	./$(PROJ)

clean:
	rm -rf *.o **/*.o CMakeFiles/ *.cmake CMakeCache.txt *.a AES

test:
	openssl aes-128-cbc -in test/in.dat -out test/out.dat -K 31323334353637383930313233343536 -iv 00000000000000000000000000000000

.PHONY: build run clean test

$(PROJ): main.c $(SOURCES)
	gcc $(CFLAGS) -o $@ main.c $(SOURCES)
	@echo "Compilado correctamente."

lib/aes-intrinsics.o: lib/aes-intrinsics.c include/aes-intrinsics.h
	gcc $(CFLAGS) -o $@ lib/aes-intrinsics.c -c -maes -mavx

lib/aes.o: lib/aes.c include/aes.h
	gcc $(CFLAGS) -o $@ lib/aes.c -c

lib/utils.o: lib/utils.c include/utils.h
	gcc $(CFLAGS) -o $@ lib/utils.c -c

lib/ccm.o: lib/ccm.c include/ccm.h
	gcc $(CFLAGS) -o $@ lib/ccm.c -c
