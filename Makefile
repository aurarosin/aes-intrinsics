PROJ=AES

build: $(PROJ)

run: build
	./$(PROJ)

$(PROJ): main.c lib/aes.o lib/aes-intrinsics.o lib/utils.o
	gcc main.c lib/aes.o lib/aes-intrinsics.o lib/utils.o -o $(PROJ) -O2 -Iinclude
	@echo "Compilado correctamente."

lib/aes-intrinsics.o: lib/aes-intrinsics.c include/aes-intrinsics.h
	gcc lib/aes-intrinsics.c -o lib/aes-intrinsics.o -O2 -c -maes -mavx -Iinclude

lib/aes.o: lib/aes.c include/aes.h
	gcc lib/aes.c -o lib/aes.o -O2 -c -Iinclude

lib/utils.o: lib/utils.c include/utils.h
	gcc lib/utils.c -o lib/utils.o -O2 -c -Iinclude

clean:
	rm -rf **/*.o CMakeFiles/ *.cmake CMakeCache.txt *.a AES

.PHONY: build run clean