INCLUDE=../src
LIBS=../src
CFLAGS= -std=gnu99
all:
	$(CC) $(CFLAGS) gpiovuart_cliutil.c -o gpiovuart -lgpiovuart -L$(LIBS) -I$(INCLUDE)

clean:
	-rm gpiovuart
