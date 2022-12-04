#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/mman.h>

typedef long i64;
typedef int i32;
typedef short i16;
typedef char i8;

typedef unsigned long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef signed long s64;
typedef signed int s32;
typedef signed short s16;
typedef signed char s8;

void *memcpy(void *dest, const void *src, u64 size);
u64 strlen(const i8 *cstr);

struct str {
    i8 *data;
    s64 len;
};

struct file {
    struct str name;
    i8 *data;
    s64 len;
    s64 pos;
};

#define cstr(str) { str, sizeof(str) - 1 }

struct file OpenFile(struct str name)
{
    i32 fd; u64 size; i8 *data;
    struct stat st;
    struct file result = {0};

    fd = open(name.data, O_RDONLY);
    if (0 > fd) {
        result.len = -errno;
        return result;
    }

    if (0 != fstat(fd, &st)) {
        result.len = -errno;
        return result;
    }

    size = (s64) st.st_size;
    data = mmap(0x0, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (0 == data) {
        result.len = -errno;
        return result;
    }
    result.name.data = malloc(name.len+1);
    if (0 == result.name.data) {
        result.len = -errno;
        return result;
    }

    memcpy(result.name.data, name.data, name.len);
    result.name.len = name.len;
    result.name.data[name.len] = '\0';

    result.len = size;
    result.data = data;
    return result;
}

i32 GetNextLine(struct file *f, i8 *dst) {
    i32 len = 0;

    while (f->data[f->pos+len] != '\n') {
        dst[len] = f->data[f->pos+len];
        ++len;
    }
    dst[len] = '\0';
    f->pos += len+1;
    return len;
}

i32 main()
{
    i32 increases = 0;
    i8 line[64];
    struct str filename = cstr("input");
    struct file input = OpenFile(filename); 
    s64 lastValue = 0;
    if (!input.data) {
        return -input.len;
    }
    GetNextLine(&input, line);
    lastValue = atol(line);

    while (input.pos < input.len) {
        s64 value = 0;
        GetNextLine(&input, line);
        value = atol(line);
        if (value > lastValue) {
            increases += 1;
        }
        lastValue = value;
    }

    printf("total number of increases is %d\n", increases);

    return 0;
}
