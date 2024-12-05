package main

import (
	"fmt"
	"os"
	"slices"
	"strconv"
	"strings"
)

func main() {

	problem_one := `
--- Day 5: Print Queue ---

Satisfied with their search on Ceres, the squadron of scholars suggests
subsequently scanning the stationery stacks of sub-basement 17.

The North Pole printing department is busier than ever this close to Christmas,
and while The Historians continue their search of this historically significant
facility, an Elf operating a very familiar printer beckons you over.

The Elf must recognize you, because they waste no time explaining that the new
sleigh launch safety manual updates won't print correctly. Failure to update the
safety manuals would be dire indeed, so you offer your services.

Safety protocols clearly indicate that new pages for the safety manuals must
be printed in a very specific order. The notation X|Y means that if both page
number X and page number Y are to be produced as part of an update, page number
X must be printed at some point before page number Y.

The Elf has for you both the page ordering rules and the pages to produce in
each update (your puzzle input), but can't figure out whether each update has
the pages in the right order.

For example:

47|53
97|13
97|61
97|47
75|29
61|13
75|53
29|13
97|29
53|29
61|53
97|53
61|29
47|13
75|47
97|75
47|61
75|61
47|29
75|13
53|13

75,47,61,53,29
97,61,53,29,13
75,29,13
75,97,47,61,53
61,13,29
97,13,75,29,47

The first section specifies the page ordering rules, one per line. The first
rule, 47|53, means that if an update includes both page number 47 and page
number 53, then page number 47 must be printed at some point before page number
53. (47 doesn't necessarily need to be immediately before 53; other pages are
allowed to be between them.)

The second section specifies the page numbers of each update. Because most
safety manuals are different, the pages needed in the updates are different too.
The first update, 75,47,61,53,29, means that the update consists of page numbers
75, 47, 61, 53, and 29.

To get the printers going as soon as possible, start by identifying which
updates are already in the right order.

In the above example, the first update (75,47,61,53,29) is in the right order:

    75 is correctly first because there are rules that put each other page after
       it: 75|47, 75|61, 75|53, and 75|29.
    47 is correctly second because 75 must be before it (75|47) and every other
       page must be after it according to 47|61, 47|53, and 47|29.
    61 is correctly in the middle because 75 and 47 are before it (75|61 and 47|
       61) and 53 and 29 are after it (61|53 and 61|29).
    53 is correctly fourth because it is before page number 29 (53|29).
    29 is the only page left and so is correctly last.

Because the first update does not include some page numbers, the ordering rules
involving those missing page numbers are ignored.

The second and third updates are also in the correct order according to the
rules. Like the first update, they also do not include every page number, and so
only some of the ordering rules apply - within each update, the ordering rules
that involve missing page numbers are not used.

The fourth update, 75,97,47,61,53, is not in the correct order: it would print
75 before 97, which violates the rule 97|75.

The fifth update, 61,13,29, is also not in the correct order, since it breaks
the rule 29|13.

The last update, 97,13,75,29,47, is not in the correct order due to breaking
several rules.

For some reason, the Elves also need to know the middle page number of
each update being printed. Because you are currently only printing the
correctly-ordered updates, you will need to find the middle page number of each
correctly-ordered update. In the above example, the correctly-ordered updates
are:

75,47,61,53,29
97,61,53,29,13
75,29,13

These have middle page numbers of 61, 53, and 29 respectively. Adding these page
numbers together gives 143.

Of course, you'll need to be careful: the actual list of page ordering rules is
bigger and more complicated than the above example.

Determine which updates are already in the correct order. What do you get if you
add up the middle page number from those correctly-ordered updates?`

	problem_two := `
--- Part Two ---

While the Elves get to work printing the correctly-ordered updates, you have a
little time to fix the rest of them.

For each of the incorrectly-ordered updates, use the page ordering rules to put
the page numbers in the right order. For the above example, here are the three
incorrectly-ordered updates and their correct orderings:

    75,97,47,61,53 becomes 97,75,47,61,53.
    61,13,29 becomes 61,29,13.
    97,13,75,29,47 becomes 97,75,47,29,13.

After taking only the incorrectly-ordered updates and ordering them correctly,
their middle page numbers are 47, 29, and 47. Adding these together produces
123.

Find the updates which are not in the correct order. What do you get if you add
up the middle page numbers after correctly ordering just those updates?`

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
		}
		if arg == "-p" {
			fmt.Printf("%s\n%s\n", problem_one, problem_two)
			return
		}
	}

	if !have_file_name {
		fmt.Printf("%s\n%s\n", problem_one, problem_two)
		fmt.Printf("Usage: %s [-p] [-i <file_path>]\n", os.Args[0])
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

	lines := strings.Split(string(contents), "\n")
	lines = lines[:len(lines)-1]

	var first_rule int = 0
	_ = first_rule
	var first_ordering int
	for i := 0; i < len(lines); i++ {
		line := lines[i]
		if line == "" {
			first_ordering = i + 1
		}
	}

	if first_ordering == 0 {
		fmt.Fprintf(os.Stderr, "The input does not have a blank line separating the ordering rules from the actual page orderings. The program cannot proceed.\n")
		os.Exit(1)
	}

	rules := make([]rule, 0)

	for i := 0; i < first_ordering-1; i++ {
		line := lines[i]
		page_values := strings.Split(line, "|")
		if len(page_values) != 2 {
			fmt.Fprintf(os.Stderr, "The program found that line %d has %d page values instead of 2.\n", i+1, len(page_values))
		}
		before, err := strconv.Atoi(page_values[0])
		if err != nil {
			fmt.Fprintf(os.Stderr, "Line %d: The program cannot convert the value %s into an integer. The error is %s.\n", i+1, page_values[0], err.Error())
			os.Exit(1)
		}
		after, err := strconv.Atoi(page_values[1])
		if err != nil {
			fmt.Fprintf(os.Stderr, "Line %d: The program cannot convert the value %s into an integer. The error is %s.\n", i+1, page_values[1], err.Error())
			os.Exit(1)
		}
		rules = append(rules, rule{before: before, after: after})
	}

	orderings := make([][]int, 0)

	for i := first_ordering; i < len(lines); i++ {
		line := lines[i]
		page_values := strings.Split(line, ",")
		order := make([]int, 0)
		for j := 0; j < len(page_values); j++ {
			value, err := strconv.Atoi(page_values[j])
			if err != nil {
				fmt.Fprintf(os.Stderr, "Line %d: The program cannot convert the value %s into an integer. The error is %s.\n", i+1, page_values[0], err.Error())
				os.Exit(1)
			}
			order = append(order, value)
		}
		orderings = append(orderings, order)
	}

	valids, invalids := findOrderValidities(rules, orderings)
	valid_sum := 0
	for _, order := range valids {
		valid_sum += order[len(order)/2]
	}
	fmt.Printf("The sum of the middle pages in all valid print orderings is %d.\n", valid_sum)

	// Construct a sort function based on the rules, then sort each order using it.

	sortFromRules := func(l, r int) int {
		for _, rule := range rules {
			if rule.before == l && rule.after == r {
				return -1
			}
			if rule.after == l && rule.before == r {
				return 1
			}
		}
		fmt.Fprintf(os.Stderr, "Failed to sort the invalid slice based on the rules.\n")
		os.Exit(1)
		return 0
	}

	invalid_sum := 0
	for i := 0; i < len(invalids); i++ {
		order := invalids[i]
		slices.SortFunc(order, sortFromRules)
		invalid_sum += order[len(order)/2]
	}

	fmt.Printf("The sum of the middle pages in all invalid print orderings after being fixed is %d.\n", invalid_sum)

}

func findOrderValidities(rules []rule, orderings [][]int) ([][]int, [][]int) {
	valids := make([][]int, 0)
	invalids := make([][]int, 0)
	for i := 0; i < len(orderings); i++ {
		order := orderings[i]

		valid := true
		may_not_find := make([]int, 0)
		for j := 0; j < len(order); j++ {
			page := order[j]
			if slices.Contains(may_not_find, page) {
				valid = false
				break
			}
			for k := 0; k < len(rules); k++ {
				rule := rules[k]
				if page == rule.after {
					// fmt.Fprintf(os.Stderr, "%d found, %d is no longer allowed.\n", page, rule.before)
					may_not_find = append(may_not_find, rule.before)
				}
			}
		}
		if valid {
			valids = append(valids, order)
		} else {
			invalids = append(invalids, order)
		}
	}
	return valids, invalids
}

type rule struct {
	before int
	after  int
}
