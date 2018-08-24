all: net_client net_server

net_client: net_raid_client.c file_syscalls.o
	gcc -Wall net_raid_client.c ../bin/file_syscalls.o `pkg-config fuse --cflags --libs` -o net_client

file_syscalls.o: file_syscalls.c
	gcc -Wall -c file_syscalls.c `pkg-config fuse --cflags --libs` -o ../bin/file_syscalls.o

net_server: net_raid_server.c
	gcc -Wall net_raid_server.c -o net_server

.PHONY: clean umount rebuild
clean:
	rm -f ../bin/*.o net_client net_server
umount:
	fusermount -u ../mountdir
rebuild:
	make umount; make clean; make;S
