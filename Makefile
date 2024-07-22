out/helium: src/helium.h src/helium.c src/lib.h src/lib.c src/hmc.c src/hmc.h src/main.c
	cd src
	clang src/main.c src/lib.c src/helium.c src/hmc.c -o out/helium -lm

test:
	clang test.c lib.c helium.c hmc.c -o test -lm

clean: test out/helium
	rm test out/helium