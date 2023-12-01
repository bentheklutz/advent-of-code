#include <stdio.h>
#include <ctype.h>
#include "util.h"

int main() {
	StringBuffer buf = {0};
	if (!ReadEntireFile("input/01.txt", &buf)) { return 1; }

	StringView content = {
		.data = buf.data,
		.count = buf.count,
	};

	int result = 0;
	for (int i = 0; content.count > 0; ++i) {
		content = StringView_TrimLeft(content);
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
			if (sv.count - j >= 5) {
				// compare with all
				if (!strncmp(sv.data+j, "one", 3)) {
					line_num[0] = '1';
					break;
				} else if (!strncmp(sv.data+j, "two", 3)) {
					line_num[0] = '2';
					break;
				} else if (!strncmp(sv.data+j, "three", 5)) {
					line_num[0] = '3';
					break;
				} else if (!strncmp(sv.data+j, "four", 4)) {
					line_num[0] = '4';
					break;
				} else if (!strncmp(sv.data+j, "five", 4)) {
					line_num[0] = '5';
					break;
				} else if (!strncmp(sv.data+j, "six", 3)) {
					line_num[0] = '6';
					break;
				} else if (!strncmp(sv.data+j, "seven", 5)) {
					line_num[0] = '7';
					break;
				} else if (!strncmp(sv.data+j, "eight", 5)) {
					line_num[0] = '8';
					break;
				} else if (!strncmp(sv.data+j, "nine", 4)) {
					line_num[0] = '9';
					break;
				}
			} else if (sv.count - j >= 4) {
				// exclude "eight", "seven", and "three"
				if (!strncmp(sv.data+j, "one", 3)) {
					line_num[0] = '1';
					break;
				} else if (!strncmp(sv.data+j, "two", 3)) {
					line_num[0] = '2';
					break;
				} else if (!strncmp(sv.data+j, "four", 4)) {
					line_num[0] = '4';
					break;
				} else if (!strncmp(sv.data+j, "five", 4)) {
					line_num[0] = '5';
					break;
				} else if (!strncmp(sv.data+j, "six", 3)) {
					line_num[0] = '6';
					break;
				} else if (!strncmp(sv.data+j, "nine", 4)) {
					line_num[0] = '9';
					break;
				}
			} else if (sv.count - j >= 3) {
				// exclude "four", "five", "nine"
				if (!strncmp(sv.data+j, "one", 3)) {
					line_num[0] = '1';
					break;
				} else if (!strncmp(sv.data+j, "two", 3)) {
					line_num[0] = '2';
					break;
				} else if (!strncmp(sv.data+j, "six", 3)) {
					line_num[0] = '6';
					break;
				}
			}
		}
		for (size_t k = sv.count - 1; ; --k) {
			if (isdigit(sv.data[k])) {
				line_num[1] = sv.data[k];
				break;
			}
			if (sv.count - k >= 5) {
				// compare with all
				if (!strncmp(sv.data+k, "one", 3)) {
					line_num[1] = '1';
					break;
				} else if (!strncmp(sv.data+k, "two", 3)) {
					line_num[1] = '2';
					break;
				} else if (!strncmp(sv.data+k, "three", 5)) {
					line_num[1] = '3';
					break;
				} else if (!strncmp(sv.data+k, "four", 4)) {
					line_num[1] = '4';
					break;
				} else if (!strncmp(sv.data+k, "five", 4)) {
					line_num[1] = '5';
					break;
				} else if (!strncmp(sv.data+k, "six", 3)) {
					line_num[1] = '6';
					break;
				} else if (!strncmp(sv.data+k, "seven", 5)) {
					line_num[1] = '7';
					break;
				} else if (!strncmp(sv.data+k, "eight", 5)) {
					line_num[1] = '8';
					break;
				} else if (!strncmp(sv.data+k, "nine", 4)) {
					line_num[1] = '9';
					break;
				}
			} else if (sv.count - k >= 4) {
				// exclude "eight", "seven", and "three"
				if (!strncmp(sv.data+k, "one", 3)) {
					line_num[1] = '1';
					break;
				} else if (!strncmp(sv.data+k, "two", 3)) {
					line_num[1] = '2';
					break;
				} else if (!strncmp(sv.data+k, "four", 4)) {
					line_num[1] = '4';
					break;
				} else if (!strncmp(sv.data+k, "five", 4)) {
					line_num[1] = '5';
					break;
				} else if (!strncmp(sv.data+k, "six", 3)) {
					line_num[1] = '6';
					break;
				} else if (!strncmp(sv.data+k, "nine", 4)) {
					line_num[1] = '9';
					break;
				}
			} else if (sv.count - k >= 3) {
				// exclude "four", "five", "nine"
				if (!strncmp(sv.data+k, "one", 3)) {
					line_num[1] = '1';
					break;
				} else if (!strncmp(sv.data+k, "two", 3)) {
					line_num[1] = '2';
					break;
				} else if (!strncmp(sv.data+k, "six", 3)) {
					line_num[1] = '6';
					break;
				}
			}
			if (k == 0) { break; }
		}

		int value = atoi(line_num);
		fprintf(stderr, "%.*s: %d\n", (int) sv.count, sv.data, value);
		result += value;
	}

	printf("Total value is %d\n", result);
	return 0;
}
