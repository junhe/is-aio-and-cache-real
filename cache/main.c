/* 
 * 
 */

#define _GNU_SOURCE

#include <fcntl.h> /* open, O_RDONLY */
#include <unistd.h> /* close */
#include <stdio.h> /* fprintf, perror */
#include <stdlib.h> /* EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h> /* memset */
#include <errno.h> /* EINPROGRESS */
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define REPTIMES 10000
#define READSIZE 2048
#define TIMING_METHOD CLOCK_REALTIME

struct timespec diff(struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}


int main(int argc, char ** argv)
{
    int fd1 = 0;
    char data[READSIZE];
    struct timespec time1, time2;
    int i;
    off_t offset;
    off_t len1;
    ssize_t bytes_read1;
    ssize_t bytes_read2;
    float firstcalltime, secondcalltime;

    if ( argc != 2 ) {
        printf( "Usage: %s file-path\n", argv[0] );
        printf( "Use a file that you have not been used for a long tim.");
        exit(1);
    }

    
    fd1 = open(argv[1], O_RDONLY);

    if (fd1 == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    len1 = lseek(fd1, 0, SEEK_END);
    assert(len1 >= 0);
    
    srand(5);
    firstcalltime = 0;
    secondcalltime = 0;
    bytes_read1 = 0;
    bytes_read2 = 0;
    for ( i = 0 ; i < REPTIMES ; i++ ) {
        //offset = len1 * (i / (double)REPTIMES);
        offset = len1 * (rand() / (double)RAND_MAX);
        //printf("%ld * %d / %d = %ld \n", len1, i, REPTIMES, offset);
        clock_gettime(TIMING_METHOD, &time1);
        bytes_read1 += pread(fd1, data, READSIZE, offset);
        clock_gettime(TIMING_METHOD, &time2);
        firstcalltime += diff(time1,time2).tv_sec 
            + diff(time1,time2).tv_nsec/1000000000.0;

        clock_gettime(TIMING_METHOD, &time1);
        bytes_read2 += pread(fd1, data, READSIZE, offset);
        clock_gettime(TIMING_METHOD, &time2);
        secondcalltime += diff(time1,time2).tv_sec 
            + diff(time1,time2).tv_nsec/1000000000.0;
    }
   
    printf("First  %9d calls time consumed (sec): %f\n", REPTIMES, firstcalltime);
    printf("Second %9d calls time consumed (sec): %f\n", REPTIMES, secondcalltime);
    printf("Time difference first-second (sec):         %f\n", firstcalltime - secondcalltime);

    printf("First  calls read (Kb): %d\n", bytes_read1/1024);
    printf("Second calls read (Kb): %d\n", bytes_read2/1024);

    close(fd1);

    return EXIT_SUCCESS;
}


