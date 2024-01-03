$(CC) = clang

heap : heap.c
	$(CC) heap.c -o heap -g

clean : 
	rm *.o heap
