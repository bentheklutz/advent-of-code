package main

import (
	"fmt"
	"os"
	"strconv"
	"strings"
)

func main() {

	problem_one := `
--- Day 2: Cube Conundrum ---

You're launched high into the atmosphere! The apex of your trajectory just
barely reaches the surface of a large island floating in the sky. You gently
land in a fluffy pile of leaves. It's quite cold, but you don't see much snow.
An Elf runs over to greet you.

The Elf explains that you've arrived at Snow Island and apologizes for the lack
of snow. He'll be happy to explain the situation, but it's a bit of a walk, so
you have some time. They don't get many visitors up here; would you like to play
a game in the meantime?

As you walk, the Elf shows you a small bag and some cubes which are either red,
green, or blue. Each time you play this game, he will hide a secret number of
cubes of each color in the bag, and your goal is to figure out information about
the number of cubes.

To get information, once a bag has been loaded with cubes, the Elf will reach
into the bag, grab a handful of random cubes, show them to you, and then put
them back in the bag. He'll do this a few times per game.

You play several games and record the information from each game (your puzzle
input). Each game is listed with its ID number (like the 11 in Game 11: ...)
followed by a semicolon-separated list of subsets of cubes that were revealed
from the bag (like 3 red, 5 green, 4 blue).

For example, the record of a few games might look like this:

Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green

In game 1, three sets of cubes are revealed from the bag (and then put back
again). The first set is 3 blue cubes and 4 red cubes; the second set is 1 red
cube, 2 green cubes, and 6 blue cubes; the third set is only 2 green cubes.

The Elf would first like to know which games would have been possible if the bag
contained only 12 red cubes, 13 green cubes, and 14 blue cubes?

In the example above, games 1, 2, and 5 would have been possible if the bag had
been loaded with that configuration. However, game 3 would have been impossible
because at one point the Elf showed you 20 red cubes at once; similarly, game
4 would also have been impossible because the Elf showed you 15 blue cubes at
once. If you add up the IDs of the games that would have been possible, you
get 8.

Determine which games would have been possible if the bag had been loaded with
only 12 red cubes, 13 green cubes, and 14 blue cubes. What is the sum of the IDs
of those games?
`

	problem_two := `
--- Part Two ---

The Elf says they've stopped producing snow because they aren't getting any
water! He isn't sure why the water stopped; however, he can show you how to get
to the water source to check it out for yourself. It's just up ahead!

As you continue your walk, the Elf poses a second question: in each game you
played, what is the fewest number of cubes of each color that could have been in
the bag to make the game possible?

Again consider the example games from earlier:

Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green

    In game 1, the game could have been played with as few as 4 red, 2 green, and 6 blue cubes. If any color had even one fewer cube, the game would have been impossible.
    Game 2 could have been played with a minimum of 1 red, 3 green, and 4 blue cubes.
    Game 3 must have been played with at least 20 red, 13 green, and 6 blue cubes.
    Game 4 required at least 14 red, 3 green, and 15 blue cubes.
    Game 5 needed no fewer than 6 red, 3 green, and 2 blue cubes in the bag.

The power of a set of cubes is equal to the numbers of red, green, and blue
cubes multiplied together. The power of the minimum set of cubes in game 1 is
48. In games 2-5 it was 12, 1560, 630, and 36, respectively. Adding up these
five powers produces the sum 2286.

For each game, find the minimum set of cubes that must have been present. What
is the sum of the power of these sets?`

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
			fmt.Printf("%s\n%s\n", problem_one, problem_two)
			return
		}
	}

	if !have_file_name {
		fmt.Fprintf(os.Stderr, "%s\n%s\n\n", problem_one, problem_two)
		fmt.Fprintf(os.Stderr, "Usage: %s [-p] [-i <file_path>]\n", os.Args[0])
		os.Exit(1)
	}

	file, err := os.Open(file_name)
	if err != nil {
		fmt.Fprintf(os.Stderr, "The program failed to open the input file (%s).\nThe error is %s.\n", file_name, err.Error())
		os.Exit(1)
	}

	file_info, err := file.Stat()
	if err != nil {
		fmt.Fprintf(os.Stderr, "The program failed to get the size of the input file (%s).\nThe error is %s.\n", file_name, err.Error())
		os.Exit(1)
	}
	contents := make([]byte, file_info.Size())
	bytes_read, err := file.Read(contents)
	if err != nil {
		fmt.Fprintf(os.Stderr, "The program failed to read the input file (%s).\nThe error is %s.\n", file_name, err.Error())
		os.Exit(1)
	}
	if bytes_read != len(contents) {
		fmt.Fprintf(os.Stderr, "The program only read %d of %d total bytes from the input file (%s).\n", bytes_read, len(contents), file_name)
		os.Exit(1)
	}

	lines := strings.Split(string(contents), "\n")
	lines = lines[:len(lines)-1]

	max_red := 12
	max_green := 13
	max_blue := 14

	sum := 0

	for i := 0; i < len(lines); i++ {
		line := lines[i]
		gid_game := strings.Split(line, ":")
		game_id, err := strconv.Atoi(strings.Split(gid_game[0], " ")[1])
		if err != nil {
			fmt.Fprintf(os.Stderr, "Could not parse the game ID %s.\n", gid_game[0])
			os.Exit(1)
		}
		game := strings.TrimLeft(gid_game[1], " ")
		rounds := strings.Split(game, ";")
		game_valid := true
		for j := 0; j < len(rounds); j++ {
			pulls := strings.Split(rounds[j], ",")
			for k := 0; k < len(pulls); k++ {
				number_and_color := strings.Split(strings.TrimLeft(pulls[k], " "), " ")
				num_marbles, err := strconv.Atoi(number_and_color[0])
				if err != nil {
					fmt.Fprintf(os.Stderr, "Line %d is malformed. %s is not a number.\n", i+1, number_and_color[0])
					os.Exit(1)
				}
				color := number_and_color[1]
				switch color {
				case "blue":
					if num_marbles > max_blue {
						game_valid = false
					}
				case "red":
					if num_marbles > max_red {
						game_valid = false
					}
				case "green":
					if num_marbles > max_green {
						game_valid = false
					}
				default:
					fmt.Fprintf(os.Stderr, "'%s' is not a valid color.\n", color)
				}
			}
		}
		// fmt.Fprintf(os.Stderr, "Game %d requires %d red, %d blue, and %d green marbles.\n", game_id, total_red, total_blue, total_green)
		if game_valid {
			fmt.Printf("Game %d is valid.\n", game_id)
			sum += game_id
		}
	}

	fmt.Printf("The sum of all valid game IDs is %d.\n", sum)

	power_sum := 0

	for i := 0; i < len(lines); i++ {
		line := lines[i]
		gid_game := strings.Split(line, ":")
		if err != nil {
			fmt.Fprintf(os.Stderr, "Could not parse the game ID %s.\n", gid_game[0])
			os.Exit(1)
		}
		game := strings.TrimLeft(gid_game[1], " ")
		rounds := strings.Split(game, ";")
		most_green := 0
		most_red := 0
		most_blue := 0
		for j := 0; j < len(rounds); j++ {
			pulls := strings.Split(rounds[j], ",")
			for k := 0; k < len(pulls); k++ {
				number_and_color := strings.Split(strings.TrimLeft(pulls[k], " "), " ")
				num_marbles, err := strconv.Atoi(number_and_color[0])
				if err != nil {
					fmt.Fprintf(os.Stderr, "Line %d is malformed. %s is not a number.\n", i+1, number_and_color[0])
					os.Exit(1)
				}
				color := number_and_color[1]
				switch color {
				case "blue":
					if num_marbles > most_blue {
						most_blue = num_marbles
					}
				case "red":
					if num_marbles > most_red {
						most_red = num_marbles
					}
				case "green":
					if num_marbles > most_green {
						most_green = num_marbles
					}
				default:
					fmt.Fprintf(os.Stderr, "'%s' is not a valid color.\n", color)
				}
			}
		}
		// fmt.Fprintf(os.Stderr, "Game %d requires %d red, %d blue, and %d green marbles.\n", game_id, total_red, total_blue, total_green)
		power_score := most_green * most_blue * most_red
		power_sum += power_score
	}
	fmt.Printf("The sum of all power scores is %d.\n", power_sum)
}
