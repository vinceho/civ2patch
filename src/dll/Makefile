all: ../../out/civ2patch.dll

version.ro: version.rc
	windres version.rc version.ro

*.o: *.cpp
	g++ -O3 -c -DBUILD_DLL $^

../../out/civ2patch.dll: *.o version.ro
	g++ -shared -static-libgcc -static-libstdc++ -O3 -o $@ $^ -lwinmm -lsdl2 -lsdl2_mixer

clean:
	rm -f version.ro *.o
