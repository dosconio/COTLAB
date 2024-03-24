udir_win = ../unisym
udir_lin = /mnt/hgfs/unisym
VERS  = 0.0.3.0
CX32 = g++ -m32 -D_DEBUG 
CX64 = g++ -m64 -D_DEBUG 

builds = ./src/*.cpp

#{TODO} make PPA of unisym and rely on it. Now is only for 64

lin64:
	sudo ${CX64} -D_Linux $(builds) -I$(udir_lin)/inc -L../_bin -ll64 -s -O3 -o /usr/bin/cot -Wno-unused-result

lin32:
	sudo ${CX32} -D_Linux $(builds) -I$(udir_lin)/inc -L../_bin -ll32 -s -O3 -o /usr/bin/cot -Wno-unused-result

win32:
	${CX32} -D_WinNT $(builds) -I$(udir_win)/inc -L../_bin -lw32 -s -O3 -o "../_bin/cot.exe"

debian:
	mkdir -pv ./.deb/COTLAB/src
	mkdir -pv ./.deb/COTLAB/inc
	mkdir -pv ./.deb/COTLAB/inc/c
	mkdir -pv ./.deb/COTLAB/inc/cpp
	cp ./src/* ./.deb/COTLAB/src
	cp ./inc/* ./.deb/COTLAB/inc
	cp ./Makefile.debian ./.deb/COTLAB/Makefile
	cp ../_bin/libl32.a ./.deb/COTLAB/libl32
	cp ../_bin/libl64.a ./.deb/COTLAB/libl64
	cd ../unisym/inc/cpp && cp cinc coear dnode floating inode nnode node nodes tnode ../../../COTLAB/.deb/COTLAB/inc/cpp
	cd ../unisym/inc/c && cp aldbg.h alice.h coear.h consio.h dnode.h host.h inode.h nnode.h node.h tnode.h uctype.h ustring.h ../../../COTLAB/.deb/COTLAB/inc/c
	cp README.md ./.deb/COTLAB/debian/README
