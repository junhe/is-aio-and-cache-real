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
#include <sys/time.h>
#include <fcntl.h>

#define REPTIMES 1024
#define READSIZE 81290

int main(int argc, char ** argv)
{
    ssize_t bytes_read = 0, bytes_read_pread = 0;
    int fd1 = 0, fd2 = 0;
    char data[READSIZE];
    char data_pread[READSIZE];
    struct aiocb aio;
    int i;
    struct timespec time1, time2;
    off_t offset;
    off_t len1, len2;

    struct timeval start, end, result, totaltime, tmp;

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
    memset(&data_pread, 0, sizeof(data_pread));

    aio.aio_fildes = fd1;
    aio.aio_buf = data;
    aio.aio_nbytes = READSIZE;

    len1 = lseek(fd1, 0, SEEK_END);
    assert(len1 >= 0);
  
    timerclear(&totaltime);

    srand(5);
    //clock_gettime(TIMING_METHOD, &time1);
    gettimeofday(&start, NULL);
    for ( i = 0 ; i < REPTIMES ; i++ ) {
        offset = len1 * (rand() / (double)RAND_MAX);
        //printf("%ld ", aio.aio_offset);
        posix_fadvise( fd1, offset, READSIZE, POSIX_FADV_WILLNEED);
    }
    gettimeofday(&end, NULL);
    timersub( &end, &start, &result );
    printf("posix_fadvise time (REPTIMES:%d): %ld.%.6ld\n", 
            REPTIMES, result.tv_sec, result.tv_usec);

    sleep(20);


    srand(5);
    //clock_gettime(TIMING_METHOD, &time1);
    gettimeofday(&start, NULL);
    for ( i = 0 ; i < REPTIMES ; i++ ) {
        offset = len1 * (rand() / (double)RAND_MAX);

        bytes_read_pread += pread( fd1, data_pread, READSIZE, offset ); 
    }
    gettimeofday(&end, NULL);
    timersub( &end, &start, &result );
    printf("pread() with caching time (REPTIMES:%d): %ld.%.6ld\n", 
            REPTIMES, result.tv_sec, result.tv_usec);

    printf("%d bytes read by pread()\n", bytes_read_pread);

    close(fd1);

    /* ************************************************ */
    len2 = lseek(fd2, 0, SEEK_END);
    assert(len2 >= 0);
    
    timerclear(&totaltime);

    srand(5);
    bytes_read = 0;
    gettimeofday(&start, NULL);
    for ( i = 0 ; i < REPTIMES ; i++ ) {
        offset = len2 * (rand() / (double)RAND_MAX);

        bytes_read += pread(fd2, data, READSIZE, offset);
    }
    gettimeofday(&end, NULL);
    timersub( &end, &start, &result );
    printf("pread() without caching time (REPTIMES:%d): %ld.%.6ld\n", 
            REPTIMES, result.tv_sec, result.tv_usec);
    printf("%d bytes read by pread().\n", bytes_read);
    close(fd2);

    return EXIT_SUCCESS;
}


