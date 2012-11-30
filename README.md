is-aio-and-cache-real
=====================

This is to test if POSIX AIO really works and if the file system 
cache really reduces delay.


Folders:
./aio .................. Test Async I/O. clock_gettime() is used 
                         for timing.
./aio-gettimeofday ..... Test Async I/O. gettimeofday() is used
                         for timing.
./cache ................ Test how much time can be reduced by using
                         cache.

