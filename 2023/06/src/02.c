#include <stdio.h>
#include "util.h"

const char *default_path = "input/06.txt";
const char *example_path = "input/example.txt";

#ifdef SILENT
#define fprintf(fd, fmt, ...) do { } while (0)
#endif

const char *determine_input_path(int argc, char **argv) {
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

long calculate_maximum_distance(long race_time) {
	long max_distance = 0;

	for (long hold_time = 0; hold_time < race_time; hold_time += 1) {
		// d = v * t
		// v = hold_time * 1 mm/ms
		// t = race_time - hold_time
		long distance = (race_time - hold_time)*hold_time;
		if (distance > max_distance) { max_distance = distance; }
	}

	return max_distance;
}

int main(int argc, char **argv) {
	const char *input_path = determine_input_path(argc, argv);
	StringBuffer file;
	if (!read_entire_file(input_path, &file)) { return 1; }

	StringView content = {
		.data = file.data,
		.count = file.count,
	};

	DynArray lines = string_view_lines(content);
	StringView *lines_arr = dyn_array_as_type(lines, StringView);

	StringView time_line = lines_arr[0];
	StringView dist_line = lines_arr[1];

	string_view_split_on(&time_line, ':');
	DynArray times_da = string_view_parse_numbers_with_spaces(time_line);

	string_view_split_on(&dist_line, ':');
	DynArray dists_da = string_view_parse_numbers_with_spaces(dist_line);

	if (times_da.count != dists_da.count) {
		fprintf(stderr, "Different number of times (%ld) and distances (%ld). This input makes no fuckin sense.\n", times_da.count, dists_da.count);
		return 1;
	}

	if (times_da.count != 1) {
		fprintf(stderr, "We failed to parse the lines correctly, it should be one large number but instead it's %ld.\n", times_da.count);
		return 1;
	}

	size_t races = times_da.count;
	long *times = dyn_array_as_type(times_da, long);
	long *dists = dyn_array_as_type(dists_da, long);

	long answer = 1;
	for (size_t race = 0; race < races; race += 1) {
		long race_time = times[race];
		long race_record = dists[race];
		int ways_to_win = 0;
		for (long hold_time = 0; hold_time < race_time; hold_time += 1) {
			long distance = hold_time*(race_time-hold_time);
			if (distance > race_record) {
				ways_to_win += 1;
			}
		}
		answer *= ways_to_win;
	}

	printf("The answer is %ld.\n", answer);

	return 0;
}
