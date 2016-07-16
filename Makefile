all: out
	cd src/dll; mingw32-make

out:
	mkdir -p out

clean:
	rm -rf out
	cd src/dll; mingw32-make clean
