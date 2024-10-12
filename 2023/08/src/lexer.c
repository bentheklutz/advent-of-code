#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>

#include <stdint.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef size_t  usize;
typedef ssize_t ssize;

#define DYNARRAY_INIT_CAP 1024

#define make_array_type(elem_type, name)\
typedef struct { \
	elem_type *data; \
	usize count; \
	usize capacity; \
} name; \
void name##_add(name *arr, elem_type e) \
{ \
	if (arr->count == arr->capacity) {                                      \
		arr->capacity = arr->capacity ?                                     \
			arr->capacity*2                                                  \
			: DYNARRAY_INIT_CAP;                                              \
		arr->data = realloc(arr->data, arr->capacity*sizeof(elem_type));                     \
	}                                                                         \
	arr->data[arr->count] = e; \
	arr->count += 1; \
}

make_array_type(char, StringBuffer)
make_array_type(u32, u32_array);

typedef struct StringView {
	char *data;
	usize count;
} StringView;

#define SV_FMT "%.*s"
#define SV_ARG(sv) (int)((sv).count), (sv).data

#define SV_literal(str) { .data = str, .count = sizeof(str)-1 }

bool string_view_equal(StringView left, StringView right) {
	if (left.count != right.count) { return false; }
	if (left.data == right.data)   { return true; }
	for (usize i = 0; i < left.count; i += 1) {
		if (left.data[i] != right.data[i]) { return false; }
	}
	return true;
}

typedef struct {
	char *input;
	usize pos, bol, line;
	usize len;
} Lexer;

static char lexer_current_char(Lexer *lexer)
{
	return lexer->input[lexer->pos];
}

void lexer_set_input_from_file(Lexer *lexer, const char *path)
{
	int fd = open(path, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Failed to open file: %s\n", path);
		return;
	}

	struct stat st = {0};
	fstat(fd, &st);

	usize file_size = st.st_size;

	char *contents_buffer = malloc(file_size);
	if (!contents_buffer) {
		fprintf(stderr, "Failed to allocate %zu bytes for file contents\n", file_size);
		close(fd);
		return;
	}
	char read_buffer[4096];

	for(int i = 0; ; ++i) {
		int bytes_read = read(fd, read_buffer, sizeof(read_buffer));
		if (!bytes_read) { break; }
		memcpy(contents_buffer+(i*sizeof(read_buffer)), read_buffer, bytes_read);
	}

	lexer->input = contents_buffer;
	lexer->len   = file_size;

	close(fd);
}

typedef enum {
	TokenKind_EndOfInput,
	TokenKind_LeftParen,
	TokenKind_RightParen,
	TokenKind_Comma,
	TokenKind_Equals,
	TokenKind_Identifier,
	TokenKind_Invalid,
} TokenKind;

StringView token_kind_name(TokenKind kind)
{
	StringView result;
	switch (kind) {
		case TokenKind_EndOfInput: {
			result = (StringView)SV_literal("end of input");
		} break;
		case TokenKind_LeftParen: {
			result = (StringView)SV_literal("(");
		} break;
		case TokenKind_RightParen: {
			result = (StringView)SV_literal(")");
		} break;
		case TokenKind_Comma: {
			result = (StringView)SV_literal(",");
		} break;
		case TokenKind_Equals: {
			result = (StringView)SV_literal("=");
		} break;
		case TokenKind_Identifier: {
			result = (StringView)SV_literal("identifier");
		} break;
		case TokenKind_Invalid: {
			result = (StringView)SV_literal("invalid");
		} break;
	}
	return result;
}

typedef struct {
	TokenKind kind;
	StringView text;
	union {
		s64 numeric_value;
	} value;
} Token;

StringView token_as_string(Token token)
{
	StringView result;
	switch (token.kind) {
		case TokenKind_EndOfInput: {
			result = (StringView)SV_literal("end of input");
		} break;
		case TokenKind_LeftParen: {
			result = (StringView)SV_literal("(");
		} break;
		case TokenKind_RightParen: {
			result = (StringView)SV_literal(")");
		} break;
		case TokenKind_Comma: {
			result = (StringView)SV_literal(",");
		} break;
		case TokenKind_Equals: {
			result = (StringView)SV_literal("=");
		} break;
		case TokenKind_Identifier: {
			result = token.text;
		} break;
		case TokenKind_Invalid: {
			result = (StringView)SV_literal("invalid");
		} break;
	}
	return result;
}

void lexer_eat_whitespace(Lexer *lexer)
{
	if (lexer->pos >= lexer->len) { return; }

	char curr = lexer_current_char(lexer);
	while(isspace(curr))
	{
		if (curr == '\n') {
			lexer->line += 1;
			lexer->bol = lexer->pos + 1;
		}


		lexer->pos += 1;
		if (lexer->pos == lexer->len) { break; }
		curr = lexer_current_char(lexer);
	}
 }

Token lexer_take_token(Lexer *lexer)
{
	Token token = {0};

	lexer_eat_whitespace(lexer);
	if (lexer->pos >= lexer->len) { return token; }
	token.text.data = lexer->input + lexer->pos;

	char curr = lexer_current_char(lexer);
	if (curr == '(') {
		lexer->pos += 1;
		token.text.count += 1;
		token.kind = TokenKind_LeftParen;
		return token;
	} else if (curr == ')') {
		lexer->pos += 1;
		token.text.count += 1;
		token.kind = TokenKind_RightParen;
		return token;
	} else if (curr == '=') {
		lexer->pos += 1;
		token.text.count += 1;
		token.kind = TokenKind_Equals;
		return token;
	} else if (curr == ',') {
		lexer->pos += 1;
		token.text.count += 1;
		token.kind = TokenKind_Comma;
		return token;
	} else if (isalpha(curr)) {
		token.kind = TokenKind_Identifier;
		while (isalnum(curr)) {
			token.text.count += 1;

			lexer->pos += 1;
			if (lexer->pos == lexer->len) { break; }

			curr = lexer_current_char(lexer);
		}
		return token;
	}

	lexer->pos += 1;
	token.kind = TokenKind_Invalid;
	token.text.count += 1;
	return token;
}

Token lexer_expect_kind(Lexer *lexer, TokenKind kind)
{
	Token token = lexer_take_token(lexer);
	if (token.kind != kind) {
		fprintf(stderr, "expected '"SV_FMT"' but found '"SV_FMT"'.\n", SV_ARG(token_kind_name(kind)), SV_ARG(token_as_string(token)));
		token.kind = TokenKind_Invalid;
	}
	return token;
}

#define UNUSED(a) do { a = a; } while (0)

int main(void)
{
	Lexer _lexer = {0};
	Lexer *lexer = &_lexer;
	lexer_set_input_from_file(lexer, "input/example.txt");

	Token moves_token = lexer_take_token(lexer);
	for (usize i = 0; i < moves_token.text.count; i += 1) {
		if (moves_token.text.data[i] != 'L' && moves_token.text.data[i] != 'R') {
			fprintf(stderr, "Invalid choice in input at position %lu: %c.\n", i, moves_token.text.data[i]);
			return 1;
		}
	}
	while (true) {
		Token primary = lexer_take_token(lexer);
		if (primary.kind == TokenKind_EndOfInput) { break; }

		Token equals = lexer_expect_kind(lexer, TokenKind_Equals);
		if (equals.kind == TokenKind_Invalid) { return 1; }

		Token l_paren = lexer_expect_kind(lexer, TokenKind_LeftParen);
		if (l_paren.kind == TokenKind_Invalid) { return 1; }

		Token left = lexer_expect_kind(lexer, TokenKind_Identifier);
		if (left.kind == TokenKind_Invalid) { return 1; }

		Token comma = lexer_expect_kind(lexer, TokenKind_Comma);
		if (comma.kind == TokenKind_Invalid) { return 1; }

		Token right = lexer_expect_kind(lexer, TokenKind_Identifier);
		if (right.kind == TokenKind_Invalid) { return 1; }

		Token r_paren = lexer_expect_kind(lexer, TokenKind_RightParen);
		if (r_paren.kind == TokenKind_Invalid) { return 1; }

		fprintf(stderr, SV_FMT" "SV_FMT" "SV_FMT SV_FMT SV_FMT" "SV_FMT SV_FMT"\n", SV_ARG(token_as_string(primary)), SV_ARG(token_as_string(equals)), SV_ARG(token_as_string(l_paren)), SV_ARG(token_as_string(left)), SV_ARG(token_as_string(comma)), SV_ARG(token_as_string(right)), SV_ARG(token_as_string(r_paren)));
	}

	return 0;
}
