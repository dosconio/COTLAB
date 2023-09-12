usym = ../Unisym/c/
depend_usym = $(usym)source/ustring.c $(usym)source/hstring.c
builds = cotlab.c erro.c parser.c executor.c
# Windows
c64:
	gcc $(builds) $(depend_usym) -m64 -I $(usym) -s -O3 -o "../_bin/COTLAB.x64.exe" -D_dbg
clean:
	del *.o