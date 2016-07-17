# This file is part of Civ 2 MGE Patch.
# Copyright (C) 2016 Vincent Ho
#
# Civ 2 MGE Patch is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Civ 2 MGE Patch is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Civ 2 MGE Patch.  If not, see <http://www.gnu.org/licenses/>.
#
all: ../../out/civ2patch.dll

version.ro: version.rc
	windres version.rc version.ro

*.o: *.cpp
	g++ -O3 -c -DBUILD_DLL $^

../../out/civ2patch.dll: *.o version.ro
	g++ -shared -static-libgcc -static-libstdc++ -O3 -o $@ $^ -lwinmm

clean:
	rm -f version.ro *.o
