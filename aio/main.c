/* 
 * To Compile:
 * $ gcc main.c -lrt
 *
 * To Run:
 * $ ./a.out path-of-file-01 path-of-file-02
 *
 * You want the file to be of equal size so it is 
 * fair for bot cases.
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
#include <aio.h> /* aio_read, aio_error, aio_return */
#include <errno.h> /* EINPROGRESS */
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define REPTIMES 10000
#define READSIZE 1
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
    ssize_t bytes_read = 0;
    int fd1 = 0, fd2 = 0;
    char data[READSIZE];
    struct aiocb aio;
    int i;
    struct timespec time1, time2;
    off_t offset;
    off_t len1, len2;

    if ( argc != 3 ) {
        printf( "Usage: %s file-1-path file-2-path", argv[0] );
        exit(1);
    }

    
    fd1 = open(argv[1], O_RDONLY);

    if (fd1 == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    fd2 = open(argv[2], O_RDONLY);

    if (fd2 == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    /* ************************************************ */
    memset(&aio, 0, sizeof(aio));
    memset(&data, 0, sizeof(data));

    aio.aio_fildes = fd1;
    aio.aio_buf = data;
    aio.aio_nbytes = READSIZE;

    len1 = lseek(fd1, 0, SEEK_END);
    assert(len1 >= 0);
   
    srand(5);
    clock_gettime(TIMING_METHOD, &time1);
    for ( i = 0 ; i < REPTIMES ; i++ ) {
        aio.aio_offset = len1 * (rand() / (double)RAND_MAX);
        //printf("%ld ", aio.aio_offset);
        aio_read(&aio);
    }
    clock_gettime(TIMING_METHOD, &time2);
    printf("AIO time consumed: %f\n", 
            diff(time1,time2).tv_sec + diff(time1,time2).tv_nsec/1000000000.0);
   

    //while (aio_error(&aio) == EINPROGRESS) {}

    //bytes_read = aio_return(&aio);

    //printf("%d bytes in the last aio read.\n", bytes_read);
    //printf("They are: %s\n", data);

    close(fd1);



    /* ************************************************ */
    len2 = lseek(fd2, 0, SEEK_END);
    assert(len2 >= 0);
    
    srand(5);
    bytes_read = 0;
    clock_gettime(TIMING_METHOD, &time1);
    for ( i = 0 ; i < REPTIMES ; i++ ) {
        offset = len2 * (rand() / (double)RAND_MAX);
        //printf("%ld ", offset);
        bytes_read += pread(fd2, data, READSIZE, offset);
    }
    clock_gettime(TIMING_METHOD, &time2);
    printf("%d bytes read in total in sync io.\n", bytes_read);
    printf("Sync IO time consumed: %f\n", 
            diff(time1,time2).tv_sec + diff(time1,time2).tv_nsec/1000000000.0);

    close(fd2);




    return EXIT_SUCCESS;
}


