CC=gcc

ALIGN = -Wl,--file-alignment,0x0200 \
		-Wl,--section-alignment,0x0200 

CFLAGS=-Os -s
LFLAGS=-Os -s -fmerge-all-constants  -mwindows #$(ALIGN)

OBJS = socket_ctrl.o\
	   socket_func.o\
	   session.o\
	   config.o\
	   server_main.o\
	   iodirect.o\
	   newconnect.o\
	   control.o\
	   file.o\
	   reconnect.o\
	   md5.o\
	   rc4.o

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

file.o:command/file.c
	$(CC) $(CFLAGS) -c $^ -o $@

##

#crypto module
rc4.o:crypto/rc4.c
	$(CC) $(CFLAGS) -c $^ -o $@

md5.o:crypto/md5.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm *.o *.exe *.dll
