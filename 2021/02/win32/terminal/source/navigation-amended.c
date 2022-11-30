#include "helpers.c"

#define MAX_LINE_LENGTH 11
#define MAX_WORD_LENGTH 7 // "forward"

int main(int argc, char **argv) {
	u8 *filename = "input.txt";
	if (argc > 1) {
		filename = argv[1];
	}

	file input_file = OpenFileForReading(filename);
	if (!input_file.data) {
		printf("failed to open input file for reading (error: %llX", input_file.length);
	}

	s32 horizontal_position = 0;
	s32 depth = 0;
	s32 aim = 0;

	u8 *next = input_file.data;
	for (;;) {
		if (!next) {
			printf("program indicates end of input, finishing\n");
			break;
		}
		string line = GetNextLine(next, MAX_LINE_LENGTH);
		if (!line.data) {
			printf("program indicates no more lines, finishing\n");
			break;
		}
		string direction = TakeWord(line.data, MAX_WORD_LENGTH);
		u8 *rest_of_line_pointer = EatWhiteSpace(line.data+direction.length);
		// ^^ the start of our mag string
		string magnitude_string = TakeNumber(rest_of_line_pointer, MAX_LINE_LENGTH);
		u32 magnitude = StringToUnsignedInteger(magnitude_string);
		switch (*(direction.data)) {
			case 'f': {
				horizontal_position += magnitude;
				depth += aim*magnitude;
			} break;
			case 'd': {
				aim += magnitude;
			} break;
			case 'u': {
			    aim -= magnitude;
			} break;
			default: {
			} break;
		}
		next = EatWhiteSpace(magnitude_string.data+magnitude_string.length);
	}

	printf("H: %d D: %d Mul: %d\n", horizontal_position, depth, horizontal_position*depth);
	return 0;
}
