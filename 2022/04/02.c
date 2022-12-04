#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/mman.h>

/* primitive typedefs */
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
void *malloc(u64 size);
i32 atoi(i8 *buf);

i32 printf(const char *fmt, ...);

#define cstr(str) { str, sizeof(str) - 1 }

struct str {
    i8 *data;
    s64 len;
};

struct bytebuf {
    i8 *data;
    s64 len;
    s64 cap;
};

struct file {
    struct str name;
    i8 *data;
    s64 len;
    s64 pos;
};

struct file OpenFile(struct str name)
{
    i32 fd; u64 size; i8 *data;
    struct stat st;
    struct str filename;
    struct file result = {0};
    
    fd = open(name.data, O_RDONLY);
    if (0 > fd) {
        result.len = -errno;
        return result;
    }
        
    if (0 != stat(name.data, &st)) {
        result.len = -errno;
        return result;
    }

    size = (s64) st.st_size;
    data = mmap(0x0, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (0 == data) {
        result.len = -errno;
        return result;
    }

    filename.data = malloc(name.len+1);

    if (0 == filename.data) {
        result.len = -errno;
        return result;
    }

    memcpy(filename.data, name.data, name.len);
    filename.data[name.len] = '\0';

    result.name = filename;
    result.len = size;
    result.data = data;

    return result;
}

i32 GetNextLine(struct file *f, i8 *dst)
{
    i32 i = 0;
    while(f->data[f->pos+i] != '\n') {
        dst[i] = f->data[f->pos+i];
        ++i;
    }
    
    dst[i] = '\0';
    f->pos += i+1;
    return i;
}

i32 main()
{
    struct str name = cstr("input");
    struct file input;
    i8 line[16];
    i32 overlaps = 0; i32 lines = 0;

    input = OpenFile(name);

    if (0 == input.data) {
        printf("failed to open input file: %ld\n", -input.len);
        return -input.len;
    }

    while (input.pos < input.len) {
        i8 *cursor = line;
        i8 buf[4] = {0};
        i32 starts[2] = {0};
        i32 ends[2] = {0};
        i32 i;
       
        GetNextLine(&input, line);
        
        i = 0; 
        while (*cursor != '-') {
            buf[i] = *cursor;
            ++i; ++cursor;
        }
        buf[i] = '\0';
        starts[0] = atoi(buf);
        ++cursor;
        
        i = 0;
        while (*cursor != ',') {
            buf[i] = *cursor;
            ++i; ++cursor;
        }
        ends[0] = atoi(buf);
        ++cursor;
        
        i = 0; 
        while (*cursor != '-') {
            buf[i] = *cursor;
            ++i; ++cursor;
        }
        buf[i] = '\0';
        starts[1] = atoi(buf);
        ++cursor;
        
        i = 0;
        while (*cursor != '\0') {
            buf[i] = *cursor;
            ++i; ++cursor;
        }
        ends[1] = atoi(buf);

        if (starts[0] >= starts[1] && starts[0] <= ends[1]) {
            overlaps += 1;
        } else if (starts[1] >= starts[0] && starts[1] <= ends[0]) {
            overlaps += 1;
        }

        lines += 1;
    }

    printf("input had %d lines, of which %d had overlapping ranges\n", lines, overlaps);



    return 0;
}
