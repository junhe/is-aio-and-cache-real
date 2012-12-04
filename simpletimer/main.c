/* 
 * To Compile:
 * $ gcc main.c -lrt
 *
 * To Run:
 * $ ./a.out path-of-file
 *
 * The bigger the file is, the bigger difference you will
 * see between the first read call and second one, because 
 * of seeking.
 *
 * Macro:
 *   REPTIMES : times of read
 *   READSIZE : size of each read
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
    int fd = 0;
    char *data;
    struct timespec time1, time2;
    int i;
    off_t offset, stridesize, reqsize, nstride;
    ssize_t bytes_read;
    ssize_t bytes_read2;
    float exetime; 

    if ( argc != 5 ) {
        printf( "Usage: %s file-path stridesize reqsize NumOfStride\n", argv[0] );
        exit(1);
    }

    stridesize = atol(argv[2]);
    reqsize = atol(argv[3]);
    nstride = atol(argv[4]);

    printf("stridesize: %lld, reqsize: %lld, nstride: %lld.\n", stridesize, reqsize, nstride);
    
    fd = open(argv[1], O_RDONLY, O_DIRECT);

    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    
    bytes_read = 0;
    data = malloc( reqsize ); 
    clock_gettime(TIMING_METHOD, &time1);
    for ( i = 0 ; i < nstride ; i++ ) {
        offset = stridesize * i;
        ssize_t ret = pread(fd, data, reqsize, offset);
        bytes_read += ret;
    }
    clock_gettime(TIMING_METHOD, &time2);
    free(data); 

    exetime = diff(time1,time2).tv_sec 
              + diff(time1,time2).tv_nsec/1000000000.0;
   
    printf("Execution Time (sec): %f\n", exetime);
    printf("First  calls read (bytes): %d\n", bytes_read);

    close(fd);

    return EXIT_SUCCESS;
}


