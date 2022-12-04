#! /usr/bin/env bash

CC=gcc
CFlags='-g -Wall -Wextra -Wpedantic -Werror -ansi'
LDFlags='-lm'
$CC $CFlags $LDFlags -o 0401 01.c
$CC $CFlags $LDFlags -o 0402 02.c
