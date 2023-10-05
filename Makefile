usym = ../unisym/c/
depend_usym = $(usym)source/datime.c $(usym)source/consio.c $(usym)source/numar.c $(usym)source/cdear.c $(usym)source/ustring.c $(usym)source/hstring.c
builds = cotlab.c erro.c parser.c executor.c dtrlib.c dtr_float.c dtrfunc.c
# Windows
w64gcc:
	windres -i resources.rc -o ../_obj/cotres.obj
	gcc "../_obj/cotres.obj" $(builds) $(depend_usym) -m64 -I $(usym) -s -O3 -o "../_bin/COTLAB.x64.exe" -D_dbg -D_WinNT
	cd ../_bin/ && copy COTLAB.x64.exe cot.exe
win64:
	gcc $(builds) $(depend_usym) -m64 -I $(usym) -s -O3 -o "../_bin/COTLAB.x64.exe" -D_dbg -D_WinNT
	cd ../_bin/ && copy COTLAB.x64.exe cot.exe
lin64:
	gcc $(builds) $(depend_usym) -m64 -I $(usym) -s -O3 -o "../_bin/COTx64" -D_dbg -D_Linux -lm -Wno-unused-result
	cd ../_bin/ && cp COTx64 ~/cot
l:
	gcc $(builds) $(depend_usym) -I $(usym) -s -O3 -o "../_bin/cot" -D_dbg -D_Linux -lm -w
clean:
	del *.o