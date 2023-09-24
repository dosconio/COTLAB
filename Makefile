usym = ../Unisym/c/
depend_usym = $(usym)source/cdear.c $(usym)source/ustring.c $(usym)source/hstring.c
builds = cotlab.c erro.c parser.c executor.c dtrlib.c dtr_float.c
# Windows
win64:
	gcc $(builds) $(depend_usym) -m64 -I $(usym) -s -O3 -o "../_bin/COTLAB.x64.exe" -D_dbg -D_WinNT -Wunused-function
	cd ../_bin/ && copy COTLAB.x64.exe cot.exe
lin64:
	gcc $(builds) $(depend_usym) -m64 -I $(usym) -s -O3 -o "../_bin/COTLAB.x64" -D_dbg -D_Linux -lm -Wunused-function 2>../_bin/COTLAB.err.txt
	cd ../_bin/ && cp COTLABx64 cot
clean:
	del *.o