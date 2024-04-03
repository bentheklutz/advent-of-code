#include <stdio.h>
#include "util.h"

const char *default_path = "input/05.txt";
const char *example_path = "input/example.txt";

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

typedef enum ParseState {
	PS_Unknown,
	PS_Init,
	PS_ReadingRanges,
} ParseState;

typedef struct Mapping {
	long source_start;
	long dest_start;
	long range;
} Mapping;

long find_value(DynArray mappings_da, long input) {
	long result = -1;
	Mapping *mappings = dyn_array_as_type(mappings_da, Mapping);
	for (size_t i = 0; i < mappings_da.count; i += 1) {
		Mapping m = mappings[i];
		if (m.source_start <= input && input < m.source_start + m.range) {
			return result = m.dest_start + (input - m.source_start);
		}
	}
	return result = input;
}

int main(int argc, char **argv) {
	const char *input_path = determine_input_path(argc, argv);
	StringBuffer file;
	if (!read_entire_file(input_path, &file)) { return 1; }

	StringView content = {
		.data = file.data,
		.count = file.count,
	};

	ParseState state = PS_Init;

	DynArray lines = string_view_lines(content);
	StringView *lines_arr = dyn_array_as_type(lines, StringView);

	StringView seed_line = lines_arr[0];
	string_view_split_on(&seed_line, ':');
	DynArray seeds = string_view_parse_numbers(seed_line);
	long *seeds_arr = dyn_array_as_type(seeds, long);

	for (size_t i = 0; i < seeds.count; ++i) {
		fprintf(stderr, "[%zu]: %ld\n", i, seeds_arr[i]);
	}

	DynArray seed_to_soil_da = {0};

	DynArray soil_to_fertilizer_da = {0};
	DynArray fertilizer_to_water_da = {0};
	DynArray water_to_light_da = {0};
	DynArray light_to_temperature_da = {0};
	DynArray temperature_to_humidity_da = {0};
	DynArray humidity_to_location_da = {0};

	DynArray *current;

	for (size_t i = 1; i < lines.count; ++i) {
		switch (state) {
		case PS_Init: {
			if (lines_arr[i].count != 0) {
				// Start of a section
				if (0 == strncmp("seed", lines_arr[i].data, 4)) {
					current = &seed_to_soil_da;
					fprintf(stderr, "Parsing seeds\n");
				} else if (0 == strncmp("soil", lines_arr[i].data, 4)) {
					current = &soil_to_fertilizer_da;
					fprintf(stderr, "Parsing soil\n");
				} else if (0 == strncmp("fertilizer", lines_arr[i].data, 10)) {
					current = &fertilizer_to_water_da;
					fprintf(stderr, "Parsing fertilizer\n");
				} else if (0 == strncmp("water", lines_arr[i].data, 5)) {
					current = &water_to_light_da;
					fprintf(stderr, "Parsing water\n");
				} else if (0 == strncmp("light", lines_arr[i].data, 5)) {
					current = &light_to_temperature_da;
					fprintf(stderr, "Parsing light\n");
				} else if (0 == strncmp("temperature", lines_arr[i].data, 11)) {
					current = &temperature_to_humidity_da;
					fprintf(stderr, "Parsing temperature\n");
				} else if (0 == strncmp("humidity", lines_arr[i].data, 8)) {
					current = &humidity_to_location_da;
					fprintf(stderr, "Parsing humidity\n");
				}
				state = PS_ReadingRanges;
			}
		} break;
		case PS_ReadingRanges: {
			if (lines_arr[i].count == 0) {
				fprintf(stderr, "EMPTY LINE\n");
				state = PS_Init;
			} else {
				fprintf(stderr, SV_FMT"\n", SV_ARG(lines_arr[i]));
				DynArray range = string_view_parse_numbers(lines_arr[i]);
				if (range.count != 3) {
					fprintf(stderr, "Range didn't have three values: "SV_FMT"\n", SV_ARG(lines_arr[i]));
				}
				long *numbers = dyn_array_as_type(range, long);
				Mapping m = {0};
				m.dest_start = numbers[0];
				m.source_start = numbers[1];
				m.range = numbers[2];

				free(numbers);

				dyn_array_add(current, m);
			}
		} break;
		case PS_Unknown: {
			fprintf(stderr, "We're in a bad state, fuck all this\n");
			exit(1);
		} break;
		}
	}

	long min_location = 0x0FFFFFFFFFFFFFFF;
	for (size_t i = 0; i < seeds.count; i += 1) {
		long seed = seeds_arr[i];
		long soil = find_value(seed_to_soil_da, seed);
		if (soil < 0) {
			goto fucked;
		}
		long fertilizer = find_value(soil_to_fertilizer_da, soil);
		if (fertilizer < 0) {
			goto fucked;
		}
		long water = find_value(fertilizer_to_water_da, fertilizer);
		if (water < 0) {
			goto fucked;
		}
		long light = find_value(water_to_light_da, water);
		if (light < 0) {
			goto fucked;
		}
		long temperature = find_value(light_to_temperature_da, light);
		if (temperature < 0) {
			goto fucked;
		}
		long humidity = find_value(temperature_to_humidity_da, temperature);
		if (humidity < 0) {
			goto fucked;
		}
		long location = find_value(humidity_to_location_da, humidity);
		if (location < 0) {
			goto fucked;
		}
		if (location < min_location) {
			min_location = location;
		}
		fprintf(stdout, "Seed %ld, soil %ld, fertilizer %ld, water %ld, light %ld, temperature %ld, humidity %ld, location %ld.\n", seed, soil, fertilizer, water, light, temperature, humidity, location);
	}
	fprintf(stdout, "Minimum location value is %ld.\n", min_location);

	return 0;

fucked:
	fprintf(stderr, "This shit is fucked, yo.\n");
	return 1;
}
