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

int main(int argc, char** argv) {
	const char *input_path = DetermineInputPath(argc, argv);
	StringBuffer file;
	if (!ReadEntireFile(input_path, &file)) { return 1; }

	StringView content = {
		.data  = file.data,
		.count = file.count,
	};

	int result = 0;

	DynArray lines = StringView_Lines(content);
	StringView *lines_arr = lines.data;

	for (size_t i = 0; i < lines.count; ++i) {
		StringView line = lines_arr[i];
		StringView card_id = StringView_SplitOn(&line, ':');
		StringView winners = StringView_SplitOn(&line, '|');
		DynArray winning_numbers = StringView_TakeNumbers(winners);
		DynArray chosen_numbers = StringView_TakeNumbers(line);
		fprintf(stderr, SV_FMT": "SV_FMT" | "SV_FMT"\n", SV_ARG(card_id), SV_ARG(winners), SV_ARG(line));

		StringView *winning = winning_numbers.data;
		StringView *chosen = chosen_numbers.data;

		int result_for_game = 0;
		for (size_t j = 0; j < chosen_numbers.count; ++j) {
			StringView num = chosen[j];
			for (size_t k = 0; k < winning_numbers.count; ++k) {
				StringView winner = winning[k];
				if (num.count == winner.count && strncmp(num.data, winner.data, num.count) == 0) {
					result_for_game = result_for_game ? result_for_game*2 : 1;
					break;
				}
			}
		}
		fprintf(stderr, "Result for game is %d\n", result_for_game);
		result += result_for_game;
	}

	fprintf(stdout, "Sum of points for all cards is %d\n", result);
}
