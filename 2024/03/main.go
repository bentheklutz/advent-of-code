package main

import (
	"fmt"
	"os"
	"regexp"
	"strconv"
	"strings"
)

func main() {
	problem_one := `
--- Day 3: Mull It Over ---

"Our computers are having issues, so I have no idea if we have any Chief
Historians in stock! You're welcome to check the warehouse, though," says
the mildly flustered shopkeeper at the North Pole Toboggan Rental Shop. The
Historians head out to take a look.

The shopkeeper turns to you. "Any chance you can see why our computers are
having issues again?"

The computer appears to be trying to run a program, but its memory (your puzzle
input) is corrupted. All of the instructions have been jumbled up!

It seems like the goal of the program is just to multiply some numbers. It does
that with instructions like mul(X,Y), where X and Y are each 1-3 digit numbers.
For instance, mul(44,46) multiplies 44 by 46 to get a result of 2024. Similarly,
mul(123,4) would multiply 123 by 4.

However, because the program's memory has been corrupted, there are also many
invalid characters that should be ignored, even if they look like part of a
mul instruction. Sequences like mul(4*, mul(6,9!, ?(12,34), or mul ( 2 , 4 )
do nothing.

For example, consider the following section of corrupted memory:

xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))
 ^                           ^                       ^        ^
Only the four highlighted sections are real mul instructions. Adding up the
result of each instruction produces 161 (2*4 + 5*5 + 11*8 + 8*5).

Scan the corrupted memory for uncorrupted mul instructions. What do you get if
you add up all of the results of the multiplications?
	`
	problem_two := `
--- Part Two ---

As you scan through the corrupted memory, you notice that some of the
conditional statements are also still intact. If you handle some of the
uncorrupted conditional statements in the program, you might be able to get an
even more accurate result.

There are two new instructions you'll need to handle:

    The do() instruction enables future mul instructions.
    The don't() instruction disables future mul instructions.

Only the most recent do() or don't() instruction applies. At the beginning of
the program, mul instructions are enabled.

For example:

xmul(2,4)&mul[3,7]!^don't()_mul(5,5)+mul(32,64](mul(11,8)undo()?mul(8,5))

This corrupted memory is similar to the example from before, but this time the
mul(5,5) and mul(11,8) instructions are disabled because there is a don't()
instruction before them. The other mul instructions function normally, including
the one at the end that gets re-enabled by a do() instruction.

This time, the sum of the results is 48 (2*4 + 8*5).

Handle the new instructions; what do you get if you add up all of the results of
just the enabled multiplications?`

	have_file_name := false
	var file_name string
	for i := 0; i < len(os.Args); i++ {
		arg := os.Args[i]
		if arg == "-i" {
			if i == len(os.Args)-1 {
				fmt.Fprintf(os.Stderr, "The program requires that the '-i' input argument be followed by a file path argument.\n")
				os.Exit(1)
			}
			have_file_name = true
			file_name = os.Args[i+1]
			i += 1
		} else if arg == "-p" {
			fmt.Fprintf(os.Stderr, "%s\n", problem_one)
			fmt.Fprintf(os.Stderr, "%s\n", problem_two)
			return
		}
	}

	if !have_file_name {
		fmt.Fprintf(os.Stderr, "%s\n", problem_one)
		fmt.Fprintf(os.Stderr, "%s\n\n", problem_two)
		fmt.Fprintf(os.Stderr, "Usage: %s [-p] [-i <file_path>]\n", os.Args[0])
		return
	}

	file, err := os.Open(file_name)
	if err != nil {
		fmt.Fprintf(os.Stderr, "The program failed to open the input file (%s).\nThe error is %s.\n", file_name, err.Error())
		os.Exit(1)
	}
	file_info, err := file.Stat()
	if err != nil {
		fmt.Fprintf(os.Stderr, "The program failed to get size information about the input file.\nThe error is %s.\n", err.Error())
		os.Exit(1)
	}
	contents := make([]byte, file_info.Size())
	bytes_read, err := file.Read(contents)
	if err != nil {
		fmt.Fprintf(os.Stderr, "The program encountered an error reading the input file (%s).\nThe error is %s.\n", file_name, err.Error())
		os.Exit(1)
	}
	if bytes_read != len(contents) {
		fmt.Fprintf(os.Stderr, "The program only read %d of %d total input bytes.\n", bytes_read, len(contents))
		os.Exit(1)
	}

	mul_regex := regexp.MustCompile(`mul\([0-9][0-9]?[0-9]?,[0-9][0-9]?[0-9]?\)`)
	multiplies := mul_regex.FindAllString(string(contents), -1)

	fmt.Fprintf(os.Stderr, "There are %d valid mul instructions in the input.\n", len(multiplies))

	var sum int64 = 0
	for i := 0; i < len(multiplies); i++ {
		mul := multiplies[i]
		mul_args := strings.Split(strings.TrimSuffix(strings.TrimPrefix(mul, "mul("), ")"), ",")
		first, err := strconv.Atoi(mul_args[0])
		if err != nil {
			fmt.Fprintf(os.Stderr, "Malformed input in argument conversion. Double check the instruction match regex.\nThe error is %s.\n", err.Error())
			os.Exit(1)
		}
		second, err := strconv.Atoi(mul_args[1])
		if err != nil {
			fmt.Fprintf(os.Stderr, "Malformed input in argument conversion. Double check the instruction match regex.\nThe error is %s.\n", err.Error())
			os.Exit(1)
		}
		instruction_value := first * second
		fmt.Fprintf(os.Stderr, "%s -> %d\n", mul, instruction_value)
		sum += int64(instruction_value)
	}
	fmt.Fprintf(os.Stdout, "The input contains %d mul instructions whose total values accumulate to %d.\n", len(multiplies), sum)

	instruction_regex := regexp.MustCompile(`(mul\([0-9][0-9]?[0-9]?,[0-9][0-9]?[0-9]?\)|do\(\)|don't\(\))`)
	instructions := instruction_regex.FindAllString(string(contents), -1)

	fmt.Fprintf(os.Stderr, "There are %d instructions.\n", len(instructions))

	do_multiply := true
	sum = 0
	total_muls := 0
	for i := 0; i < len(instructions); i++ {
		instruction := instructions[i]
		if instruction == "do()" {
			do_multiply = true
			fmt.Fprintf(os.Stderr, "Multiply is now on.\n")
		} else if instruction == "don't()" {
			do_multiply = false
			fmt.Fprintf(os.Stderr, "Multiply is now off.\n")
		} else if do_multiply {
			total_muls += 1
			mul_args := strings.Split(strings.TrimSuffix(strings.TrimPrefix(instruction, "mul("), ")"), ",")
			first, err := strconv.Atoi(mul_args[0])
			if err != nil {
				fmt.Fprintf(os.Stderr, "Malformed input in argument conversion. Double check the instruction match regex.\nThe error is %s.\n", err.Error())
				os.Exit(1)
			}
			second, err := strconv.Atoi(mul_args[1])
			if err != nil {
				fmt.Fprintf(os.Stderr, "Malformed input in argument conversion. Double check the instruction match regex.\nThe error is %s.\n", err.Error())
				os.Exit(1)
			}
			instruction_value := first * second
			fmt.Fprintf(os.Stderr, "%s -> %d\n", instruction, instruction_value)
			sum += int64(instruction_value)
		}
	}
	fmt.Fprintf(os.Stdout, "The input contains %d mul instructions whose total values accumulate to %d.\n", total_muls, sum)
}
