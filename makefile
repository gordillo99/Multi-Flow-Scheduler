.phony all:
all: MFS

MFS: MFS.c
	gcc MFS.c linked_list.c -o mfs -lreadline -lhistory -ggdb -pthread

.PHONY clean:
clean:
	-rm -rf *.o *.exe
