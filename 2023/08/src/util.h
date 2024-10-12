#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>

typedef struct StringBuffer {
	char *data;
	size_t count;
	size_t capacity;
} StringBuffer;

typedef struct StringView {
	char *data;
	size_t count;
} StringView;

#define SV_FMT "%.*s"
#define SV_ARG(sv) (int)((sv).count), (sv).data
typedef struct DynArray {
	void   *data;
	size_t count;
	size_t capacity;
} DynArray;

#define DYNARRAY_INIT_CAP 1024

#define dyn_array_as_type(da, T) ((T *)(da).data)

#define dyn_array_add(da, item)                                                    \
	do {                                                                             \
		if ((da)->count == (da)->capacity) {                                      \
			(da)->capacity = (da)->capacity ?                                     \
				(da)->capacity*2                                                  \
				: DYNARRAY_INIT_CAP;                                              \
			(da)->data = realloc((da)->data, (da)->capacity*sizeof(item));                     \
		}                                                                         \
		memcpy((char *)(da)->data+(da)->count*sizeof(item), &item, sizeof(item)); \
		(da)->count += 1;                                                         \
	} while (0)


// Hand *elems = (Hand *)((&hands_da)->data);
// if ((lowest_score_index) <= ((&hands_da)->count)) {
// 	memmove(elems+lowest_score_index, elems+lowest_score_index+1, ((&hands_da)->count - lowest_score_index-1)*sizeof(Hand));
// 	(&hands_da)->count -= 1;
// }
#define dyn_array_remove(da, idx, T)                                               \
	do {                                                                           \
		T *elems = (T *)((da)->data);                                              \
		if ((idx) <= ((da)->count)) {                                              \
			memmove(elems+idx, elems+idx+1, ((da)->count - idx-1)*sizeof(T));      \
			(da)->count -= 1;                                                      \
		}                                                                          \
	} while (0)

bool read_entire_file(const char *file_path, StringBuffer *buf) {
	bool result = false;

	if (!buf) { return result; }

	int fd = open(file_path, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Failed to open file: %s\n", file_path);
		return result;
	}

	struct stat st = {0};
	fstat(fd, &st);

	size_t file_size = st.st_size;

	char *contents_buffer = malloc(file_size);
	if (!contents_buffer) {
		fprintf(stderr, "Failed to allocate %zu bytes for file contents\n", file_size);
		close(fd);
		return result;
	}
	char read_buffer[4096];

	for(int i = 0; ; ++i) {
		int bytes_read = read(fd, read_buffer, sizeof(read_buffer));
		if (!bytes_read) { break; }
		memcpy(contents_buffer+(i*sizeof(read_buffer)), read_buffer, bytes_read);
	}

	buf->data = contents_buffer;
	buf->count = file_size;
	buf->capacity = file_size;

	close(fd);

	result = true;
	return result;
}

bool string_view_equal(StringView left, StringView right) {
	if (left.count != right.count) { return false; }
	if (left.data == right.data) { return true; }
	for (size_t i = 0; i < left.count; i += 1) {
		if (left.data[i] != right.data[i]) { return false; }
	}
	return true;
}

StringView string_view_trim_left(StringView str) {
	StringView result = {0};
	if (!str.data) { return result; }

	result.data = str.data;
	result.count = str.count;

	while (result.count > 0 && isspace(*result.data)) {
		result.data  += 1;
		result.count -= 1;
	}

	if (!result.count) {
		result.data = 0;
	}
	return result;
}

StringView string_view_take_line(StringView *str) {
	StringView line = {0};

	if (!str || !str->data || !str->count) { return line; }

	line.data = str->data;

	bool had_carriage_return = false;
	for (size_t i = 0; i < str->count; ++i) {
		if (line.data[i] == '\n') {
			if (i && line.data[i-1] == '\r') { had_carriage_return = true; }
			break;
		}
		line.count += 1;
	}

	// Skip past the matched newline if it exists
	if (line.count < str->count) {
		str->data += line.count + 1;
		str->count -= line.count + 1;
	} else {
		str->data  += line.count;
		str->count -= line.count;
	}

	if (had_carriage_return) { line.count -= 1; }

	return line;
}

StringView string_view_split_on(StringView *str, char delimiter) {
	StringView result = {0};

	if (!str || !str->data || !str->count) { return result; }

	result.data = str->data;

	for (size_t i = 0; i < str->count; ++i) {
		if (result.data[i] == delimiter) {
			break;
		}
		result.count += 1;
	}

	if (result.count < str->count) {
		str->data += result.count + 1;
		str->count -= result.count + 1;
	} else {
		str->data  += result.count;
		str->count -= result.count;
	}

	return result;
}

void string_view_eat(StringView *str, char character) {
	if (!str || !str->data || !str->count) { return; }

	for (size_t i = 0; i < str->count; ++i) {
		if (*(str->data) != character) { break; }
		str->count -= 1;
		str->data += 1;
	}
}

DynArray string_view_lines(StringView str) {
	DynArray arr = {0};
	for (; str.count > 0;) {
		StringView line = string_view_take_line(&str);
		dyn_array_add(&arr, line);
	}
	return arr;
}

DynArray string_view_take_numbers(StringView str) {
	DynArray arr = {0};
	StringView current = {0};
	bool in_number = false;
	for (; str.count > 0;) {
		if (in_number) {
			if (str.data[0] >= '0' && str.data[0] <= '9') {
				current.count += 1;
			} else {
				dyn_array_add(&arr, current);
				current.data = 0x0;
				current.count = 0;
				in_number = false;
			}
		} else {
			if (str.data[0] >= '0' && str.data[0] <= '9') {
				in_number = true;
				current.data = str.data;
				current.count += 1;
			}
		}
		str.data  += 1;
		str.count -= 1;
	}
	if (in_number) {
		dyn_array_add(&arr, current);
	}
	return arr;
}

DynArray string_view_parse_numbers(StringView str) {
	DynArray arr = {0};
	StringView current = {0};
	bool in_number = false;
	for (; str.count > 0;) {
		if (in_number) {
			if (str.data[0] >= '0' && str.data[0] <= '9') {
				current.count += 1;
			} else {
				long number = strtoll(current.data, 0x0, 10);
				dyn_array_add(&arr, number);
				current.data = 0x0;
				current.count = 0;
				in_number = false;
			}
		} else {
			if (str.data[0] >= '0' && str.data[0] <= '9') {
				in_number = true;
				current.data = str.data;
				current.count += 1;
			}
		}
		str.data  += 1;
		str.count -= 1;
	}
	if (in_number) {
		long number = strtoll(current.data, 0x0, 10);
		dyn_array_add(&arr, number);
	}
	return arr;
}

StringView string_view_remove_spaces(StringView input, int num_spaces) {
	StringView result = {0};

	result.data = malloc(input.count - num_spaces);
	result.count = input.count - num_spaces;

	size_t input_index = 0;
	for (size_t i = 0; i < result.count; i += 1) {
		while (input.data[input_index] == ' ') {
			input_index += 1;
		}
		result.data[i] = input.data[input_index];
		input_index += 1;
	}

	return result;
}

DynArray string_view_parse_numbers_with_spaces(StringView str) {
	DynArray arr = {0};
	StringView current = {0};
	int num_spaces_in_number;
	bool in_number = false;
	for (; str.count > 0;) {
		if (in_number) {
			if (str.data[0] >= '0' && str.data[0] <= '9') {
				current.count += 1;
			} else if (str.data[0] == ' ') {
				current.count += 1;
				num_spaces_in_number += 1;
			} else {
				// An allocation here sucks, but I don't really want to fuck with the input.
				// Maybe I should.
				StringView sv = string_view_remove_spaces(current, num_spaces_in_number);
				long number = strtoll(sv.data, 0x0, 10);
				dyn_array_add(&arr, number);
				current.data = 0x0;
				current.count = 0;
				in_number = false;
			}
		} else {
			if (str.data[0] >= '0' && str.data[0] <= '9') {
				in_number = true;
				num_spaces_in_number = 0;
				current.data = str.data;
				current.count += 1;
			}
		}
		str.data  += 1;
		str.count -= 1;
	}
	if (in_number) {
		StringView sv = string_view_remove_spaces(current, num_spaces_in_number);
		long number = strtoll(sv.data, 0x0, 10);
		dyn_array_add(&arr, number);
	}
	return arr;
}

DynArray string_view_take_symbols(StringView str) {
	DynArray arr = {0};
	StringView current = {0};
	for (; str.count > 0;) {
		if (str.data[0] != '.' && !(str.data[0] >= '0' && str.data[0] <= '9')) {
			current.data = str.data;
			current.count = 1;
			dyn_array_add(&arr, current);
		}
		str.data  += 1;
		str.count -= 1;
	}
	return arr;
}
