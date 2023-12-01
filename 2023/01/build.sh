#! /bin/bash

mkdir -p out/

gcc -o out/0101 src/01.c -Wall -Werror -Wpedantic -Wextra -ggdb
gcc -o out/0102 src/02.c -Wall -Werror -Wpedantic -Wextra -ggdb
