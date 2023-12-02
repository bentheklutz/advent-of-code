#include <stdio.h>
#include "util.h"

int main() {
	StringBuffer buf = {0};
	if (!ReadEntireFile("input/02.txt", &buf)) { return 1; }

	StringView contents = {
		.data = buf.data,
		.count = buf.count,
	};

	for (int i = 0; contents.count > 0; ++i) {
		contents = StringView_TrimLeft(contents);
		if (contents.count == 0) { break; }

		StringView line = StringView_TakeLine(&contents);
		fprintf(stderr, "%d: %.*s\n", i, (int)line.count, line.data);
	}
	return 0;
}
