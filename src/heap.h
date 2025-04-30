#pragma once
#define HEAP_H

#include <stddef.h>
#include <stdint.h>

// Constants
#define PAGE_SIZE 4096
#define ALLOC_CAPACITY 1000
#define DEFAULT_ALIGNMENT 8

// Types
typedef struct {
  void *start;
  size_t size;
} Chunk;

typedef struct {
  Chunk list[ALLOC_CAPACITY];
  size_t size;
} Chunk_List;

// Extern global variables
extern void *heap;
extern Chunk_List alloced_chunks;
extern Chunk_List free_chunks;

// Functions
void *heap_init(void);
void insert_chunk(Chunk_List *list, Chunk *chunk);
void remove_chunk(Chunk_List *list, size_t index);
int find_ptr(Chunk_List *list, void *ptr);
void display_chunk_list(Chunk_List *list);
void merge_free_chunks(void);
uintptr_t align_forward(uintptr_t ptr, uintptr_t align);
void *mem_alloc(size_t size);
void mem_free(void *ptr);
