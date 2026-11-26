// FROM https://learn.microsoft.com/en-us/windows/win32/memory/creating-named-shared-memory

#include <windows.h>
#include <stdio.h>

#define BUF_SIZE 256
char szName[] = "MyFileMappingObject";

int main()
{
   HANDLE hMapFile;
   char* pBuf;

   hMapFile = OpenFileMapping(
                   FILE_MAP_ALL_ACCESS,   // read/write access
                   FALSE,                 // do not inherit the name
                   szName);               // name of mapping object

   if (hMapFile == NULL)
   {
      printf("Could not open file mapping object (%d).\n",
             GetLastError());
      return 1;
   }

   pBuf = (char *) MapViewOfFile(hMapFile, // handle to map object
               FILE_MAP_ALL_ACCESS,  // read/write permission
               0,
               0,
               BUF_SIZE);

   if (pBuf == NULL)
   {
      printf("Could not map view of file (%d).\n",
            GetLastError());

      CloseHandle(hMapFile);

      return 1;
   }

   int temp;
   while(1){
      printf("%s", pBuf);
      scanf("%d", &temp);
   }

   UnmapViewOfFile(pBuf);
   CloseHandle(hMapFile);

   return 0;
}