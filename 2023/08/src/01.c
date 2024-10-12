#include <stdio.h>
#include "util.h"

const char *default_path = "input/08.txt";
const char *example_path = "input/example.txt";

#ifdef SILENT
#define fprintf(...) do { } while (0)
#endif

const char *determine_input_path(int argc, char **argv) {
	const char *input_path = default_path;
	if (argc > 1) {
		if (strncmp(argv[1], "-e", 2) == 0 ||
		    strncmp(argv[1], "--example", 9) == 0) {
			input_path = example_path;
		} else if (strncmp(argv[1], "-f", 2) == 0 ||
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

typedef struct {
	StringView key;
	StringView left;
	StringView right;
	int k, l, r;
} Pair;

DynArray pair_parse_lines(DynArray lines_da) {
	DynArray result = {0};
	StringView *lines = dyn_array_as_type(lines_da, StringView);
	for (size_t i = 2; i < lines_da.count; i += 1) {
		StringView line = lines[i];
		Pair pair = {0};
		pair.k = i-2;
		pair.key = string_view_split_on(&line, ' ');
		string_view_split_on(&line, '('); // just some symbols we don't care about
		pair.left = string_view_split_on(&line, ',');
		line = string_view_trim_left(line);
		pair.right = string_view_split_on(&line, ')');
		dyn_array_add(&result, pair);
	}
	return result;
}

int pair_find(DynArray pairs_da, StringView key) {
	int result = -1;
	Pair *pairs = dyn_array_as_type(pairs_da, Pair);
	for (size_t i = 0; i < pairs_da.count; i += 1){
		Pair p = pairs[i];
		if (string_view_equal(key, p.key)) {
			result = i;
			break;
		}
	}
	return result;
}

void pair_fill_keys(DynArray *pairs_da) {
	Pair *pairs = dyn_array_as_type(*pairs_da, Pair);
	for (size_t i = 0; i < pairs_da->count; i += 1){
		pairs[i].l = pair_find(*pairs_da, pairs[i].left);
		pairs[i].r = pair_find(*pairs_da, pairs[i].right);
		if (pairs[i].l < 0 || pairs[i].r < 0) {
			printf("Found missing keys somehow?\nKeys are ("SV_FMT", "SV_FMT")\n", SV_ARG(pairs[i].left), SV_ARG(pairs[i].right));
		}
	}
}

size_t pair_find_ancestors(DynArray pairs_da, int k, int lr[]) {
	lr[0] = -1; lr[1] = -1;
	Pair *pairs = dyn_array_as_type(pairs_da, Pair);
	size_t num_found = 0;
	for (size_t i = 0; i < pairs_da.count; i += 1) {
		if (pairs[i].k == k) { continue; }
		if (pairs[i].l == k || pairs[i].r == k) {
			lr[0] = i;
			break;
		}
	}
	if (lr[0] == -1) {
		printf("There were no keys pointing to "SV_FMT". It's a dead end.\n", SV_ARG(pairs[k].key));
		return num_found = 0;
	}
	for (size_t i = 0; i < pairs_da.count; i += 1) {
		if (i == (size_t)lr[0]) { continue; }
		if (pairs[i].k == k) { continue; }
		if (pairs[i].l == k || pairs[i].r == k) {
			lr[1] = i;
			break;
		}
	}
	if (lr[1] == -1) {
		printf("There was only one path to "SV_FMT".\n", SV_ARG(pairs[k].key));
		return num_found = 1;
	}
	return num_found = 2;
}

int main(int argc, char **argv) {
	const char *input_path = determine_input_path(argc, argv);
	StringBuffer file;
	if (!read_entire_file(input_path, &file)) { return 1; }

	StringView content = {
		.data = file.data,
		.count = file.count,
	};

	DynArray lines_da = string_view_lines(content);

	DynArray pairs_da = pair_parse_lines(lines_da);

	pair_fill_keys(&pairs_da);

	Pair *pairs = dyn_array_as_type(pairs_da, Pair);
	for (size_t i = 0; i < pairs_da.count; i += 1) {
		Pair p = pairs[i];
		printf(SV_FMT" = ("SV_FMT","SV_FMT")\n", SV_ARG(p.key), SV_ARG(p.left), SV_ARG(p.right));
	}

	printf("There are %ld pairs.\n", pairs_da.count);

	int start = pairs[0].k;
	int end = pairs[pairs_da.count-1].k;

	int k1 = end, k2 = end;
	int checking[2] = {0};
	int path_length = 0;

	bool found_path = false;
	while (!found_path) {
		if (k1 == start) {
			found_path = true;
			continue;
		}
		size_t new_ancestors = pair_find_ancestors(pairs_da, k1, checking);
		if (new_ancestors == 0) { return 1; }
		else if (new_ancestors == 1) {
			k1 = checking[0];
			continue;
		}
		else {
			path_length += 1;
			k1 = checking[0];
			int checkingg[2] = {0};
			size_t news = pair_find_ancestors(pairs_da, k2, checkingg);
			if (news != new_ancestors) {
				printf("Found more ancestors for k1 than for k2.\n");
				return 1;
			}
			if ((checkingg[0] != checking[0] && checkingg[0] != checking[1]) ||
				(checkingg[1] != checking[0] && checkingg[1] != checking[1])) {
				printf("The ancestors for k2 were different than those for k1!.\n");
				return 1;
			}
			k2 = checking[1];
			if (k1 == start || k2 == start) {
				printf("Path found hooray.\n""Path length is %d.\n", path_length);
				found_path = true;
			}
		}
	}

	return 0;
}
