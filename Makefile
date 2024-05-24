out/helium: helium.h helium.c lib.h lib.c hmc.c hmc.h main.c
	clang main.c lib.c helium.c hmc.c -o out/helium -lm

test:
	clang test.c lib.c helium.c hmc.c -o test -lm

clean: test out/helium
	rm test out/helium