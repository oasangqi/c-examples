all:clib cpp_c cpp_clib c_clib
clib:
	gcc -o libfunc.so func.c -shared -fPIC 
	cp libfunc.so /usr/lib64
cpp_c:
	g++ -o cpp_c_bin func.c main.cpp 
cpp_clib:
	g++ -o cpp_clib_bin main.cpp -lfunc
c_clib:
	gcc -o c_clib_bin main.c -lfunc
clean:
	rm -f *.o *bin libfunc.so /usr/lib64/libfunc.so
	
