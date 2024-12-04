package main

import (
	"fmt"
	"os"
	"strings"
)

func main() {

	problem_one := `
--- Day 4: Ceres Search ---

"Looks like the Chief's not here. Next!" One of The Historians pulls out a
device and pushes the only button on it. After a brief flash, you recognize the
interior of the Ceres monitoring station!

As the search for the Chief continues, a small Elf who lives on the station tugs
on your shirt; she'd like to know if you could help her with her word search
(your puzzle input). She only has to find one word: XMAS.

This word search allows words to be horizontal, vertical, diagonal, written
backwards, or even overlapping other words. It's a little unusual, though, as
you don't merely need to find one instance of XMAS - you need to find all of
them. Here are a few ways XMAS might appear, where irrelevant characters have
been replaced with .:

..X...
.SAMX.
.A..A.
XMAS.S
.X....

The actual word search will be full of letters instead. For example:

MMMSXXMASM
MSAMXMSMSA
AMXSXMAAMM
MSAMASMSMX
XMASAMXAMM
XXAMMXXAMA
SMSMSASXSS
SAXAMASAAA
MAMMMXMMMM
MXMXAXMASX

In this word search, XMAS occurs a total of 18 times; here's the same word
search again, but where letters not involved in any XMAS have been replaced
with .:

....XXMAS.
.SAMXMS...
...S..A...
..A.A.MS.X
XMASAMX.MM
X.....XA.A
S.S.S.S.SS
.A.A.A.A.A
..M.M.M.MM
.X.X.XMASX

Take a look at the little Elf's word search. How many times does XMAS appear?`

	problem_two := `
--- Part Two ---

The Elf looks quizzically at you. Did you misunderstand the assignment?

Looking for the instructions, you flip over the word search to find that this
isn't actually an XMAS puzzle; it's an X-MAS puzzle in which you're supposed to
find two MAS in the shape of an X. One way to achieve that is like this:

M.S
.A.
M.S

Irrelevant characters have again been replaced with . in the above diagram.
Within the X, each MAS can be written forwards or backwards.

Here's the same example from before, but this time all of the X-MASes have been
kept instead:

.M.S......
..A..MSMS.
.M.S.MAA..
..A.ASMSM.
.M.S.M....
..........
S.S.S.S.S.
.A.A.A.A..
M.M.M.M.M.
..........

In this example, an X-MAS appears 9 times.

Flip the word search from the instructions back over to the word search side and
try again. How many times does an X-MAS appear?`

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

	num_matches := findXMAS(lines)

	fmt.Printf("The input file has %d total matches for 'XMAS'.\n", num_matches)

	num_matches = findMASCrossed(lines)

	fmt.Printf("The input file has %d total matches for crossing instances of 'MAS'.\n", num_matches)
}

type Indices struct {
	i int
	j int
}

func findMASCrossed(lines []string) int {
	matches_by_a_pos := make(map[Indices]int, 0)
	for i := 0; i < len(lines); i++ {
		line := lines[i]
		for j := 0; j < len(line); j++ {
			if line[j] == 'M' {
				room_left := j >= 2
				room_right := j <= len(line)-3
				room_up := i >= 2
				room_down := i <= len(lines)-3
				if room_left && room_down {
					// check backwards row and forwards column
					if lines[i+1][j-1] == 'A' && lines[i+2][j-2] == 'S' {
						index := Indices{i + 1, j - 1}
						matches_for_this_a := matches_by_a_pos[index]
						matches_by_a_pos[index] = matches_for_this_a + 1
					}
				}
				if room_right && room_down {
					// check forwards row and forwards column
					if lines[i+1][j+1] == 'A' && lines[i+2][j+2] == 'S' {
						index := Indices{i + 1, j + 1}
						matches_for_this_a := matches_by_a_pos[index]
						matches_by_a_pos[index] = matches_for_this_a + 1
					}
				}
				if room_left && room_up {
					// check backwards row and backwards column
					if lines[i-1][j-1] == 'A' && lines[i-2][j-2] == 'S' {
						index := Indices{i - 1, j - 1}
						matches_for_this_a := matches_by_a_pos[index]
						matches_by_a_pos[index] = matches_for_this_a + 1
					}
				}
				if room_right && room_up {
					// check forwards r0w and backwards column
					if lines[i-1][j+1] == 'A' && lines[i-2][j+2] == 'S' {
						index := Indices{i - 1, j + 1}
						matches_for_this_a := matches_by_a_pos[index]
						matches_by_a_pos[index] = matches_for_this_a + 1
					}
				}
			}
		}
	}

	num_matches := 0

	for _, matches := range matches_by_a_pos {
		if matches == 2 {
			num_matches += 1
		}
	}

	return num_matches
}

func findXMAS(lines []string) int {
	num_matches := 0
	for i := 0; i < len(lines); i++ {
		line := lines[i]
		for j := 0; j < len(line); j++ {
			if line[j] == 'X' {
				room_left := j >= 3
				room_right := j <= len(line)-4
				room_up := i >= 3
				room_down := i <= len(lines)-4
				if room_left {
					// check backwards in this row
					if line[j-1] == 'M' && line[j-2] == 'A' && line[j-3] == 'S' {
						num_matches += 1
					}
				}
				if room_up {
					// check backwards in this column
					if lines[i-1][j] == 'M' && lines[i-2][j] == 'A' && lines[i-3][j] == 'S' {
						num_matches += 1
					}
				}
				if room_right {
					// check forwards in this row
					if line[j+1] == 'M' && line[j+2] == 'A' && line[j+3] == 'S' {
						num_matches += 1
					}
				}
				if room_down {
					// check forwards in this column
					if lines[i+1][j] == 'M' && lines[i+2][j] == 'A' && lines[i+3][j] == 'S' {
						num_matches += 1
					}
				}
				if room_left && room_down {
					// check backwards row and forwards column
					if lines[i+1][j-1] == 'M' && lines[i+2][j-2] == 'A' && lines[i+3][j-3] == 'S' {
						num_matches += 1
					}
				}
				if room_right && room_down {
					// check forwards row and forwards column
					if lines[i+1][j+1] == 'M' && lines[i+2][j+2] == 'A' && lines[i+3][j+3] == 'S' {
						num_matches += 1
					}
				}
				if room_left && room_up {
					// check backwards row and backwards column
					if lines[i-1][j-1] == 'M' && lines[i-2][j-2] == 'A' && lines[i-3][j-3] == 'S' {
						num_matches += 1
					}
				}
				if room_right && room_up {
					// check forwards r0w and backwards column
					if lines[i-1][j+1] == 'M' && lines[i-2][j+2] == 'A' && lines[i-3][j+3] == 'S' {
						num_matches += 1
					}
				}
			}
		}
	}
	return num_matches
}
