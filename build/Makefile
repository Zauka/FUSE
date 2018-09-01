all: net_client net_server

net_client: ../src/client/net_raid_client.c client_syscalls.o
	gcc -Wall ../src/client/net_raid_client.c client_syscalls.o `pkg-config fuse --cflags --libs` -o ../bin/net_client

client_syscalls.o: ../src/client/client_syscalls.c
	gcc -Wall -c ../src/client/client_syscalls.c `pkg-config fuse --cflags --libs` -o client_syscalls.o

net_server: ../src/server/net_raid_server.c server_syscalls.o
	gcc -Wall ../src/server/net_raid_server.c server_syscalls.o -o ../bin/net_server

server_syscalls.o: ../src/server/server_syscalls.c
	gcc -Wall -c ../src/server/server_syscalls.c -o server_syscalls.o

.PHONY: clean umount rebuild
clean:
	rm -f *.o ../bin/net_client ../bin/net_server
umount:
	fusermount -u ../bin/mountdir
rebuild:
	make umount; make clean; make; echo "\nBuilt Successfuly"
