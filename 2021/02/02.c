#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>

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
void *malloc(u64 size);
u64 strlen(const i8 *cstr);
i32 atoi(i8 *cstr);

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

i32 GetNextLine(struct file *f, i8 *dst)
{
    i32 len = 0;

    while (f->data[f->pos+len] != '\n') {
        dst[len] = f->data[f->pos+len];
        ++len;
    }
    dst[len] = '\0';
    f->pos += len+1;
    return len;
}

struct pos {
    i32 x;
    i32 z;
};

i32 main()
{
    struct pos p = { 0, 0 };
    i32 aim = 0;
    i8 line[16];
    struct str filename = cstr("input");
    struct file input = OpenFile(filename); 
    if (!input.data) {
        return -input.len;
    }
   
    while (input.pos < input.len) {
        i32 moveDistance = 0;
        i8 *cursor = line;
        GetNextLine(&input, line);
        while (*cursor != ' ') {
            ++cursor;
        }
        ++cursor;
        moveDistance = atoi(cursor);
        
        if (line[0] == 'u') {
            aim -= moveDistance;    
        } else if (line[0] == 'd') {
            aim += moveDistance;
        } else if (line[0] == 'f') {
            p.x += moveDistance;
            p.z += aim*moveDistance;
        }
    }

    printf("final postition is x: %d z: %d\n", p.x, p.z);
    printf("multiplied is: %d\n", p.x*p.z);

    return 0;
}
