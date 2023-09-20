usym = ../Unisym/c/
depend_usym = $(usym)source/cdear.c $(usym)source/ustring.c $(usym)source/hstring.c
builds = cotlab.c erro.c parser.c executor.c dtrlib.c
# Windows
Win64:
	gcc $(builds) $(depend_usym) -m64 -I $(usym) -s -O3 -o "../_bin/COTLAB.x64.exe" -D_dbg -D_WinNT -Wunused-function
	cd ../_bin/ && copy COTLAB.x64.exe cot.exe
Lin64:
	gcc $(builds) $(depend_usym) -m64 -I $(usym) -s -O3 -o "../_bin/COTLAB.x64.exe" -D_dbg -D_Linux -Wunused-function
	cd ../_bin/ && cp COTLABx64 cot
clean:
	del *.o