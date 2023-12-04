default: out/0101 out/0102

out:
	mkdir -p out
out/0102: src/02.c out
	gcc -o out/0102 src/02.c -Wall -Werror -Wpedantic -Wextra -ggdb
out/0101: src/01.c out
	gcc -o out/0101 src/01.c -Wall -Werror -Wpedantic -Wextra -ggdb
clean:
	rm -r out
