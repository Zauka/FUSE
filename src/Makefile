net_client: net_raid_client.c file_syscalls.o
	gcc -Wall net_raid_client.c ../bin/file_syscalls.o `pkg-config fuse --cflags --libs` -o net_client

file_syscalls.o: file_syscalls.c
	gcc -Wall -c file_syscalls.c `pkg-config fuse --cflags --libs` -o ../bin/file_syscalls.o

.PHONY: clean
clean:
	rm -f *.o net_client
