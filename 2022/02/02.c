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

struct file OpenFile(struct str name)
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

enum GameChoice {
    Unknown,
    Rock,
    Paper,
    Scissors
};

i32 main()
{
    i8 line[4];
    i32 totalScore = 0;
    struct str name = { "input", sizeof("input") - 1 };
    struct file input = OpenFile(name);

    if (!input.data) {
        printf("failed to open input file: %ld\n", -input.len);
        return -input.len;
    }

    while(1) {
        i32 pickScore = 0;
        i32 winScore = 0;
        enum GameChoice oppChoice = Unknown, ourChoice = Unknown;

        GetNextLine(&input, line);
        
        if (line[0] == 'A') {
            oppChoice = Rock;
        } else if (line[0] == 'B') {
            oppChoice = Paper;
        } else if (line[0] == 'C') {
            oppChoice = Scissors;
        }

        if (line[2] == 'X') { /* need to lose */
            if (oppChoice == Rock)          { ourChoice = Scissors; }
            else if (oppChoice == Paper)    { ourChoice = Rock; }
            else if (oppChoice == Scissors) { ourChoice = Paper; }
            winScore = 0;
        } else if (line[2] == 'Y') { /* need to draw */
            ourChoice = oppChoice;
            winScore = 3;
        } else if (line[2] == 'Z') { /* need a dub */
            if (oppChoice == Rock)          { ourChoice = Paper; }
            else if (oppChoice == Paper)    { ourChoice = Scissors; }
            else if (oppChoice == Scissors) { ourChoice = Rock; }
            winScore = 6;
        }

        pickScore = (i32) ourChoice;

        totalScore += pickScore + winScore;

        if (input.pos == input.len) { break; }
    }

    printf("total score is: %d\n", totalScore);
    
    return 0;
}
