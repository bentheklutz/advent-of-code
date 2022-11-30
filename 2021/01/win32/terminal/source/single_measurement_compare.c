#include "helpers.c"

#if 0
#define ERROR_FILE_NOT_FOUND 0x2
#endif


int main(int argc, char **argv)
{
	u8 *filename = "input.txt";
	if (argc > 1) {
		filename = argv[1];
	}
	file input_file = OpenFileForReading(filename);
	if (!input_file.data) {
		u32 error = input_file.length;
		if (error == ERROR_FILE_NOT_FOUND) {
			printf("failed to open file (not found)\n");
			return 1;
		} else if (error == ERROR_ALREADY_EXISTS) {
			printf("failed to open file (mapping object already exists)\n");
			return 1;
		} else {
			printf("failed to open file (windows error %X)\n", error);
			return 1;
		}
	}

	int num_increases = 0;

	string first_line = GetNextLine(input_file.data, MAX_LINE_LENGTH);
	s32 last_number = StringToSignedInteger(first_line);
	u8 *next = EatWhiteSpace(input_file.data+first_line.length);	
	for (;;) {
		string line;
		if (!next) {
			printf("function indicates no more data. completing\n");
			break;
		}
		line = GetNextLine(next, MAX_LINE_LENGTH);

		s32 line_as_number = StringToSignedInteger(line);
		if (line_as_number > last_number) {
			++num_increases;
		}
		last_number = line_as_number;

		next = EatWhiteSpace(next+line.length);
	}

	printf("total number of increases is %d\n", num_increases);

	return 0;
}
