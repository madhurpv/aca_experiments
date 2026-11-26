# Simple inter-process communication


From https://learn.microsoft.com/en-us/windows/win32/memory/creating-named-shared-memory

`Process1.c` writes, `Process2.c` reads the same shared memory region

Both processes access a named memory-mapped object with a max size of 256 bytes(`BUF_SIZE`).

**`Process1.c`** : Uses `CreateFileMapping()` to create the shared memory object and MapViewOfFile() to map it into its address space. It copies data from `szMsg` into the shared memory buffer (`pBuf`) using `CopyMemory()`.  
 
**`Process2.c`** : Uses `OpenFileMapping()` to open the existing shared memory object and `MapViewOfFile()` to map the same memory region into its address space. It reads the data from `pBuf`.

**Note** - While running, run `Process1.c` first and then run `Process2.c`, because `Process1.c` needs to execute `CreateFileMapping()` earlier.
