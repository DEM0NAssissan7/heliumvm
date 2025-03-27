compiler=gcc

out:

out/helium: src/helium.h src/helium.c src/lib.h src/lib.c src/helium-main.c
	mkdir -p out && echo "*" > out/.gitignore
	$(compiler) src/helium-main.c src/lib.c src/helium.c -g -o out/helium -lm

out/hmc: src/hmc.c src/hmc.h src/hmc-main.c src/lib.h src/lib.c src/helium.c src/helium.h
	mkdir -p out && echo "*" > out/.gitignore
	$(compiler) src/hmc.c src/helium.c src/lib.c src/hmc-main.c -g -o out/hmc -lm

out/hlc:
	mkdir -p out && echo "*" > out/.gitignore
	$(compiler) src/compiler.c src/vars.c src/instr.c src/helium.c src/lib.c src/hmc.c src/hlc-main.c -g -o out/hlc -lm

all:
	make out/helium
	make out/hmc
	make out/hlc

clean:
	rm -rf out