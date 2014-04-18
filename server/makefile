CC=gcc
CFLAGS=-Os -s
LFLAGS=-Os -s -mwindows -fmerge-all-constants
OBJS = socket_ctrl.o socket_func.o session.o server_main.o iodirect.o newconnect.o config.o reconnect.o
DLL_FILE = server.dll
EXE_FILE = server.exe

all:$(EXE_FILE)

dll:$(DLL_FILE)

$(EXE_FILE):$(OBJS)
	$(CC) $(LFLAGS) $^ -o $@

$(DLL_FILE):$(OBJS)
	$(CC) $(LFLAGS) $^ -shared -o $@

socket_ctrl.o:socket_ctrl.c
	$(CC) $(CFLAGS) -c $^

socket_func.o:socket_func.c
	$(CC) $(CFLAGS) -c $^

session.o:session.c
	$(CC) $(CFLAGS) -c $^

config.o:config.c
	$(CC) $(CFLAGS) -c $^

server_main.o:server_main.c
	$(CC) $(CFLAGS) -c $^

iodirect.o:command/iodirect_windows.c
	$(CC) $(CFLAGS) -c $^ -o $@

newconnect.o:command/newconnect.c
	$(CC) $(CFLAGS) -c $^ -o $@

reconnect.o:command/reconnect.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm *.o *.exe *.dll
