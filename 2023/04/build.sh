#! /bin/bash

mkdir -p out/

gcc -o out/0401 src/01.c -Wall -Werror -Wpedantic -Wextra -ggdb
gcc -o out/0402 src/02.c -Wall -Werror -Wpedantic -Wextra -ggdb
