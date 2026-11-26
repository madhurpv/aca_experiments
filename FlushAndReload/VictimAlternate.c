#include <windows.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 2
char szName[] = "MyFileMappingObject";
char szMsg[BUF_SIZE] = "1";

int main()
{
   HANDLE hMapFile;
   char* pBuf;

   hMapFile = CreateFileMapping(
                 INVALID_HANDLE_VALUE,    // use paging file
                 NULL,                    // default security
                 PAGE_READWRITE,          // read/write access
                 0,                       // maximum object size (high-order DWORD)
                 BUF_SIZE,                // maximum object size (low-order DWORD)
                 szName);                 // name of mapping object

   if (hMapFile == NULL){
      printf("Could not create file mapping object (%d).\n", GetLastError());
      return 1;
   }
   pBuf = (char*) MapViewOfFile(hMapFile,   // handle to map object
                     FILE_MAP_ALL_ACCESS,   // read/write permission
                     0,
                     0,
                     BUF_SIZE);

   if (pBuf == NULL){
        printf("Could not map view of file (%d).\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    long long readingInt = 0;

    while(1){
        szMsg[0] = '0' + readingInt%10;
        CopyMemory((void *)pBuf, szMsg, ((strlen(szMsg)+1) * sizeof(char)));
        readingInt++;
        if(readingInt%20000000==0){
            printf("- ");
            Sleep(100);
            //readingInt = 1;
            printf("%lld\n", readingInt/20000000);
        }
    }

   UnmapViewOfFile(pBuf);
   CloseHandle(hMapFile);
   return 0;
}