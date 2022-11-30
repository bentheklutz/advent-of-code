#include "helpers.c"

int main(int argc, char **argv) {
	u8 *filename = "input.txt";
	if (argc > 1) {
		filename = argv[1];
	}
	
	file input_file = OpenFileForReading(filename);
	if (!input_file.data) {
		printf("failed to open input file (error: %llX)\n", input_file.length);
		return 1;
	}

	int num_increases = 0;

	u8 *next;
	
	string first = GetNextLine(input_file.data, MAX_LINE_LENGTH);
	next = EatWhiteSpace(input_file.data+first.length);
	string second = GetNextLine(next, MAX_LINE_LENGTH);
	next = EatWhiteSpace(next+second.length);
	string third = GetNextLine(next, MAX_LINE_LENGTH);
	next = EatWhiteSpace(next+third.length);

	s32 first_number = StringToSignedInteger(first);
	s32 second_number = StringToSignedInteger(second);
	s32 third_number = StringToSignedInteger(second);

	s32 last_window = first_number + second_number + third_number;
	s32 current_window = second_number + third_number;
	for (;;) {
		string line;
		if (!next) {
			printf("function indicates no more lines. completing\n");
			break;
		}
	    line = GetNextLine(next, MAX_LINE_LENGTH);			

		if (!line.length) {
			continue;
		}

		s32 line_as_number = StringToSignedInteger(line);
		s32 window = current_window + line_as_number;
		if (window > last_window) {
			++num_increases;
		}
		second_number = third_number;
		third_number = line_as_number;
		last_window = window;
		current_window = second_number + third_number;

		next = EatWhiteSpace(next + line.length);
	}

	printf("total number of increases (3 measurement window) is %d\n", num_increases);
}
