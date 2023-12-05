#include <stdio.h>
#include "util.h"

const char *default_path = "input/05.txt";
const char *example_path = "input/example.txt";

const char *DetermineInputPath(int argc, char **argv) {
	const char *input_path = default_path;
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

int main(int argc, char **argv) {
	const char *input_path = DetermineInputPath(argc, argv);
	StringBuffer file;
	if (!ReadEntireFile(input_path, &file)) { return 1; }

	StringView content = {
		.data = file.data,
		.count = file.count,
	};

	DynArray lines = StringView_Lines(content);
	StringView *lines_arr = lines.data;

	for (size_t i = 0; i < lines.count; ++i) {
		fprintf(stderr, SV_FMT"\n", SV_ARG(lines_arr[i]));
	}

	return 0;
}
