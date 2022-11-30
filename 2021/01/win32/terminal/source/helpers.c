#include <stdio.h>
#include <windows.h>
#include <stdint.h>

#define MAX_LINE_LENGTH 10

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef struct {
	u64 length;
	u8 *data;
} file;

typedef struct {
	u64 length;
	u8 *data;
} string;

file OpenFileForReading(u8 *filename)
{
	HANDLE file_handle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0x0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0x0);
	if (file_handle == INVALID_HANDLE_VALUE) {
		u32 error = GetLastError();
		file out = {error};
		return out;
	} else {
		HANDLE file_handle_mapping = CreateFileMappingA(file_handle, 0x0, PAGE_READONLY, 0, 0, 0x0);
		if (!file_handle_mapping) {
			u32 error = GetLastError();
			file out = {error};
			return out;
		} else {
			void *file_data = MapViewOfFile(file_handle_mapping, FILE_MAP_READ, 0, 0, 0);
			if (!file_data) {
				u32 error = GetLastError();
				file out = {error};
				return out;
			} else {
				LARGE_INTEGER file_size;
				if (GetFileSizeEx(file_handle, &file_size)) {
					file out = {file_size.QuadPart};
					out.data = file_data;
					return out;
				} else {
					UnmapViewOfFile(file_data);
					CloseHandle(file_handle_mapping);
					CloseHandle(file_handle);
					u32 error = GetLastError();
					file out = {error};
					return out;
				}
			}
		}
	}
}

string GetNextLine(u8 *data, u64 max_length) {
	s32 charsize = 0;
 	int end_string = 0;
	while (charsize < max_length) {
		u8 current_char = *(data+charsize);
		if (current_char == '\r' || current_char == '\n' || current_char == '\0') {
			break;
		}
		++charsize;
	}
	if (charsize == 0) {
		string empty = {0};
		return empty;
	}
	string line = {charsize};
	line.data = data;
	return line;
}

u8 *EatWhiteSpace(u8 *data) {
	u64 iterations = 0;
	for (; ;) {
		u8 byte = *(data+iterations);
		if (byte == '\t' || byte == ' ' || byte == '\n' || byte == '\r') {
		} else if (byte == '\0') {
			return (u8 *)0x0;
		} else {
			return data+iterations;
		}
		++iterations;
	}
}


int RaiseToPower_iterative(int base, u32 exponent) {
	if (exponent == 0) {
		return 1;
	}
	if (base == 0) {
		return 0;
	}
	int result = 1;
	for (int iteration = 1; iteration <= exponent; ++iteration) {
		result = result * base;
	}
	return result;
}

int RaiseToPower_recursive(int base, u32 exponent) {
	if (exponent == 0) {
		return 1;
	}
	if (base == 0) {
		return 0;
	}
	return base * RaiseToPower_recursive(base, exponent-1);
}

s32 StringToSignedInteger(string input) {
	int positive = 1;
	if (*(input.data) == '-') {
		positive = 0;
	}
	int start_index = 0;
	s32 result = 0;
	if (!positive) {
		start_index = 1;
	}
	u8 non_numerical_characters[128];
	int number_bad_characters = 0;
	for(int index = start_index; index < input.length; ++index) {
		int power_offset = input.length - index - 1;
		u8 char_to_process = *(input.data+index);
		switch (char_to_process) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': {
				// -552
				// length : 4
				// idx = 1 -> 5 * 10^4-1-1 -> 5*10^2 -> 500
				// idx = 2 -> 5 * 10^4-2-1 -> 5*10^1 -> 50
				// idx = 3 -> 2 * 10^4-3-1 -> 2*10^0 -> 2
				int num = char_to_process - '0';
				result += (num*RaiseToPower_iterative(10, power_offset));
			} break;
			default: {
				non_numerical_characters[number_bad_characters] = char_to_process;
				++number_bad_characters;
			} break;				
		}
	}
	// c-strings...
	non_numerical_characters[number_bad_characters] = '\0';
	if (number_bad_characters) {
		printf("%d invalid characters were found, unless they were all leading it messed up the parse\n\t%s\n", number_bad_characters, non_numerical_characters);
	}
	return result;
}
