CC=gcc
CFLAGS=-Os -s
LFLAGS=-Os -s -fmerge-all-constants  -mwindows

OBJS = socket_ctrl.o\
	   socket_func.o\
	   session.o\
	   config.o\
	   server_main.o\
	   iodirect.o\
	   newconnect.o\
	   control.o\
	   reconnect.o\
	   aes256.o

DLL_FILE = server.dll
EXE_FILE = server.exe

all:$(EXE_FILE)

dll:$(DLL_FILE)

$(EXE_FILE):$(OBJS)
	$(CC) $(LFLAGS) $^ -o $@

$(DLL_FILE):$(OBJS)
	$(CC) $(LFLAGS) $^ -shared -o $@


.c.o:
	$(CC) $(CFLAGS) -c $^ -o $@

#command module

iodirect.o:command/iodirect_windows.c
	$(CC) $(CFLAGS) -c $^ -o $@

newconnect.o:command/newconnect.c
	$(CC) $(CFLAGS) -c $^ -o $@

reconnect.o:command/reconnect.c
	$(CC) $(CFLAGS) -c $^ -o $@

control.o:command/control.c
	$(CC) $(CFLAGS) -c $^ -o $@

##

#crypto module
aes256.o:crypto/aes256.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm *.o *.exe *.dll
