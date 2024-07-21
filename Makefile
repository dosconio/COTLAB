udir_win = ../unisym
udir_lin = /mnt/hgfs/unisym
VERS  = 0.0.3.0
CX32 = g++ -m32 -D_DEBUG 
CX64 = g++ -m64 -D_DEBUG 

builds = ./src/*.cpp

.PHONY: updhead

#{TODO} make PPA of unisym and rely on it. Now is only for 64

lin64: updhead
	sudo ${CX64} -D_Linux64 $(builds) -I$(udir_lin)/inc -L../_bin -ll64d -s -O3 -o /usr/bin/cot -Wno-unused-result

lin32: updhead
	sudo ${CX32} -D_Linux32 $(builds) -I$(udir_lin)/inc -L../_bin -ll32d -s -O3 -o /usr/bin/cot -Wno-unused-result

win32: updhead
	windres -i ./inc/resources.rc -o ../_obj/cotres.obj
	${CX32} -D_Win32 $(builds) ../_obj/cotres.obj -I$(udir_win)/inc -L../_bin -lw32d -s -O3 -o "../_bin/cot.exe"

debian: updhead
	mkdir -pv ./.deb/COTLAB/src
	mkdir -pv ./.deb/COTLAB/inc
	mkdir -pv ./.deb/COTLAB/inc/c
	mkdir -pv ./.deb/COTLAB/inc/cpp
	cp ./src/* ./.deb/COTLAB/src
	cp ./inc/* ./.deb/COTLAB/inc
	cp ./Makefile.debian ./.deb/COTLAB/Makefile
	cp ../_bin/libl32.a ./.deb/COTLAB/libl32
	cp ../_bin/libl64.a ./.deb/COTLAB/libl64
	cp README.md ./.deb/COTLAB/debian/README

updhead:
	cd ../unisym/inc/cpp && cp cinc coear dnode floating inode nnode node nodes tnode ../../../COTLAB/.deb/COTLAB/inc/cpp
	cd ../unisym/inc/c && cp debug.h integer.h floating.h archit.h alice.h coear.h consio.h dnode.h host.h inode.h nnode.h node.h tnode.h uctype.h ustring.h ../../../COTLAB/.deb/COTLAB/inc/c
