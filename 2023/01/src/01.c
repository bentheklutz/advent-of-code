#include <stdio.h>
#include <ctype.h>
#include "util.h"

int main() {
	StringBuffer file = {0};
	if (!ReadEntireFile("input/01.txt", &file)) { return 1; }

	StringView content = {
		.data  = file.data,
		.count = file.count,
	};

	int result = 0;
	for (int i = 0; content.count > 0; ++i) {
		StringView sv = StringView_TakeLine(&content);

		if (sv.count == 0) {
			break;
		}

		char line_num[3];
		memset(line_num, 0, sizeof(line_num));
		for (size_t j = 0; j < sv.count; ++j) {
			if (isdigit(sv.data[j])) {
				line_num[0] = sv.data[j];
				break;
			}
		}
		for (size_t j = sv.count-1; ; --j) {
			if (isdigit(sv.data[j])) {
				line_num[1] = sv.data[j];
				break;
			}
			if (j == 0) {
				break;
			}
		}

		int value = atoi(line_num);
		fprintf(stderr, "%.*s: %d\n", (int) sv.count, sv.data, value);
		result += value;
	}

	printf("Total value is %d\n", result);
	return 0;
}
