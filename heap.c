#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define HEAP_CAPACITY 640000
#define ALLOC_CAPACITY 1080
void *heap = NULL;

typedef struct {
  void *start;
  size_t size;
} Chunk;

typedef struct {
  Chunk list[ALLOC_CAPACITY];
  size_t size;
} Chunk_List;

Chunk_List alloced_chunks = {.list = {NULL}, .size = 0};
Chunk_List free_chunks = {};

void *heap_init() {
  void *new_h = mmap(NULL, HEAP_CAPACITY, PROT_READ | PROT_WRITE,
                     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (new_h == (void *)-1) {
    fprintf(stderr, "HEAP INITIALIZATION FAILED");
    exit(EXIT_FAILURE);
  }
  free_chunks.size = 1;
  free_chunks.list[0].start = new_h;
  free_chunks.list[0].size = HEAP_CAPACITY;
  return new_h;
}

void insert_chunk(Chunk_List *list, Chunk *chunk) {
  if (list->size >= ALLOC_CAPACITY)
    return;
  list->list[list->size++] = *chunk;
  Chunk *chunk1;
  Chunk *chunk2;
  Chunk temp;
  for (int i = list->size - 1; i > 0; i--) {
    chunk1 = &list->list[i];
    chunk2 = &list->list[i - 1];
    if (chunk1->start < chunk2->start) {
      temp = *chunk1;
      *chunk1 = *chunk2;
      *chunk2 = temp;
    }
  }
}

void remove_chunk(Chunk_List *list, size_t index) {
  for (size_t i = index; i < list->size - 1; i++) {
    list->list[i] = list->list[i + 1];
  }
  list->size--;
}

int find_ptr(Chunk_List *list, void *ptr) {
  size_t left = 0;
  size_t right = list->size - 1;
  size_t mid;
  while (left <= right) {
    mid = (left + right) / 2;
    void *p = list->list[mid].start;
    if (p == ptr)
      return mid;
    if (p > ptr) {
      right = mid - 1;
    }
    if (p < ptr) {
      left = mid + 1;
    }
  }

  return -1;
}
void display_chunk_list(Chunk_List *list) {
  printf("In list : %zu \n", list->size);
  for (int i = 0; i < list->size; i++) {
    printf("size : %zu , address : %p\n", list->list[i].size,
           list->list[i].start);
  }
}

void merge_free_chunks() {
  Chunk_List temp = {.size = 0};
  temp.list[temp.size++] = free_chunks.list[0];
  for (size_t i = 1; i < free_chunks.size; i++) {
    Chunk *temp_top_chunk = &temp.list[temp.size - 1];
    Chunk *curr_free_chunk = &free_chunks.list[i];
    if (temp_top_chunk->start + temp_top_chunk->size ==
        curr_free_chunk->start) {
      temp_top_chunk->size += curr_free_chunk->size;
    } else {
      temp.list[temp.size++] = *curr_free_chunk;
    }
  }
  // copy temp into free_chunks
  for (size_t j = 0; j < temp.size; j++) {
    free_chunks.list[j] = temp.list[j];
  }
  free_chunks.size = temp.size;
}

void *mem_alloc(size_t size) {
  if (heap == NULL) {
    heap = heap_init();
  }
  if (size == 0)
    return NULL;

  Chunk free_chunk;
  Chunk new_alloc_chunk;
  for (size_t i = 0; i < free_chunks.size; i++) {
    free_chunk = free_chunks.list[i];
    if (free_chunk.size < size) {
      continue;
    }
    new_alloc_chunk.start = free_chunk.start;
    new_alloc_chunk.size = size;
    insert_chunk(&alloced_chunks, &new_alloc_chunk);
    int diff = free_chunk.size - size;
    if (diff > 0) {
      Chunk new_free_chunk = {.start = free_chunk.start + size,
                              .size = (size_t)diff};
      insert_chunk(&free_chunks, &new_free_chunk);
    }
    remove_chunk(&free_chunks, i);
    return new_alloc_chunk.start;
  }
  return NULL;
};

void mem_free(void *ptr) {
  if (ptr == NULL)
    return;
  if (heap == NULL) {
    fprintf(stderr, "Pointer not in heap");
    exit(EXIT_FAILURE);
  }
  int index = find_ptr(&alloced_chunks, ptr);
  if (index < 0) {
    fprintf(stderr, "heap_free() : Invalid pointer");
    exit(EXIT_FAILURE);
  }
  Chunk chunk = alloced_chunks.list[index];
  insert_chunk(&free_chunks, &chunk);
  remove_chunk(&alloced_chunks, index);
  merge_free_chunks();
}

int main(int argc, char *argv[]) {
  for (int i = 1; i <= 10; i++) {
    void *p = mem_alloc(i);
    if (i % 2 == 0) {
      mem_free(p);
    }
  }
  display_chunk_list(&alloced_chunks);
  display_chunk_list(&free_chunks);
  return 0;
}
