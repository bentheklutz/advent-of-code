#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <errno.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

/* typedefs */
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

struct file {
    i8 *data;
    s64 size;
};

struct str {
    i8 *data;
    s64 size;
};

struct file OpenInputFile(i8 *name)
{
    i32 fd; u64 size; i8 *data;
    struct stat st;
    struct file result = {0};
    
    fd = open(name, O_RDONLY);
    if (0 < fd) {
        if (0 == stat(name, &st)) {
            size = (s64) st.st_size;

            data = mmap(0x0, size, PROT_READ, MAP_PRIVATE, fd, 0);
            if (data) {
                result.size = size;
                result.data = data;
            } else {
                result.size = -errno;
            }
        } else {
            result.size = -errno;
        }
    } else {
        result.size = -errno;
    }
    return result;
}

struct str NextLine(i8 *data, s64 max_size)
{
    struct str result = {0};
    s64 size = 0;

    while (size < max_size) {
        if (data[size] == '\n') {
            break;
        }
        ++size;
    }
    
    if (size) {
        result.data = data;
        result.size = size;
    }

    return result;
}

/*
 * +543
 + -> multiplier = 1
 5 -> 5*10^2-0
 4 -> 4*10^2-1
 3 -> 3*10^2-2
*/
s32 ParseValue(struct str line)
{
    s64 result = 0;
    s8 multiplier = 0;
    s64 total_digits = line.size - 1;
    s64 i = 1;

    if (line.data[0] == '+') {
        multiplier = 1;
    } else if (line.data[0] == '-') {
        multiplier = -1;
    }    


    while(line.data[i] >= '0' && line.data[i] <= '9') {
        i8 digit = line.data[i] - '0';
        s64 digit_scale = (total_digits) - i;
        result += digit * (s64)pow(10, digit_scale);
        ++i;
    }


    result *= multiplier;
    return result;
}

i32 main()
{
    s64 offset = 0;
    s64 frequency = 0;
    i8 *input_file_name = "input";
    struct file input_file = OpenInputFile(input_file_name);

    if (0 == input_file.data) {
        fprintf(stderr, "Failed to open input file \"%s\": %s\n", input_file_name, strerror(-input_file.size));
        return -input_file.size;
    }
    
    for (;;) {
        struct str line = NextLine(input_file.data + offset, input_file.size - offset);
        s32 line_value = 0;
        if (0 == line.size) {
            break;
        }
        
        offset += line.size + 1;
        line_value = ParseValue(line);
        frequency += line_value;
    }

    printf("the final frequency is %ld\n", frequency);

    return 0;
}


