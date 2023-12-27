import ctypes as C
dll = C.cdll.LoadLibrary('E:\PROJ\SGArithmetic\ShellDemo\sgax.dll')
calc = dll.NumberArithmetic
calc.argtypes = [C.c_char_p]
calc.restype = C.c_char_p
in_str = C.create_string_buffer(1024,'\0')
out_str = calc(in_str)
print("1 + 2 = ")
print(out_str)
