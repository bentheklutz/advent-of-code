package main

import (
	"fmt"
	"os"
	"strings"
)

func main() {
	problem_one := `
--- Day 8: Resonant Collinearity ---

You find yourselves on the roof of a top-secret Easter Bunny installation.

While The Historians do their thing, you take a look at the familiar huge
antenna. Much to your surprise, it seems to have been reconfigured to emit a
signal that makes people 0.1% more likely to buy Easter Bunny brand Imitation
Mediocre Chocolate as a Christmas gift! Unthinkable!

Scanning across the city, you find that there are actually many such antennas.
Each antenna is tuned to a specific frequency indicated by a single lowercase
letter, uppercase letter, or digit. You create a map (your puzzle input) of
these antennas. For example:

............
........0...
.....0......
.......0....
....0.......
......A.....
............
............
........A...
.........A..
............
............

The signal only applies its nefarious effect at specific antinodes based on the
resonant frequencies of the antennas. In particular, an antinode occurs at any
point that is perfectly in line with two antennas of the same frequency - but
only when one of the antennas is twice as far away as the other. This means that
for any pair of antennas with the same frequency, there are two antinodes, one
on either side of them.

So, for these two antennas with frequency a, they create the two antinodes
marked with #:

..........
...#......
..........
....a.....
..........
.....a....
..........
......#...
..........
..........

Adding a third antenna with the same frequency creates several more antinodes.
It would ideally add four antinodes, but two are off the right side of the map,
so instead it adds only two:

..........
...#......
#.........
....a.....
........a.
.....a....
..#.......
......#...
..........
..........

Antennas with different frequencies don't create antinodes; A and a count as
different frequencies. However, antinodes can occur at locations that contain
antennas. In this diagram, the lone antenna with frequency capital A creates no
antinodes but has a lowercase-a-frequency antinode at its location:

..........
...#......
#.........
....a.....
........a.
.....a....
..#.......
......A...
..........
..........

The first example has antennas with two different frequencies, so the antinodes
they create look like this, plus an antinode overlapping the topmost A-frequency
antenna:

......#....#
...#....0...
....#0....#.
..#....0....
....0....#..
.#....A.....
...#........
#......#....
........A...
.........A..
..........#.
..........#.

Because the topmost A-frequency antenna overlaps with a 0-frequency antinode,
there are 14 total unique locations that contain an antinode within the bounds
of the map.

Calculate the impact of the signal. How many unique locations within the bounds
of the map contain an antinode?`

	problem_two := `
--- Part Two ---

Watching over your shoulder as you work, one of The Historians asks if you took
the effects of resonant harmonics into your calculations.

Whoops!

After updating your model, it turns out that an antinode occurs at any grid
position exactly in line with at least two antennas of the same frequency,
regardless of distance. This means that some of the new antinodes will occur
at the position of each antenna (unless that antenna is the only one of its
frequency).

So, these three T-frequency antennas now create many antinodes:

T....#....
...T......
.T....#...
.........#
..#.......
..........
...#......
..........
....#.....
..........

In fact, the three T-frequency antennas are all exactly in line with two
antennas, so they are all also antinodes! This brings the total number of
antinodes in the above example to 9.

The original example now has 34 antinodes, including the antinodes that appear
on every antenna:

##....#....#
.#.#....0...
..#.#0....#.
..##...0....
....0....#..
.#...#A....#
...#..#.....
#....#.#....
..#.....A...
....#....A..
.#........#.
...#......##

Calculate the impact of the signal using this updated model. How many unique
locations within the bounds of the map contain an antinode?`

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
		fmt.Printf("%s\n\n%s\n", problem_one, problem_two)
		fmt.Printf("Usage: %s [-p] [-i <file_path>]\n", os.Args[0])
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

	width := len(lines[0])
	height := len(lines)

	antenna_locations := make(map[byte][]vec2)

	for i := 0; i < len(lines); i++ {
		line := lines[i]
		for j := 0; j < len(line); j++ {
			if line[j] != '.' {
				x := j
				y := len(lines) - (i + 1)
				antenna_locations[line[j]] = append(antenna_locations[line[j]], vec2{x, y})
			}
		}
	}

	antinodes := make([]vec2, 0)
	distinct_antinodes := make(map[vec2]bool)

	for _, locs := range antenna_locations {
		for i := 0; i < len(locs); i++ {
			first := locs[i]
			for j := i + 1; j < len(locs); j++ {
				second := locs[j]
				diff_vert := second.y - first.y
				diff_horz := second.x - first.x

				an1 := vec2{first.x - diff_horz, first.y - diff_vert}
				an2 := vec2{second.x + diff_horz, second.y + diff_vert}
				if an1.x < width && an1.x >= 0 && an1.y < height && an1.y >= 0 {
					antinodes = append(antinodes, an1)
					distinct_antinodes[an1] = true
				}
				if an2.x < width && an2.x >= 0 && an2.y < height && an2.y >= 0 {
					antinodes = append(antinodes, an2)
					distinct_antinodes[an2] = true
				}
			}
		}
	}

	num_nodes := 0
	for range distinct_antinodes {
		num_nodes++
	}

	fmt.Printf("Part One: There are %d antinodes total.\n", num_nodes)

	antinodes = antinodes[:0]
	clear(distinct_antinodes)
	for _, locs := range antenna_locations {
		for i := 0; i < len(locs); i++ {
			first := locs[i]
			antinodes = append(antinodes, first)
			distinct_antinodes[first] = true
			for j := i + 1; j < len(locs); j++ {
				second := locs[j]
				distinct_antinodes[second] = true
				antinodes = append(antinodes, second)
				diff_vert := second.y - first.y
				diff_horz := second.x - first.x

				an1 := vec2{first.x - diff_horz, first.y - diff_vert}
				for an1.x >= 0 && an1.x < width && an1.y >= 0 && an1.y < height {
					antinodes = append(antinodes, an1)
					distinct_antinodes[an1] = true
					an1.x -= diff_horz
					an1.y -= diff_vert
				}
				an2 := vec2{second.x + diff_horz, second.y + diff_vert}
				for an2.x >= 0 && an2.x < width && an2.y >= 0 && an2.y < height {
					antinodes = append(antinodes, an2)
					distinct_antinodes[an2] = true
					an2.x += diff_horz
					an2.y += diff_vert
				}
			}
		}
	}
	num_nodes = 0
	for range distinct_antinodes {
		num_nodes++
	}
	fmt.Printf("Part Two: There are %d antinodes total.\n", num_nodes)
}

type vec2 struct {
	x, y int
}
