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

#define REPTIMES 10000
#define READSIZE 1

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

    
    fd1 = open(argv[1], O_RDONLY, O_DIRECT);

    if (fd1 == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    fd2 = open(argv[2], O_RDONLY, O_DIRECT);

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
    for ( i = 0 ; i < REPTIMES ; i++ ) {
        aio.aio_offset = len1 * (rand() / (double)RAND_MAX);
        //printf("%ld ", aio.aio_offset);
        aio_read(&aio);

        while (aio_error(&aio) == EINPROGRESS) {}

        bytes_read += aio_return(&aio);

        //printf("%d bytes in the last aio read.\n", bytes_read);
        //printf("They are: %s\n", data);

        // Test if pread still cost lots of time
        gettimeofday(&start, NULL);

        bytes_read_pread += pread( fd1, data_pread, READSIZE, aio.aio_offset ); 
        
        gettimeofday(&end, NULL);
        timersub( &end, &start, &result );
        timeradd( &totaltime, &result, &tmp );
        totaltime = tmp;
    }


    printf("========== pread() with caching ============\n");
    printf("pread() Time after caching by aio: %ld.%ld\n", 
            totaltime.tv_sec, totaltime.tv_usec);
    printf("%d bytes read by aio_read()\n", bytes_read);
    printf("%d bytes read by pread()\n", bytes_read_pread);

    close(fd1);

    /* ************************************************ */
    len2 = lseek(fd2, 0, SEEK_END);
    assert(len2 >= 0);
    
    timerclear(&totaltime);

    srand(5);
    bytes_read = 0;
    for ( i = 0 ; i < REPTIMES ; i++ ) {
        offset = len2 * (rand() / (double)RAND_MAX);
        //printf("%ld ", offset);
        gettimeofday(&start, NULL);

        bytes_read += pread(fd2, data, READSIZE, offset);

        gettimeofday(&end, NULL);
        timersub( &end, &start, &result );
        timeradd( &totaltime, &result, &tmp );
        totaltime = tmp;
    }

    printf("========== pread() without caching ============\n");
    printf("pread() Time without caching: %ld.%ld\n", totaltime.tv_sec, totaltime.tv_usec);
    printf("%d bytes read by pread().\n", bytes_read);
    close(fd2);

    return EXIT_SUCCESS;
}


