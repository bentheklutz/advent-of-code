package main

import (
	"fmt"
	"os"
	"strconv"
	"strings"
)

func main() {

	problem_one := `
--- Day 2: Red-Nosed Reports ---

Fortunately, the first location The Historians want to search isn't a long walk
from the Chief Historian's office.

While the Red-Nosed Reindeer nuclear fusion/fission plant appears to contain no
sign of the Chief Historian, the engineers there run up to you as soon as they
see you. Apparently, they still talk about the time Rudolph was saved through
molecular synthesis from a single electron.

They're quick to add that - since you're already here - they'd really appreciate
your help analyzing some unusual data from the Red-Nosed reactor. You turn
to check if The Historians are waiting for you, but they seem to have already
divided into groups that are currently searching every corner of the facility.
You offer to help with the unusual data.

The unusual data (your puzzle input) consists of many reports, one report per
line. Each report is a list of numbers called levels that are separated by
spaces. For example:

7 6 4 2 1
1 2 7 8 9
9 7 6 2 1
1 3 2 4 5
8 6 4 4 1
1 3 6 7 9

This example data contains six reports each containing five levels.

The engineers are trying to figure out which reports are safe. The Red-Nosed
reactor safety systems can only tolerate levels that are either gradually
increasing or gradually decreasing. So, a report only counts as safe if both of
the following are true:

    The levels are either all increasing or all decreasing.
    Any two adjacent levels differ by at least one and at most three.

In the example above, the reports can be found safe or unsafe by checking those rules:

    7 6 4 2 1: Safe because the levels are all decreasing by 1 or 2.
    1 2 7 8 9: Unsafe because 2 7 is an increase of 5.
    9 7 6 2 1: Unsafe because 6 2 is a decrease of 4.
    1 3 2 4 5: Unsafe because 1 3 is increasing but 3 2 is decreasing.
    8 6 4 4 1: Unsafe because 4 4 is neither an increase or a decrease.
    1 3 6 7 9: Safe because the levels are all increasing by 1, 2, or 3.

So, in this example, 2 reports are safe.

Analyze the unusual data from the engineers. How many reports are safe?`

	problem_two := `
--- Part Two ---

The engineers are surprised by the low number of safe reports until they realize
they forgot to tell you about the Problem Dampener.

The Problem Dampener is a reactor-mounted module that lets the reactor safety
systems tolerate a single bad level in what would otherwise be a safe report.
It's like the bad level never happened!

Now, the same rules apply as before, except if removing a single level from an
unsafe report would make it safe, the report instead counts as safe.

More of the above example's reports are now safe:

    7 6 4 2 1: Safe without removing any level.
    1 2 7 8 9: Unsafe regardless of which level is removed.
    9 7 6 2 1: Unsafe regardless of which level is removed.
    1 3 2 4 5: Safe by removing the second level, 3.
    8 6 4 4 1: Safe by removing the third level, 4.
    1 3 6 7 9: Safe without removing any level.

Thanks to the Problem Dampener, 4 reports are actually safe!

Update your analysis by handling situations where the Problem Dampener can
remove a single level from unsafe reports. How many reports are now safe?`

	var file_name string
	have_file_name := false

	for i, arg := range os.Args {
		if arg == "-p" {
			fmt.Printf("%s\n\n%s\n", problem_one, problem_two)
			return
		}
		if arg == "-i" {
			if i == len(os.Args)-1 {
				fmt.Fprintf(os.Stderr, "The program requires that the '-i' argument be followed by a file name argument\n")
				os.Exit(1)
			}
			file_name = os.Args[i+1]
			have_file_name = true
		}
	}

	if !have_file_name {
		fmt.Fprintf(os.Stderr, "%s\n\n%s\n\n", problem_one, problem_two)
		fmt.Fprintf(os.Stderr, "Usage '%s' [-p] [-i <file_path>]\n", os.Args[0])
		return
	}

	file, err := os.Open(file_name)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Failed to open the input file (%s). The error is %s.\n", file_name, err.Error())
		os.Exit(1)
	}
	file_info, err := file.Stat()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Failed to get the input file's (%s) size. The program cannot proceed. The error is %s.\n", file_name, err.Error())
		os.Exit(1)
	}

	contents := make([]byte, file_info.Size())
	bytes_read, err := file.Read(contents)
	if err != nil {
		fmt.Fprintf(os.Stderr, "The program encountered an error while reading the input file. The error is %s.\n", err.Error())
		os.Exit(1)
	}
	if bytes_read != len(contents) {
		fmt.Fprintf(os.Stderr, "The program only read %d bytes into the contents buffer, which is not the entire file contents.\n", bytes_read)
		os.Exit(1)
	}

	lines := strings.Split(string(contents), "\n")
	lines = lines[:len(lines)-1]

	nums := extractNumbers(lines)
	/*
		Part One
		A Report is safe when:
		1) All numbers are either increasing or decreasing
		2) The difference between each pair of numbers is no greater than 3.
	*/
	num_safe_reports := countSafeReports(nums)
	fmt.Printf("Part One: The program determined that %d reports were safe.\n", num_safe_reports)

	/*
		Part Two
		If removing a single element makes the report safe, then the report is now safe.
		Otherwise, the same rules apply as before.
	*/
	num_safe_reports = countSafeReportsWithTolerance(nums)

	fmt.Printf("Part Two: The program determined that %d reports were safe.\n", num_safe_reports)
}

func countSafeReports(nums [][]int) int {
	num_safe_reports := 0
	for i := 0; i < len(nums); i++ {
		report_safe := reportSafe(nums[i])
		if report_safe {
			num_safe_reports += 1
		}
	}
	return num_safe_reports
}

func countSafeReportsWithTolerance(nums [][]int) int {
	num_safe_reports := 0
	retries := make([]int, 0)
	for i := 0; i < len(nums); i++ {
		report_safe := reportSafe(nums[i])

		if report_safe {
			num_safe_reports += 1
		} else {
			retries = append(retries, i)
		}
	}

	for len(retries) > 0 {
		retrying := nums[retries[0]]
		for i := 0; i < len(retrying); i++ {
			shorter := make([]int, i)
			copy(shorter, retrying[:i])
			shorter = append(shorter, retrying[i+1:]...)
			if reportSafe(shorter) {
				num_safe_reports += 1
				break
			}
		}
		retries = retries[1:]
	}
	return num_safe_reports
}

func extractNumbers(lines []string) [][]int {
	nums := make([][]int, len(lines))
	for i, line := range lines {
		num_strings := strings.Split(line, " ")
		nums[i] = make([]int, len(num_strings))
		for j, num_string := range num_strings {
			num, err := strconv.Atoi(num_string)
			if err != nil {
				fmt.Fprintf(os.Stderr, "Line %d: The program could not convert '%s' into an int for processing. The error is %s.\n", i+1, num_string, err.Error())
				os.Exit(1)
			}
			nums[i][j] = num
		}
	}
	return nums
}

func reportSafe(numbers []int) bool {
	increasing := numbers[1] > numbers[0]
	for j := 0; j < len(numbers)-1; j++ {
		if increasing {
			if numbers[j+1] <= numbers[j] {
				// fmt.Fprintf(os.Stderr, "Line %d has a non-increasing pair (%d, %d) when it should be increasing.\n", i, numbers[j], numbers[j+1])
				return false
			}
			if numbers[j+1]-numbers[j] > 3 {
				// fmt.Fprintf(os.Stderr, "Line %d has a pair differing by more than 3 (%d, %d).\n", i, numbers[j], numbers[j+1])
				return false
			}
		} else {
			if numbers[j+1] >= numbers[j] {
				// fmt.Fprintf(os.Stderr, "Line %d has a non-decreasing pair (%d, %d) when it should be decreasing.\n", i, numbers[j], numbers[j+1])
				return false
			}
			if numbers[j]-numbers[j+1] > 3 {
				// fmt.Fprintf(os.Stderr, "Line %d has a pair differing by more than 3 (%d, %d).\n", i, numbers[j], numbers[j+1])
				return false
			}
		}
	}
	return true
}
