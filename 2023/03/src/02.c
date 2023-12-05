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
char *default_path = "input/03.txt";

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

	DynArray lines = StringView_Lines(content);
	StringView *lines_arr = lines.data;

	DynArray *_symbols = malloc(lines.count*sizeof(DynArray));
	DynArray *_numbers = malloc(lines.count*sizeof(DynArray));

	for (size_t i = 0; i < lines.count; ++i) {
		_symbols[i] = StringView_TakeSymbols(lines_arr[i]);
		_numbers[i] = StringView_TakeNumbers(lines_arr[i]);
	}


	int result = 0;
	// NOTE: I added an extra line of no symbols and no numbers at the beginning
	// and end so that I don't need a special case for the first and last lines.
	for (size_t i = 1; i < lines.count-1; ++i) {
		DynArray symbols = _symbols[i];
		StringView line_last = lines_arr[i-1];
		StringView line_curr = lines_arr[i];
		StringView line_next = lines_arr[i+1];

		StringView *symbols_arr = symbols.data;
		DynArray num_last = _numbers[i-1];
		DynArray num_curr = _numbers[i];
		DynArray num_next = _numbers[i+1];
		StringView *numbers_last = num_last.data;
		StringView *numbers_curr = num_curr.data;
		StringView *numbers_next = num_next.data;

		fprintf(stderr, "Processing line %zu\n", i+1);
		fprintf(stderr, SV_FMT"\n"SV_FMT"\n"SV_FMT"\n",
		        SV_ARG(line_last), SV_ARG(line_curr), SV_ARG(line_next));


		for (size_t j = 0; j < symbols.count; ++j) {
			DynArray adjacent = {0};
			StringView symbol = symbols_arr[j];
			size_t sym_line_pos = symbol.data - line_curr.data;

			fprintf(stderr, "\t\tChecking "SV_FMT" : %zu\n", SV_ARG(symbol), sym_line_pos);

			if (symbol.data[0] != '*') { continue; }

			for (size_t k = 0; k < num_last.count; ++k) {
				StringView number = numbers_last[k];
				size_t num_line_pos = number.data - line_last.data;
				fprintf(stderr, "\t\tChecking "SV_FMT" : %zu - %zu\n", SV_ARG(number), num_line_pos, num_line_pos+number.count-1);
				bool in_left_region = sym_line_pos >= (num_line_pos ? num_line_pos-1 : num_line_pos);
				bool in_right_region = sym_line_pos <= num_line_pos+number.count;
				if (in_left_region) {
					if (in_right_region) {
						DynArray_Add(&adjacent, number);
						fprintf(stderr,
						        "\t\t\tAdjacent from previous line: "SV_FMT" : %zu / "SV_FMT" : %zu - %zu\n",
						        SV_ARG(symbol), sym_line_pos, SV_ARG(number), num_line_pos, num_line_pos+number.count-1);
					} else {
						fprintf(stderr, "\t\t\tSymbol was in the left region, but not the right region.\n");
					}
				} else {
					if (in_right_region) {
						fprintf(stderr, "\t\t\tSymbol was in the right region, but not the left region.\n");
					} else {
						fprintf(stderr, "\t\t\tSymbol was in neither region.\n");
					}
				}
			}
			for (size_t k = 0; k < num_curr.count; ++k) {
				StringView number = numbers_curr[k];
				size_t num_line_pos = number.data - line_curr.data;
				fprintf(stderr, "\t\tChecking "SV_FMT" : %zu - %zu\n", SV_ARG(number), num_line_pos, num_line_pos+number.count-1);
				bool in_left_region = sym_line_pos >= (num_line_pos ? num_line_pos-1 : num_line_pos);
				bool in_right_region = sym_line_pos <= num_line_pos+number.count;
				if (in_left_region) {
					if (in_right_region) {
						DynArray_Add(&adjacent, number);
						fprintf(stderr,
						        "\t\t\tAdjacent from current line: "SV_FMT" : %zu / "SV_FMT" : %zu - %zu\n",
						        SV_ARG(symbol), sym_line_pos, SV_ARG(number), num_line_pos, num_line_pos+number.count-1);
					} else {
						fprintf(stderr, "\t\t\tSymbol was in the left region, but not the right region.\n");
					}
				} else {
					if (in_right_region) {
						fprintf(stderr, "\t\t\tSymbol was in the right region, but not the left region.\n");
					} else {
						fprintf(stderr, "\t\t\tSymbol was in neither region.\n");
					}
				}
			}
			for (size_t k = 0; k < num_next.count; ++k) {
				StringView number = numbers_next[k];
				size_t num_line_pos = number.data - line_next.data;
				fprintf(stderr, "\t\tChecking "SV_FMT" : %zu - %zu\n", SV_ARG(number), num_line_pos, num_line_pos+number.count-1);
				bool in_left_region = sym_line_pos >= (num_line_pos ? num_line_pos-1 : num_line_pos);
				bool in_right_region = sym_line_pos <= num_line_pos+number.count;
				if (in_left_region) {
					if (in_right_region) {
						DynArray_Add(&adjacent, number);
						fprintf(stderr,
						        "\t\t\tAdjacent from next line: "SV_FMT" : %zu / "SV_FMT" : %zu - %zu\n",
						        SV_ARG(symbol), sym_line_pos, SV_ARG(number), num_line_pos, num_line_pos+number.count-1);
					} else {
						fprintf(stderr, "\t\t\tSymbol was in the left region, but not the right region.\n");
					}
				} else {
					if (in_right_region) {
						fprintf(stderr, "\t\t\tSymbol was in the right region, but not the left region.\n");
					} else {
						fprintf(stderr, "\t\t\tSymbol was in neither region.\n");
					}
				}
			}

			if (adjacent.count == 2) {
				StringView *gears = adjacent.data;
				long gear1 = strtol(gears[0].data, 0x0, 10);
				long gear2 = strtol(gears[1].data, 0x0, 10);

				result += (gear1*gear2);
			}
			adjacent.count = 0;
		}
	}
	fprintf(stdout, "Sum of part numbers is %d\n", result);
}
