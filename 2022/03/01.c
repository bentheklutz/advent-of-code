#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

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

i32 printf(const char *fmt, ...);

#define cstr(str) { str, sizeof(str) - 1 }

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
    struct file input = OpenFile(name);
    i32 totalPriorityDuplicated = 0;
    
    if (!input.data) {
        printf("failed to open input file: %ld\n", -input.len);
        return -input.len;
    }

    while (input.pos < input.len) {
        i32 i;
        i32 lineLength = 0; i8 line[1024];
        i32 parts[2][52] = {0};
        
        lineLength = GetNextLine(&input, line);

        for (i = 0; i < lineLength / 2; ++i) {
            i8 curr = line[i];
            i32 itemIdx;
            if ('a' <= curr) {
                itemIdx = curr - 'a';
            } else if ('A' <= curr) {
                itemIdx = (curr - 'A') + 26;
            }
            parts[0][itemIdx] += 1;
        }

        for (i = (lineLength / 2); i < lineLength; ++i) {
            i8 curr = line[i];
            i32 itemIdx;
            if ('a' <= curr) {
                itemIdx = curr - 'a';
            } else if ('A' <= curr) {
                itemIdx = (curr - 'A') + 26;
            }
            parts[1][itemIdx] += 1;
        }

        for (i = 0; i < 52; ++i) {
            if (parts[0][i] && parts[1][i]) {
                i32 priority = i + 1;
                totalPriorityDuplicated += priority;
            }
        }
    }
    printf("total priority of duplicated items is %d\n", totalPriorityDuplicated);
    return 0;
}
