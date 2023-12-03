#include <stdio.h>
#include "util.h"


int main() {
	StringBuffer buf;
	if (!ReadEntireFile("input/03.txt", &buf)) { return 1; }

	StringView content = {
		.data = buf.data,
		.count = buf.count,
	};

	for (int i = 1; content.count > 0; ++i) {
		content = StringView_TrimLeft(content);
		StringView line = StringView_TakeLine(&content);

		if (line.count == 0) {
			break;
		}
		fprintf(stderr, "%03d: %.*s\n", i, (int)line.count, line.data);
	}
}
