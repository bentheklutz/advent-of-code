#include <stdio.h>
#include <stdbool.h>
#include "util.h"

typedef struct Round {
	int red;
	int blue;
	int green;
} Round;

typedef struct Game {
	int max_red;
	int max_blue;
	int max_green;
} Game;

int main() {
	StringBuffer buf = {0};
	if (!ReadEntireFile("input/02.txt", &buf)) { return 1; }
	StringView contents = {
		.data = buf.data,
		.count = buf.count,
	};

	int result = 0;

	for (int i = 0; contents.count > 0; ++i) {
		StringView line = StringView_TakeLine(&contents);
		// Screw parsing the gameID, it's just 1 + current line number
		Game game = {0};
		StringView_SplitOn(&line, ':');

		StringView round = StringView_TrimLeft(StringView_SplitOn(&line, ';'));
		while (round.count > 0) {
			Round r = {0};
			StringView selection = StringView_TrimLeft(StringView_SplitOn(&round, ','));
			while (selection.count > 0) {
				StringView num = StringView_SplitOn(&selection, ' ');
				StringView color = StringView_TrimLeft(selection);
				char num_nt[3];
				memset(num_nt, 0, sizeof(num_nt));
				if (num.count > 2) {
					fprintf(stderr, "line %d: TOO MANY DIGITS WTF: %zu\n", i, num.count);
					exit(1);
				}
				memcpy(num_nt, num.data, num.count);
				int num_cubes = atoi(num_nt);
				if (color.count == 3) { // red
					r.red = num_cubes;
				} else if (color.count == 4) { // blue
					r.blue = num_cubes;
				} else if (color.count == 5) { // green
					r.green = num_cubes;
				} else {
					fprintf(stderr, "UNKNOWN COLOR %.*s\n", (int)color.count, color.data);
					exit(1);
				}
				selection = StringView_TrimLeft(StringView_SplitOn(&round, ','));
			}

			if (r.red > game.max_red) {
				game.max_red = r.red;
			}
			if (r.blue > game.max_blue) {
				game.max_blue = r.blue;
			}
			if (r.green > game.max_green) {
				game.max_green = r.green;
			}

			round = StringView_TrimLeft(StringView_SplitOn(&line, ';'));
		}
		int game_power = game.max_red * game.max_blue * game.max_green;
		result += game_power;
	}
	printf("%d\n", result);
	return 0;
}
