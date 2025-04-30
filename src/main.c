#include "heap.h"
#include <stdio.h>

int main() {
  void *a = mem_alloc(128);
  void *b = mem_alloc(256);
  void *c = mem_alloc(64);
  void *d = mem_alloc(128);
  void *e = mem_alloc(256);

  printf("\n--- Allocated Chunks ---\n");
  display_chunk_list(&alloced_chunks);
  printf("\n--- Free Chunks ---\n");
  display_chunk_list(&free_chunks);

  mem_free(b);
  mem_free(d);

  printf("\n--- After Freeing b and d (Fragmented Free Space) ---\n");
  printf("Allocated Chunks :\n");
  display_chunk_list(&alloced_chunks);
  printf("Free Chunks :\n");
  display_chunk_list(&free_chunks);

  size_t total_free = 0;
  for (size_t i = 0; i < free_chunks.size; i++) {
    total_free += free_chunks.list[i].size;
  }
  printf("\nTotal Free Space: %zu bytes\n", total_free);

  printf("Trying to allocate 3600 bytes...\n");
  void *f = mem_alloc(3600);
  if (!f) {
    printf("Allocation of 3600 bytes failed due to external fragmentation.\n");
  } else {
    printf("Allocation of 3600 bytes succeeded unexpectedly at %p.\n", f);
  }

  return 0;
}
