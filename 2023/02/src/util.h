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

bool ReadEntireFile(const char *file_path, StringBuffer *buf) {
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

StringView StringView_TrimLeft(StringView str) {
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

StringView StringView_TakeLine(StringView *str) {
	StringView line = {0};

	if (!str || !str->data || !str->count) { return line; }

	line.data = str->data;

	bool has_carriage_return = false;
	for (size_t i = 0; i < str->count; ++i) {
		if (line.data[i] == '\n') {
			if (i && line.data[i-1] == '\r') { has_carriage_return = true; }
			break;
		}
		line.count += 1;
	}

	if (line.count == str->count) {
		str->data  += line.count;
		str->count -= line.count;
	} else {
		str->data  += line.count + 1;
		str->count -= line.count + 1;
	}

	if (has_carriage_return) { line.count -= 1; }
	return line;
}

StringView StringView_SplitOn(StringView *str, char delimiter) {
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
