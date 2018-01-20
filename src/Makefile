CFLAGS=-c -std=gnu99

libgpiovuart: gpiovuart.o gpiovuart_backend.o
	ar rc libgpiovuart.a $?
	#$(CC)  -shared -o libgpiovuart.so $?

gpiovuart.o: gpiovuart.c 
	$(CC) $(CFLAGS) $? -o gpiovuart.o

gpiovuart_backend.o: gpiovuart_posix.c
	$(CC) $(CFLAGS) $? -o gpiovuart_backend.o

clean:
	-rm *.a *.so *.o
