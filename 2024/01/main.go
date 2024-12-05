package main

import (
	"fmt"
	"math"
	"os"
	"slices"
	"strconv"
	"strings"
)

func main() {
	problem_one := `
--- Day 1: Historian Hysteria ---

The Chief Historian is always present for the big Christmas sleigh launch, but
nobody has seen him in months! Last anyone heard, he was visiting locations that
are historically significant to the North Pole; a group of Senior Historians
has asked you to accompany them as they check the places they think he was most
likely to visit.

As each location is checked, they will mark it on their list with a star. They
figure the Chief Historian must be in one of the first fifty places they'll
look, so in order to save Christmas, you need to help them get fifty stars on
their list before Santa takes off on December 25th.

Collect stars by solving puzzles. Two puzzles will be made available on each
day in the Advent calendar; the second puzzle is unlocked when you complete the
first. Each puzzle grants one star. Good luck!

You haven't even left yet and the group of Elvish Senior Historians has already
hit a problem: their list of locations to check is currently empty. Eventually,
someone decides that the best place to check first would be the Chief
Historian's office.

Upon pouring into the office, everyone confirms that the Chief Historian is
indeed nowhere to be found. Instead, the Elves discover an assortment of notes
and lists of historically significant locations! This seems to be the planning
the Chief Historian was doing before he left. Perhaps these notes can be used to
determine which locations to search?

Throughout the Chief's office, the historically significant locations are listed
not by name but by a unique number called the location ID. To make sure they
don't miss anything, The Historians split into two groups, each searching the
office and trying to create their own complete list of location IDs.

There's just one problem: by holding the two lists up side by side (your puzzle
input), it quickly becomes clear that the lists aren't very similar. Maybe you
can help The Historians reconcile their lists?`

	problem_two := `
--- Part Two ---

Your analysis only confirmed what everyone feared: the two lists of location IDs
are indeed very different.

Or are they?

The Historians can't agree on which group made the mistakes or how to read
most of the Chief's handwriting, but in the commotion you notice an interesting
detail: a lot of location IDs appear in both lists! Maybe the other numbers
aren't location IDs at all but rather misinterpreted handwriting.

This time, you'll need to figure out exactly how often each number from the left
list appears in the right list. Calculate a total similarity score by adding up
each number in the left list after multiplying it by the number of times that
number appears in the right list.

Here are the same example lists again:

3   4
4   3
2   5
1   3
3   9
3   3

For these example lists, here is the process of finding the similarity score:

    The first number in the left list is 3. It appears in the right list three
        times, so the similarity score increases by 3 * 3 = 9.
    The second number in the left list is 4. It appears in the right list once,
        so the similarity score increases by 4 * 1 = 4.
    The third number in the left list is 2. It does not appear in the right
        list, so the similarity score does not increase (2 * 0 = 0).
    The fourth number, 1, also does not appear in the right list.
    The fifth number, 3, appears in the right list three times; the similarity
        score increases by 9.
    The last number, 3, appears in the right list three times; the similarity
        score again increases by 9.

So, for these example lists, the similarity score at the end of this process is
31 (9 + 4 + 0 + 0 + 9 + 9).

Once again consider your left and right lists. What is their similarity score?`

	have_input := false
	var file_name string

	for idx, arg := range os.Args {
		if arg == "-p" {
			fmt.Printf("%s\n", problem_one)
			fmt.Printf("%s\n", problem_two)
		}
		if arg == "-i" {
			if len(os.Args) < idx+2 {
				fmt.Fprintf(os.Stderr, "You must provide a file name following the '-i' input argument.\n")
				os.Exit(1)
			}
			file_name = os.Args[idx+1]
			have_input = true
		}
	}

	if !have_input {
		fmt.Printf("%s\n", problem_one)
		fmt.Printf("%s\n", problem_two)
		return
	}

	file, err := os.Open(file_name)
	if err != nil {
		fmt.Fprintf(os.Stderr, "The program failed to open your input file with this error: %s\n", err)
		os.Exit(1)
	}

	file_info, err := file.Stat()
	if err != nil {
		fmt.Fprintf(os.Stderr, "The program failed to get file info for the input file and therefore cannot find its size.\nHere is the error: '%s'", err)
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

	lines := strings.Split(string(contents), "\n")
	lines = lines[:len(lines)-1]

	lefts, rights := extractNumbers(lines)

	total_diff := findTotalDiff(lefts, rights)

	fmt.Printf("The total diff is %d.\n", int64(total_diff))

	total_similarity_score := findSimilarityScore(lefts, rights)

	fmt.Printf("The total similarity score is %d.\n", total_similarity_score)
}

func extractNumbers(lines []string) ([]int, []int) {
	lefts := make([]int, len(lines))
	rights := make([]int, len(lines))
	for idx, line := range lines {
		nums := strings.Split(line, "   ")
		if len(nums) != 2 {
			fmt.Fprintf(os.Stderr, "Line %d was not formatted as expected: there were %d items when splitting the line by ' ', but we expected there to be 2.\n", idx+1, len(nums))
			os.Exit(1)
		}
		left, err := strconv.Atoi(nums[0])
		if err != nil {
			fmt.Fprintf(os.Stderr, "Line %d: The input %s is not a valid number.\n", idx+1, nums[0])
			os.Exit(1)
		}
		lefts[idx] = left
		right, err := strconv.Atoi(nums[1])
		if err != nil {
			fmt.Fprintf(os.Stderr, "Line %d: The input %s is not a valid number.\n", idx+1, nums[1])
			os.Exit(1)
		}
		rights[idx] = right
	}
	slices.Sort(lefts)
	slices.Sort(rights)
	return lefts, rights
}

func findTotalDiff(lefts, rights []int) int {
	total_diff := 0
	for i := 0; i < len(lefts); i++ {
		diff := int(math.Abs(float64(lefts[i] - rights[i])))
		total_diff += diff
	}
	return total_diff
}

func findSimilarityScore(lefts, rights []int) int {
	total_similarity_score := 0
	similarity_scores := make([]int, len(lefts))
	for i := 0; i < len(lefts); i++ {
		for j := 0; j < len(lefts); j++ {
			if rights[j] == lefts[i] {
				similarity_scores[i] += 1
			}
		}
		similarity_scores[i] *= lefts[i]
		total_similarity_score += similarity_scores[i]
	}
	return total_similarity_score
}
