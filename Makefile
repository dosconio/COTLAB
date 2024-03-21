uinc = ../unisym/inc
ulib = ../unisym/lib/c/

# -l parameters should be set at last for linkage order
CC32 = gcc -m32 -D_DEBUG -D_WinNT
CX32 = g++ -m32 -D_DEBUG -D_WinNT

builds = ./src/*.cpp

win32:
	# cd tmp && ${CX32} -c $(builds) -I$(uinc)/c -I$(uinc)/cpp
	${CX32} $(builds) -I$(uinc)/c -I$(uinc)/cpp -L../_bin -lw32 -s -O3 -o "../_bin/cot.exe"
# ---------------------
# Windows
w64gcc:
	windres -i resources.rc -o ../_obj/cotres.obj
	gcc "../_obj/cotres.obj" $(builds) $(depend_usym) -m64 -I $(uinc) -s -O3 -o "../_bin/COTLAB.x64.exe" -D_dbg -D_WinNT
	cd ../_bin/ && copy COTLAB.x64.exe cot.exe
win64:
	gcc $(builds) $(depend_usym) -m64 -I $(uinc) -s -O3 -o "../_bin/COTLAB.x64.exe" -D_dbg -D_WinNT

lin64:
	gcc $(builds) $(depend_usym) -m64 -I $(uinc) -s -O3 -o "../_bin/COTx64" -D_dbg -D_Linux -lm -Wno-unused-result
	cd ../_bin/ && cp COTx64 ~/cot
l:
	gcc $(builds) $(depend_usym) -I $(uinc) -s -O3 -o "../_bin/cot" -D_dbg -D_Linux -lm -w
clean:
	del *.o