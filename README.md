# Custom Memory Allocator

This repository contains my implementation of the `malloc()` function in C. Please note that this is primarly made for fun and eductional purposes to understand how the heap works. It is by no means a one-to-one implementation.
## Features

- **Dynamic Memory Allocation:** The allocator dynamically manages memory using the `mmap` system call.
- **Allocation and Deallocation:** Users can allocate and deallocate memory using the `mem_alloc` and `mem_free` functions.
- **Chunk Management:** The allocator maintains lists of allocated and free memory chunks.

## Quick Start

1. **Compile the Code: (uses `clang` by default)**
   ```bash
   $ make
   $ ./heap
   ```

2. **Use the `mem_alloc()` function to allocatoe memroy:**
   ```c

   void *ptr = mem_alloc(32);

   ```

4. **Use the `mem_free()` function to deallocate memory:**
   ```c

   mem_free(ptr1);

   ```
##  Known Limitations : 

1. Only works on systems that have the `mmap()` system call
2. Not thread safe
3. Max heap size of 4096 bytes
