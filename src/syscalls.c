#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "serial_uart.h"

extern uint8_t _end;

void *_sbrk(ptrdiff_t incr)
{
    static uint8_t *heap_end = NULL;
    uint8_t *prev;

    if (heap_end == NULL)
    {
        heap_end = &_end;
    }
    prev = heap_end;
    heap_end += incr;
    return (void *)prev;
}

int _close(int file)
{
    (void)file;
    errno = ENOSYS;
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    (void)file;
    (void)ptr;
    (void)dir;
    errno = ENOSYS;
    return -1;
}

int _read(int file, char *ptr, int len)
{
    (void)file;
    (void)ptr;
    (void)len;
    errno = ENOSYS;
    return -1;
}

int _write(int file, char *ptr, int len)
{
    if (file != 1 && file != 2)
    {
        errno = ENOSYS;
        return -1;
    }

    serial_uart_write(ptr, (size_t)len);
    return len;
}

int _fstat(int file, struct stat *st)
{
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file)
{
    (void)file;
    return 1;
}