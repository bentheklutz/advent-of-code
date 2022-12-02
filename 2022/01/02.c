#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>

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

struct file OpenInputFile(struct str name)
{
    i32 fd; u64 size; i8 *data;
    struct stat st;
    struct file result = {0};
    
    fd = open(name.data, O_RDONLY);
    if (0 < fd) {
        if (0 == stat(name.data, &st)) {
            size = (s64) st.st_size;

            data = mmap(0x0, size, PROT_READ, MAP_PRIVATE, fd, 0);
            if (data) {
                struct str filename;
                filename.data = malloc(name.len+1);
                memcpy(filename.data, name.data, name.len);
                filename.data[name.len] = '\0';
                
                result.name = filename;
                result.len = size;
                result.data = data;
            } else {
                result.len = -errno;
            }
        } else {
            result.len = -errno;
        }
    } else {
        result.len = -errno;
    }
    return result;
}

void GetNextLine(struct file *f, i8 *dst)
{
    i32 i = 0;
    while(f->data[f->pos+i] != '\n') {
        dst[i] = f->data[f->pos+i];
        ++i;
    }
    dst[i] = '\0';
    f->pos += i+1;
}

i32 main()
{
    i8 line[8] = {0};
    i64 currentGroupValue = 0;
    i64 maxGroupValues[3] = {0};
    struct str name = { "input", sizeof("input") - 1 };
    struct file input = OpenInputFile(name);
    if (!input.data) {
        return -input.len;
    }

    while (1) {
        u32 lineValue = 0;
        GetNextLine(&input, line);
        if (line[0] == '\0') { 
            if (currentGroupValue > maxGroupValues[0]) {
                maxGroupValues[2] = maxGroupValues[1];
                maxGroupValues[1] = maxGroupValues[0];
                maxGroupValues[0] = currentGroupValue;
            } else if (currentGroupValue > maxGroupValues[1]) {
                maxGroupValues[2] = maxGroupValues[1];
                maxGroupValues[1] = currentGroupValue;
            } else if (currentGroupValue > maxGroupValues[2]) {
                maxGroupValues[2] = currentGroupValue;
            }
            currentGroupValue = 0;
        }
        lineValue = atoi(line);
        currentGroupValue += lineValue;
        if (input.pos == input.len) { break; }
    }

    printf("max values are:\n\t%ld\n\t%ld\n\t%ld\nTotal: %ld\n", maxGroupValues[0], maxGroupValues[1], maxGroupValues[2], maxGroupValues[0] + maxGroupValues[1] + maxGroupValues[2]);
    
    return 0;
}
