/* 
 * 
 */

#define _GNU_SOURCE

#include <fcntl.h> /* open, O_RDONLY */
#include <unistd.h> /* close */
#include <stdio.h> /* fprintf, perror */
#include <stdlib.h> /* EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h> /* memset */
#include <aio.h> /* aio_read, aio_error, aio_return */
#include <errno.h> /* EINPROGRESS */
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define REPTIMES 1000000
#define READSIZE 1


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
        offset = (float)len1 * rand() / RAND_MAX;
        //printf("%ld ", offset);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
        bytes_read1 += pread(fd1, data, READSIZE, offset);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
        firstcalltime += diff(time1,time2).tv_sec 
            + diff(time1,time2).tv_nsec/1000000000.0;

        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
        bytes_read2 += pread(fd1, data, READSIZE, offset);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
        secondcalltime += diff(time1,time2).tv_sec 
            + diff(time1,time2).tv_nsec/1000000000.0;
    }
   
    printf("First  %9d calls time consumed (sec): %f\n", REPTIMES, firstcalltime);
    printf("Second %9d calls time consumed (sec): %f\n", REPTIMES, secondcalltime);
    printf("Time difference first-second (sec):         %f\n", firstcalltime - secondcalltime);

    printf("First  calls read (bytes): %d\n", bytes_read1);
    printf("Second calls read (bytes): %d\n", bytes_read2);

    close(fd1);

    return EXIT_SUCCESS;
}

