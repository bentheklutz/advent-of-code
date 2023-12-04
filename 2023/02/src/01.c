#include <stdio.h>
#include <stdbool.h>
#include "util.h"

#define MAX_RED_CUBES   12
#define MAX_GREEN_CUBES 13
#define MAX_BLUE_CUBES  14

typedef struct Round {
	int red;
	int blue;
	int green;
} Round;

int main() {
	StringBuffer buf = {0};
	if (!ReadEntireFile("input/02.txt", &buf)) { return 1; }
	StringView contents = {
		.data = buf.data,
		.count = buf.count,
	};

	int result = 0;

	for (int i = 0; contents.count > 0; ++i) {
		if (contents.count == 0) { break; }

		StringView line = StringView_TakeLine(&contents);
		// Screw parsing the gameID, it's just 1 + current line number
		int gameid = i + 1;
		bool game_is_possible = true;
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

			if (r.red > MAX_RED_CUBES || r.blue > MAX_BLUE_CUBES || r.green > MAX_GREEN_CUBES) {
				game_is_possible = false;
				break;
			}
			round = StringView_TrimLeft(StringView_SplitOn(&line, ';'));
		}
		if (game_is_possible) {
			result += gameid;
		}
	}
	printf("%d\n", result);
	return 0;
}
