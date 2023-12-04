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


int main() {
	StringBuffer file;
	if (!ReadEntireFile("input/03.txt", &file)) { return 1; }

	StringView content = {
		.data  = file.data,
		.count = file.count,
	};

	DynArray lines = StringView_Lines(content);
	StringView *lines_arr = lines.data;

	for (size_t i = 0; i < lines.count; ++i) {
		fprintf(stderr, "%03zu: "SV_FMT"\n", i, SV_ARG(lines_arr[i]));
		DynArray numbers = StringView_TakeNumbers(lines_arr[i]);
		StringView *numbers_arr = numbers.data;
		DynArray symbols = StringView_TakeSymbols(lines_arr[i]);
		StringView *symbols_arr = symbols.data;
		if (numbers.count > 0) {
			fprintf(stderr, "Numbers are:\n");
			for (size_t j = 0; j < numbers.count; ++j) {
				fprintf(stderr, "  " SV_FMT "@%zu\n", SV_ARG(numbers_arr[j]), numbers_arr[j].data - lines_arr[i].data);
			}
		} else {
			fprintf(stderr, "In terms of numbers, we have no numbers\n");
		}
		if (symbols.count > 0) {
			fprintf(stderr, "Symbols are:\n");
			for (size_t j = 0; j < symbols.count; ++j) {
				fprintf(stderr, "  " SV_FMT "@%zu\n", SV_ARG(symbols_arr[j]), symbols_arr[j].data - lines_arr[i].data);
			}
		} else {
			fprintf(stderr, "In terms of symbols, we have no symbols\n");
		}
	}
}
