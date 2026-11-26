#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <intrin.h>
#include <immintrin.h>

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif


#define BUF_SIZE 2
char szName[] = "MyFileMappingObject";




void sleep_us(LONGLONG microseconds) {
    // Requires Windows 10 v1803+
    HANDLE timer = CreateWaitableTimerExW(
        NULL, 
        NULL, 
        CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, 
        TIMER_ALL_ACCESS
    );
    if (!timer) return;

    // Negative values indicate relative time in 100-nanosecond intervals
    LARGE_INTEGER dueTime;
    dueTime.QuadPart = -(microseconds * 10);

    SetWaitableTimer(timer, &dueTime, 0, NULL, NULL, FALSE);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}



int main()
{

   SetThreadAffinityMask(GetCurrentThread(), 1);

   HANDLE hMapFile;
   char* pBuf;

   hMapFile = OpenFileMapping(
                   FILE_MAP_ALL_ACCESS,   // read/write access
                   FALSE,                 // do not inherit the name
                   szName);               // name of mapping object

   if (hMapFile == NULL){
      printf("Could not open file mapping object (%d).\n", GetLastError());
      return 1;
   }

   pBuf = (char *) MapViewOfFile(hMapFile, // handle to map object
               FILE_MAP_ALL_ACCESS,  // read/write permission
               0,
               0,
               BUF_SIZE);

   if (pBuf == NULL){
      printf("Could not map view of file (%d).\n", GetLastError());
      CloseHandle(hMapFile);
      return 1;
   }

   unsigned char checksum = 0;

    volatile const char* target = (volatile const char*) pBuf;

    #define SAMPLES 5000
    uint64_t timings[SAMPLES];
    for (int i = 0; i < SAMPLES; i++){
        //printf("%s\n", pBuf);
        //scanf("%d", &temp);
        
        uint64_t freq, start, end;
        unsigned int aux;

        _mm_lfence();
        start = __rdtsc();
        _mm_lfence();
        char a = *target;
        end = __rdtscp(&aux);
        _mm_lfence();

        //printf("%llu\n", (unsigned long long)(end - start));
        timings[i] = end - start;
        _mm_clflush(pBuf);
        _mm_mfence();

        checksum ^= a;
        //Sleep(1);
        sleep_us(400);
   }

   // Print after all measurements
    //FILE *fptr = fopen("alternatetimings.txt", "w");
    for (int i = 0; i < SAMPLES; i++)
    {
        //fprintf(fptr, "%llu\n", (unsigned long long)timings[i]);
        printf("%llu\n", (unsigned long long)timings[i]);
    }
    //fclose(fptr);

   UnmapViewOfFile(pBuf);
   CloseHandle(hMapFile);

   return 0;
}