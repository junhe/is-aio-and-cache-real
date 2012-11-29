/* 
 * Modification based on http://code.google.com/p/fscc-linux/source/browse/examples/c/aio_read.c?spec=svn854c6f36ea2b6c8eaa1306bf5699b89a99567318&r=854c6f36ea2b6c8eaa1306bf5699b89a99567318
 * 
 */

#include <fcntl.h> /* open, O_RDONLY */
#include <unistd.h> /* close */
#include <stdio.h> /* fprintf, perror */
#include <stdlib.h> /* EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h> /* memset */
#include <aio.h> /* aio_read, aio_error, aio_return */
#include <errno.h> /* EINPROGRESS */
#include <time.h>

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
    int port_fd = 0;
    char data[20];
    struct aiocb aio;

    if ( argc != 2 ) {
        printf( "Usage: %s file-path", argv[0] );
    }

    struct timespec time1, time2;
    int temp;
    long long i;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    for ( i = 0; i< 2400000000; i++)
        temp+=temp;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    printf("%ld:%ld\n", diff(time1,time2).tv_sec, diff(time1,time2).tv_nsec);
    return 0;




    port_fd = open(argv[1], O_RDONLY);

    if (port_fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    memset(&aio, 0, sizeof(aio));
    memset(&data, 0, sizeof(data));

    aio.aio_fildes = port_fd;
    aio.aio_buf = data;
    aio.aio_nbytes = sizeof(data);

    aio_read(&aio);

    while (aio_error(&aio) == EINPROGRESS) {}

    bytes_read = aio_return(&aio);

    fprintf(stdout, "%s\n", data);

    close(port_fd);

    return EXIT_SUCCESS;
}


