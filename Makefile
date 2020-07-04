PROJ=AES
SOURCES=lib/aes.o lib/ccm.o lib/gcm.o lib/utils.o lib/aes-intrinsics.o lib/gcm-intrinsics.o lib/utils-intrinsics.o
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
	gcc $(CFLAGS) -o $@ $< $(SOURCES)
	@echo "Compilado correctamente."

lib/aes.o: lib/aes.c include/aes.h
	gcc $(CFLAGS) -o $@ $< -c

lib/ccm.o: lib/ccm.c include/ccm.h
	gcc $(CFLAGS) -o $@ $< -c

lib/gcm.o: lib/gcm.c include/gcm.h
	gcc $(CFLAGS) -o $@ $< -c

lib/utils.o: lib/utils.c include/utils.h
	gcc $(CFLAGS) -o $@ $< -c

lib/aes-intrinsics.o: lib/aes-intrinsics.c include/aes-intrinsics.h
	gcc $(CFLAGS) -o $@ $< -c -maes -mavx

lib/utils-intrinsics.o: lib/utils-intrinsics.c include/utils-intrinsics.h
	gcc $(CFLAGS) -o $@ $< -c -mpclmul -mavx

lib/gcm-intrinsics.o: lib/gcm-intrinsics.c include/gcm-intrinsics.h
	gcc $(CFLAGS) -o $@ $< -c -maes -mavx

lib/sha-intrinsics.o: lib/sha-intrinsics.c include/sha-intrinsics.h
	gcc $(CFLAGS) -o $@ $< -c -msha -mavx
