#include <stdio.h>
#include "util.h"

typedef enum TokenType {
	TT_Unknown,
	TT_Symbol,
	TT_Number,
} TokenType;

typedef struct Token {
	TokenType type;
	StringView str;
} Token;

char *example_path = "input/example.txt";
char *default_path = "input/04.txt";

char *DetermineInputPath(int argc, char **argv) {
	char *input_path = default_path;
	if (argc > 1) {
		if (strncmp(argv[1], "-e", 2) == 0 ||
		    strncmp(argv[1], "--example", 9) == 0) {
			input_path = example_path;
		}
		if (strncmp(argv[1], "-f", 2) == 0 ||
		    strncmp(argv[1], "--file", 6) == 0) {
			if (argc == 2) {
				fprintf(stderr, "Option -f <file> requires an argument, none provided\n");
				exit(1);
			}
			input_path = argv[2];
		}
	}
	return input_path;
}

typedef struct Game {
	int id;
	DynArray winners;
	DynArray chosen;
} Game;

int main(int argc, char** argv) {
	const char *input_path = DetermineInputPath(argc, argv);
	StringBuffer file;
	if (!ReadEntireFile(input_path, &file)) { return 1; }

	StringView content = {
		.data  = file.data,
		.count = file.count,
	};

	DynArray lines = StringView_Lines(content);
	DynArray total_games = {0};
	DynArray original_games = {0};
	StringView *lines_arr = lines.data;

	for (size_t i = 0; i < lines.count; ++i) {
		Game game = {0};
		StringView line = lines_arr[i];
		StringView_SplitOn(&line, ':');
		game.id = i + 1;
		StringView winners = StringView_SplitOn(&line, '|');
		StringView chosen = line;

		DynArray winning_numbers = StringView_TakeNumbers(winners);
		StringView *winning_numbers_arr = winning_numbers.data;
		for (size_t w = 0; w < winning_numbers.count; ++w) {
			int val = strtol(winning_numbers_arr[w].data, 0x0, 10);
			DynArray_Add(&game.winners, val);
		}
		DynArray chosen_numbers = StringView_TakeNumbers(chosen);
		StringView *chosen_numbers_arr = chosen_numbers.data;
		for (size_t c = 0; c < chosen_numbers.count; ++c) {
			int val = strtol(chosen_numbers_arr[c].data, 0x0, 10);
			DynArray_Add(&game.chosen, val);
		}
		DynArray_Add(&total_games, game);
		DynArray_Add(&original_games, game);
	}

	Game *originals = original_games.data;
	for (size_t i = 0; i < total_games.count; ++i) {
		Game *cards_arr = total_games.data;

		Game game = cards_arr[i];

		int *winners = game.winners.data;
		int *chosen = game.chosen.data;
		int wins_this_game = 0;
		for (size_t j = 0; j < game.chosen.count; ++j) {
			int num = chosen[j];
			for (size_t k = 0; k < game.winners.count; ++k) {
				int winner = winners[k];
				if (num == winner) {
					DynArray_Add(&total_games, originals[game.id+wins_this_game]);
					wins_this_game += 1;
					break;
				}
			}
		}
	}

	fprintf(stdout, "Total checked cards is %zu\n", total_games.count);
}
