
VERS  =0.0.2.0

builds = ./src/*.cpp
warns = -Wno-unused-result -w
cxxdef = -D_DEBUG -D_PROPERTY_STRING_OFF -D_INC_STDLIB

CX32 = g++ -m32 -s -O3 -I$(uincpath) -L$(ubinpath) $(cxxdef)
CX64 = g++ -m64 -s -O3 -I$(uincpath) -L$(ubinpath) $(cxxdef)

.PHONY: lin64 lin32 win32 debian

#{TODO} make PPA of unisym and rely on it. Now is only for 64

lin64:
	sudo ${CX64} -D_Linux64 $(builds) -ll64d -o /usr/bin/cot $(warns)

lin32:
	sudo ${CX32} -D_Linux32 $(builds) -ll32d -o /usr/bin/cot $(warns)

win32:
	windres -i ./inc/resources.rc -o ../_obj/cotres.obj
	${CX32} -D_Win32 $(builds) ../_obj/cotres.obj -I$(udir_win)/inc -L../_bin -lw32d -s -O3 -o "../_bin/cot.exe"

debian: 
	-rm -rf .deb
	mkdir -pv .deb/COTLAB/inc/c
	mkdir -pv .deb/COTLAB/inc/cpp
	mkdir -pv .deb/COTLAB/src
	cd .deb/COTLAB && dh_make -p cotlab_$(VERS) --single --native --copyright gpl3 --email m360xc@outlook.com
	mkdir -pv .deb/COTLAB/debian
	cp ./src/*.cpp .deb/COTLAB/src
	cp ./inc/*.h   .deb/COTLAB/inc
	cp ./Makefile.debian .deb/COTLAB/Makefile
	cp $(uincpath)/c/*   .deb/COTLAB/inc/c   -r
	cp $(uincpath)/cpp/* .deb/COTLAB/inc/cpp -r
	cp $(ubinpath)/libl32d.a .deb/COTLAB/libl32d
	cp $(ubinpath)/libl64d.a .deb/COTLAB/libl64d
	cp README.md .deb/COTLAB/debian/README
	cd .deb/COTLAB && rm -rf debian/*.ex
	cd .deb/COTLAB && rm -rf debian/*.EX
	cd .deb/COTLAB && perl -i -pe "s/UNRELEASED/$(shell lsb_release -cs)/" debian/changelog
	cd .deb/COTLAB && perl -i -pe "s/unknown/utils/" debian/control
	cd .deb/COTLAB && debuild -S -k97FF73B5BC52522780BF77AE240E8351D1A85F13 | tee /tmp/debuild.log 2>&1
debtest:
	cd .deb && dput ppa:dosconio/test *.changes
debrelease:
	cd .deb && dput ppa:dosconio/cot *.changes




