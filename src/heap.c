#include "heap.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

void *heap = NULL;

Chunk_List alloced_chunks = {.size = 0};
Chunk_List free_chunks = {};

void *heap_init() {
  void *new_h = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
                     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (new_h == (void *)-1) {
    fprintf(stderr, "HEAP INITIALIZATION FAILED");
    exit(EXIT_FAILURE);
  }
  free_chunks.size = 1;
  free_chunks.list[0].start = new_h;
  free_chunks.list[0].size = PAGE_SIZE;
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
  for (size_t i = 0; i < list->size; i++) {
    printf("size : %zu , address : %p\n", list->list[i].size,
           list->list[i].start);
  }
}

void merge_free_chunks() {
  size_t write_index = 1;
  for (size_t i = 1; i < free_chunks.size; i++) {
    Chunk *prev_merged_chunk = &free_chunks.list[write_index - 1];
    Chunk *current_chunk = &free_chunks.list[i];
    if (prev_merged_chunk->start + prev_merged_chunk->size ==
        current_chunk->start) {
      prev_merged_chunk->size += current_chunk->size;
    } else {
      free_chunks.list[write_index++] = *current_chunk;
    }
  }
  free_chunks.size = write_index;
}

uintptr_t align_forward(uintptr_t ptr, uintptr_t align) {
  uintptr_t p = (uintptr_t)ptr;
  uintptr_t rem = p % align;
  if (rem != 0) {
    p = p + align - rem;
  }
  return p;
}

void *mem_alloc(size_t size) {
  if (heap == NULL) {
    heap = heap_init();
  }
  if (size <= 0)
    return NULL;

  Chunk free_chunk;
  Chunk new_alloc_chunk;
  for (size_t i = 0; i < free_chunks.size; i++) {
    free_chunk = free_chunks.list[i];
    if (free_chunk.size < size) {
      continue;
    }
    uintptr_t unaligned_ptr = (uintptr_t)free_chunk.start + size;
    uintptr_t aligned_ptr = align_forward(unaligned_ptr, DEFAULT_ALIGNMENT);
    size_t aligned_size = aligned_ptr - (uintptr_t)free_chunk.start;
    new_alloc_chunk.start = free_chunk.start;
    new_alloc_chunk.size = aligned_size;
    insert_chunk(&alloced_chunks, &new_alloc_chunk);
    int diff = free_chunk.size - aligned_size;
    if (diff > 0) {
      Chunk new_free_chunk = {.start = free_chunk.start + aligned_size,
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
