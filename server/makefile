CC=gcc
CFLAGS=-Os -s
LFLAGS=-Os -s -mwindows -fmerge-all-constants

server.exe:socket_ctrl.o socket_func.o session.o server_main.o iodirect.o newconnect.o config.o
	$(CC) $(LFLAGS) socket_ctrl.o socket_func.o session.o server_main.o iodirect.o newconnect.o config.o -o server.exe

socket_ctrl.o:socket_ctrl.c
	$(CC) $(CFLAGS) -c socket_ctrl.c

socket_func.o:socket_func.c
	$(CC) $(CFLAGS) -c socket_func.c

session.o:session.c
	$(CC) $(CFLAGS) -c session.c

config.o:config.c
	$(CC) $(CFLAGS) -c config.c

server_main.o:server_main.c
	$(CC) $(CFLAGS) -c server_main.c

iodirect.o:command/iodirect_windows.c
	$(CC) $(CFLAGS) -c command/iodirect_windows.c -o iodirect.o

newconnect.o:command/newconnect.c
	$(CC) $(CFLAGS) -c command/newconnect.c -o newconnect.o

clean:
	rm *.o
	rm *.exe
