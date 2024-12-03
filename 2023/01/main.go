package main

import (
	"fmt"
	"os"
	"strconv"
	"strings"
)

func main() {

	problem_one := `
--- Day 1: Trebuchet?! ---

Something is wrong with global snow production, and you've been selected to take
a look. The Elves have even given you a map; on it, they've used stars to mark
the top fifty locations that are likely to be having problems.

You've been doing this long enough to know that to restore snow operations, you
need to check all fifty stars by December 25th.

Collect stars by solving puzzles. Two puzzles will be made available on each
day in the Advent calendar; the second puzzle is unlocked when you complete the
first. Each puzzle grants one star. Good luck!

You try to ask why they can't just use a weather machine ("not powerful enough")
and where they're even sending you ("the sky") and why your map looks mostly
blank ("you sure ask a lot of questions") and hang on did you just say the sky
("of course, where do you think snow comes from") when you realize that the
Elves are already loading you into a trebuchet ("please hold still, we need to
strap you in").

As they're making the final adjustments, they discover that their calibration
document (your puzzle input) has been amended by a very young Elf who was
apparently just excited to show off her art skills. Consequently, the Elves are
having trouble reading the values on the document.

The newly-improved calibration document consists of lines of text; each line
originally contained a specific calibration value that the Elves now need to
recover. On each line, the calibration value can be found by combining the first
digit and the last digit (in that order) to form a single two-digit number.

For example:

1abc2
pqr3stu8vwx
a1b2c3d4e5f
treb7uchet

In this example, the calibration values of these four lines are 12, 38, 15, and 77. Adding these together produces 142.

Consider your entire calibration document. What is the sum of all of the calibration values?
`

	problem_two := `
--- Part Two ---

Your calculation isn't quite right. It looks like some of the digits are
actually spelled out with letters: one, two, three, four, five, six, seven,
eight, and nine also count as valid "digits".

Equipped with this new information, you now need to find the real first and last
digit on each line. For example:

two1nine
eightwothree
abcone2threexyz
xtwone3four
4nineeightseven2
zoneight234
7pqrstsixteen

In this example, the calibration values are 29, 83, 13, 24, 42, 14, and 76.
Adding these together produces 281.

What is the sum of all of the calibration values?
`

	have_file_name := false
	var file_name string
	problem_two_only := false
	problem_one_only := false
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
		}
		if arg == "-p1" {
			problem_one_only = true
		}
		if arg == "-p2" {
			problem_two_only = true
		}
	}

	if !have_file_name {
		fmt.Fprintf(os.Stderr, "%s\n\n%s\n\n", problem_one, problem_two)
		fmt.Fprintf(os.Stderr, "Usage '%s' [-p] [-i <file_path>]\n", os.Args[0])
		os.Exit(1)
	}

	file, err := os.Open(file_name)
	if err != nil {
		fmt.Fprintf(os.Stderr, "The program failed to open the input file (%s). The error was %s.\n", file_name, err.Error())
		os.Exit(1)
	}
	file_info, err := file.Stat()
	if err != nil {
		fmt.Fprintf(os.Stderr, "The program failed to get the input file size. The error was %s.\n", err.Error())
		os.Exit(1)
	}

	contents := make([]byte, file_info.Size())
	bytes_read, err := file.Read(contents)
	if err != nil {
		fmt.Fprintf(os.Stderr, "The program failed to read the input file (%s). The error was %s.\n", file_name, err.Error())
		os.Exit(1)
	}
	if bytes_read != len(contents) {
		fmt.Fprintf(os.Stderr, "The program read %d bytes, which was not the full input file.\n", bytes_read)
		os.Exit(1)
	}
	lines := strings.Split(string(contents), "\n")
	lines = lines[:len(lines)-1]

	fmt.Fprintf(os.Stderr, "The file is %d lines long.\n", len(lines))

	/*
		Problem 1) First and last digit of each line make a number, sum these numbers.
	*/
	if !problem_two_only {
		sum := 0
		for i := 0; i < len(lines); i++ {
			line := lines[i]
			isDigit := func(r rune) bool {
				return r >= '0' && r <= '9'
			}
			first_digit := strings.IndexFunc(line, isDigit)
			last_digit := strings.LastIndexFunc(line, isDigit)
			number_bytes := make([]byte, 2)
			number_bytes[0] = line[first_digit]
			number_bytes[1] = line[last_digit]
			number, err := strconv.Atoi(string(number_bytes))
			if err != nil {
				fmt.Fprintf(os.Stderr, "The program failed to parse the input file properly. The error was %s.\n", err.Error())
				os.Exit(1)
			}
			sum += number
		}
		fmt.Fprintf(os.Stdout, "The sum of all the numbers is %d.\n", sum)
	}

	/*
		Problem 2) First and last digit or english number ("one", "two", "three", etc) are the
		digits for the number. Find the sum of these instead.
	*/
	if !problem_one_only {
		sum := 0
		for i := 0; i < len(lines); i++ {
			line := lines[i]
			isDigit := func(r rune) bool {
				return r >= '0' && r <= '9'
			}
			first_digit_idx := strings.IndexFunc(line, isDigit)
			last_digit_idx := strings.LastIndexFunc(line, isDigit)
			var first_digit byte
			var last_digit byte
			if first_digit_idx >= 0 {
				first_digit = line[first_digit_idx]
			}
			if last_digit_idx >= 0 {
				last_digit = line[last_digit_idx]
			}
			digit_names := []string{"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"}
			for j := 0; j < len(digit_names); j++ {
				start := strings.Index(line, digit_names[j])
				if start >= 0 {
					if start < first_digit_idx || first_digit_idx == -1 {
						first_digit = byte('0' + j)
						first_digit_idx = start
					}
				}
				end := strings.LastIndex(line, digit_names[j])
				if end >= 0 {
					if end > last_digit_idx || last_digit_idx == -1 {
						last_digit = byte('0' + j)
						last_digit_idx = end
					}
				}
			}

			number_bytes := make([]byte, 2)
			number_bytes[0] = first_digit
			number_bytes[1] = last_digit
			fmt.Fprintf(os.Stderr, "The number for this line is %s.\n", string(number_bytes))
			number, err := strconv.Atoi(string(number_bytes))
			if err != nil {
				fmt.Fprintf(os.Stderr, "The program failed to parse the input file properly. The error was %s.\n", err.Error())
				os.Exit(1)
			}
			sum += number
		}
		fmt.Fprintf(os.Stdout, "The sum of all the numbers is %d.\n", sum)
	}
}
