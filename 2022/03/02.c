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

void *malloc(u64 size);
void *memcpy(void *dest, const void *src, u64 size);

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
    i32 totalPrio = 0;
    if (!input.data) {
        printf("failed to open input file: %ld\n", -input.len);
        return -input.len;
    }


    while (input.pos < input.len) {
        i32 i; i32 j;
        i32 lineLengths[3] = {0}; i8 lines[3][1024];
        i8 present[3][52];

        for (i = 0; i < 3; ++i) {
            for (j = 0; j < 52; ++j) {
                present[i][j] = 0;
            }
        }

        for (i = 0; i < 3; ++i) {
            lineLengths[i] = GetNextLine(&input, lines[i]);
        }

        for (i = 0; i < 3; ++i) {
            for (j = 0; j < lineLengths[i]; ++j) {
                i32 itemIdx;
                i8 curr = lines[i][j];
                if ('a' <= curr) {
                    itemIdx = curr - 'a';
                } else if ('A' <= curr) {
                    itemIdx = curr - 'A' + 26;
                }
                present[i][itemIdx] += 1;
            }
        }

        for (j = 0; j < 52; ++j) {
            i32 prio = 0;
            if (present[0][j] && present[1][j] && present[2][j]) {
                prio = j + 1;
                totalPrio += prio;
            }
        }
    }

    printf("total priority of badge items is %d\n", totalPrio);

    return 0;
}
