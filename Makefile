usym = ../unisym/inc/c/
ulib = ../unisym/lib/c/
depend_usym = $(ulib)datime.c $(ulib)consio.c $(ulib)numar.c $(ulib)cdear.c $(ulib)ustring.c $(ulib)hstring.c
builds = cotlab.c erro.c parser.c executor.c dtrlib.c dtr_float.c dtrfunc.c extfunc.c
# Windows
w64gcc:
	windres -i resources.rc -o ../_obj/cotres.obj
	gcc "../_obj/cotres.obj" $(builds) $(depend_usym) -m64 -I $(usym) -s -O3 -o "../_bin/COTLAB.x64.exe" -D_dbg -D_WinNT
	cd ../_bin/ && copy COTLAB.x64.exe cot.exe
win64:
	gcc $(builds) $(depend_usym) -m64 -I $(usym) -s -O3 -o "../_bin/COTLAB.x64.exe" -D_dbg -D_WinNT
win32:
	gcc $(builds) $(depend_usym) -m32 -I $(usym) -s -O3 -o "../_bin/COTLAB.x86.exe" -D_dbg -D_WinNT
lin64:
	gcc $(builds) $(depend_usym) -m64 -I $(usym) -s -O3 -o "../_bin/COTx64" -D_dbg -D_Linux -lm -Wno-unused-result
	cd ../_bin/ && cp COTx64 ~/cot
l:
	gcc $(builds) $(depend_usym) -I $(usym) -s -O3 -o "../_bin/cot" -D_dbg -D_Linux -lm -w
clean:
	del *.o