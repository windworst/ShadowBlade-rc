CC=gcc
LFLAGS=-Os -s

server.exe:socket_ctrl.o socket_func.o session.o server_main.o
	$(CC) $(LFLAGS) socket_ctrl.o socket_func.o session.o server_main.o -o server.exe

socket_ctrl.o:socket_ctrl.c
	$(CC) $(LFLAGS) -c socket_ctrl.c

socket_func.o:socket_func.c
	$(CC) $(LFLAGS) -c socket_func.c

session.o:session.c
	$(CC) $(LFLAGS) -c session.c

server_main.o:server_main.c
	$(CC) $(LFLAGS) -c server_main.c

clean:
	del *.o
	del ./command/*.o
	del *.exe
