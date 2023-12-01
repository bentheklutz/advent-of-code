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

	for (size_t i = 0; i < str->count; ++i) {
		if (line.data[i] == '\n') {
			if (i && line.data[i-1] == '\r') { line.count -= 1; }
			break;
		}
		line.count += 1;
	}

	str->data  += line.count;
	str->count -= line.count;

	return line;
}
