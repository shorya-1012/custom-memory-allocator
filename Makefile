$(CC) = clang

CFLAGS =-Wall -Wextra -g 

heap: heap.c
	$(CC) $(CFLAGS) heap.c -o heap

clean:
	rm *.o heap
