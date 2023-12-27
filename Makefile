usym = ../unisym/inc/c/
ulib = ../unisym/lib/c/

# -l parameters should be set at last for linkage order
CC32 = gcc

builds = cotlab.c erro.c parser.c executor.c dtrlib.c dtrar.c
# Windows
w64gcc:
	windres -i resources.rc -o ../_obj/cotres.obj
	gcc "../_obj/cotres.obj" $(builds) $(depend_usym) -m64 -I $(usym) -s -O3 -o "../_bin/COTLAB.x64.exe" -D_dbg -D_WinNT
	cd ../_bin/ && copy COTLAB.x64.exe cot.exe
win64:
	gcc $(builds) $(depend_usym) -m64 -I $(usym) -s -O3 -o "../_bin/COTLAB.x64.exe" -D_dbg -D_WinNT
win32:
	${CC32} $(builds) -m32 -I $(usym) -L../_bin -lw32d -s -O3 -o "../_bin/cot.exe" -D_DEBUG -D_WinNT
lin64:
	gcc $(builds) $(depend_usym) -m64 -I $(usym) -s -O3 -o "../_bin/COTx64" -D_dbg -D_Linux -lm -Wno-unused-result
	cd ../_bin/ && cp COTx64 ~/cot
l:
	gcc $(builds) $(depend_usym) -I $(usym) -s -O3 -o "../_bin/cot" -D_dbg -D_Linux -lm -w
clean:
	del *.o