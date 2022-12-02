#! /usr/bin/env bash

CC=gcc
CFlags='-g -Wall -Wextra -Wpedantic -Werror -ansi'
LDFlags='-lm'
$CC $CFlags $LDFlags -o 0101 01.c
$CC $CFlags $LDFlags -o 0102 02.c
