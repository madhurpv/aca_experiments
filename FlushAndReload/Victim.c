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

    int readingInt = 0;

    while(1){
        printf("Enter 0 to exit, else enter any integer between 1 to 9 to write : ");
        scanf("%d", &readingInt);
        if(readingInt<=0){
            break;
        }
        else if(readingInt>9){
            printf("Integer too big. Make sure integer is less than 10\n");
        }
        else{
            szMsg[0] = '0' + readingInt;
            CopyMemory((void *)pBuf, szMsg, ((strlen(szMsg)+1) * sizeof(char)));
        }
    }

   UnmapViewOfFile(pBuf);
   CloseHandle(hMapFile);
   return 0;
}