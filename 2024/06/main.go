package main

import (
	"fmt"
	"os"
	"strings"
)

func main() {
	problem_one := `
--- Day 6: Guard Gallivant ---

The Historians use their fancy device again, this time to whisk you all away
to the North Pole prototype suit manufacturing lab... in the year 1518! It
turns out that having direct access to history is very convenient for a group
of historians.

You still have to be careful of time paradoxes, and so it will be important to
avoid anyone from 1518 while The Historians search for the Chief. Unfortunately,
a single guard is patrolling this part of the lab.

Maybe you can work out where the guard will go ahead of time so that The
Historians can search safely?

You start by making a map (your puzzle input) of the situation. For example:

....#.....
.........#
..........
..#.......
.......#..
..........
.#..^.....
........#.
#.........
......#...

The map shows the current position of the guard with ^ (to indicate the guard is
currently facing up from the perspective of the map). Any obstructions - crates,
desks, alchemical reactors, etc. - are shown as #.

Lab guards in 1518 follow a very strict patrol protocol which involves
repeatedly following these steps:

    If there is something directly in front of you, turn right 90 degrees.
    Otherwise, take a step forward.

Following the above protocol, the guard moves up several times until she reaches
an obstacle (in this case, a pile of failed suit prototypes):

....#.....
....^....#
..........
..#.......
.......#..
..........
.#........
........#.
#.........
......#...

Because there is now an obstacle in front of the guard, she turns right before
continuing straight in her new facing direction:

....#.....
........>#
..........
..#.......
.......#..
..........
.#........
........#.
#.........
......#...

Reaching another obstacle (a spool of several very long polymers), she turns
right again and continues downward:

....#.....
.........#
..........
..#.......
.......#..
..........
.#......v.
........#.
#.........
......#...

This process continues for a while, but the guard eventually leaves the mapped
area (after walking past a tank of universal solvent):

....#.....
.........#
..........
..#.......
.......#..
..........
.#........
........#.
#.........
......#v..

By predicting the guard's route, you can determine which specific positions in
the lab will be in the patrol path. Including the guard's starting position, the
positions visited by the guard before leaving the area are marked with an X:

....#.....
....XXXXX#
....X...X.
..#.X...X.
..XXXXX#X.
..X.X.X.X.
.#XXXXXXX.
.XXXXXXX#.
#XXXXXXX..
......#X..

In this example, the guard will visit 41 distinct positions on your map.

Predict the path of the guard. How many distinct positions will the guard visit
before leaving the mapped area?`

	problem_two := `
--- Part Two ---

While The Historians begin working around the guard's patrol route, you borrow
their fancy device and step outside the lab. From the safety of a supply closet,
you time travel through the last few months and record the nightly status of the
lab's guard post on the walls of the closet.

Returning after what seems like only a few seconds to The Historians, they
explain that the guard's patrol area is simply too large for them to safely
search the lab without getting caught.

Fortunately, they are pretty sure that adding a single new obstruction won't
cause a time paradox. They'd like to place the new obstruction in such a way
that the guard will get stuck in a loop, making the rest of the lab safe to
search.

To have the lowest chance of creating a time paradox, The Historians would
like to know all of the possible positions for such an obstruction. The new
obstruction can't be placed at the guard's starting position - the guard is
there right now and would notice.

In the above example, there are only 6 different positions where a new
obstruction would cause the guard to get stuck in a loop. The diagrams of these
six situations use O to mark the new obstruction, | to show a position where the
guard moves up/down, - to show a position where the guard moves left/right, and
+ to show a position where the guard moves both up/down and left/right.

Option one, put a printing press next to the guard's starting position:

....#.....
....+---+#
....|...|.
..#.|...|.
....|..#|.
....|...|.
.#.O^---+.
........#.
#.........
......#...

Option two, put a stack of failed suit prototypes in the bottom right quadrant
of the mapped area:

....#.....
....+---+#
....|...|.
..#.|...|.
..+-+-+#|.
..|.|.|.|.
.#+-^-+-+.
......O.#.
#.........
......#...

Option three, put a crate of chimney-squeeze prototype fabric next to the
standing desk in the bottom right quadrant:

....#.....
....+---+#
....|...|.
..#.|...|.
..+-+-+#|.
..|.|.|.|.
.#+-^-+-+.
.+----+O#.
#+----+...
......#...

Option four, put an alchemical retroencabulator near the bottom left corner:

....#.....
....+---+#
....|...|.
..#.|...|.
..+-+-+#|.
..|.|.|.|.
.#+-^-+-+.
..|...|.#.
#O+---+...
......#...

Option five, put the alchemical retroencabulator a bit to the right instead:

....#.....
....+---+#
....|...|.
..#.|...|.
..+-+-+#|.
..|.|.|.|.
.#+-^-+-+.
....|.|.#.
#..O+-+...
......#...

Option six, put a tank of sovereign glue right next to the tank of universal solvent:

....#.....
....+---+#
....|...|.
..#.|...|.
..+-+-+#|.
..|.|.|.|.
.#+-^-+-+.
.+----++#.
#+----++..
......#O..

It doesn't really matter what you choose to use as an obstacle so long as you
and The Historians can put it into position without the guard noticing. The
important thing is having enough options that you can find one that minimizes
time paradoxes, and in this example, there are 6 different positions you could
choose.

You need to get the guard stuck in a loop by adding a single new obstruction.
How many different positions could you choose for this obstruction?`

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

	for i := 1; i < len(lines); i++ {
		if len(lines[i]) != len(lines[i-1]) {
			fmt.Fprintf(os.Stderr, "Line %d: The input map is not a square. This may be problematic.\n", i+1)
			os.Exit(1)
		}
	}

	guard_direction := up
	guard_pos := vec2{}
	guard_found := false

	obstacles := make([]vec2, 0)
	for i := 0; i < len(lines); i++ {
		line := lines[i]
		for j := 0; j < len(line); j++ {
			pos := vec2{j, i}
			if line[j] == '#' {
				obstacles = append(obstacles, pos)
			}
			if line[j] == '^' {
				guard_pos.x = j
				guard_pos.y = i
				guard_found = true
			}
		}
	}

	if !guard_found {
		fmt.Fprintf(os.Stderr, "The input did not contain a guard '^'. The program cannot proceed.\n")
		os.Exit(1)
	}

	width := len(lines[0])
	height := len(lines)
	guard_in_room_test := func(pos vec2) bool {
		return pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height
	}

	obstructed_test := func(pos vec2) bool {
		for _, obstacle := range obstacles {
			if pos == obstacle {
				return true
			}
		}
		return false
	}

	// initial_guard_pos := guard_pos
	visited_set := make(map[vec2]direction)
	guard_in_room := true
	obstructed := false
	for {
		var next_pos vec2
		switch guard_direction {
		case up:
			next_pos = vec2{guard_pos.x, guard_pos.y - 1}
			guard_in_room = guard_in_room_test(next_pos)
			obstructed = obstructed_test(next_pos)
		case down:
			next_pos = vec2{guard_pos.x, guard_pos.y + 1}
			guard_in_room = guard_in_room_test(next_pos)
			obstructed = obstructed_test(next_pos)
		case left:
			next_pos = vec2{guard_pos.x - 1, guard_pos.y}
			guard_in_room = guard_in_room_test(next_pos)
			obstructed = obstructed_test(next_pos)
		case right:
			next_pos = vec2{guard_pos.x + 1, guard_pos.y}
			guard_in_room = guard_in_room_test(next_pos)
			obstructed = obstructed_test(next_pos)
		}
		if !guard_in_room {
			break
		}
		if obstructed {
			switch guard_direction {
			case up:
				guard_direction = right
			case down:
				guard_direction = left
			case left:
				guard_direction = up
			case right:
				guard_direction = down
			}
		} else {
			visited_set[next_pos] = guard_direction
			guard_pos = next_pos
		}
	}

	// The guard has obviously visited the place they started...
	num_visited := 1
	visited := make([]vec2, 0)
	visited_directions := make([]direction, 0)
	for p, d := range visited_set {
		num_visited++
		visited = append(visited, p)
		visited_directions = append(visited_directions, d)
	}

	// fmt.Fprintf(os.Stderr, "%s\n\n", string(printMap(width, height, obstacles, initial_guard_pos, visited, visited_directions, guard_pos, guard_direction)))

	fmt.Printf("The guard visited %d distinct places in the lab before exiting.\n", num_visited)
}

func printMap(width, height int, obstacles []vec2, initial_guard_pos vec2, visited []vec2, visited_directions []direction, guard_pos vec2, guard_direction direction) []byte {
	output := make([]byte, (width+1)*height)
	for i := 0; i < len(output); i++ {
		output[i] = '.'
	}

	for i := width; i < len(output); i += width + 1 {
		output[i] = '\n'
	}

	for i, p := range visited {
		switch visited_directions[i] {
		case up:
			output[p.x+p.y*(width+1)] = '|'
		case down:
			output[p.x+p.y*(width+1)] = '|'
		case left:
			output[p.x+p.y*(width+1)] = '-'
		case right:
			output[p.x+p.y*(width+1)] = '-'
		}
	}
	for _, p := range obstacles {
		output[p.x+p.y*(width+1)] = '#'
	}

	switch guard_direction {
	case up:
		output[guard_pos.x+guard_pos.y*(width+1)] = '^'
	case down:
		output[guard_pos.x+guard_pos.y*(width+1)] = 'V'
	case left:
		output[guard_pos.x+guard_pos.y*(width+1)] = '<'
	case right:
		output[guard_pos.x+guard_pos.y*(width+1)] = '>'
	}
	output[initial_guard_pos.x+initial_guard_pos.y*(width+1)] = 'S'
	return output
}

type direction int

const (
	unknown direction = iota
	up
	down
	left
	right
)

func rightFrom(d direction) direction {
	switch d {
	case up:
		return right
	case down:
		return left
	case left:
		return up
	case right:
		return down
	}
	return unknown
}

type vec2 struct {
	x, y int
}

func (v vec2) String() string {
	return fmt.Sprintf("(%d, %d)", v.x, v.y)
}
