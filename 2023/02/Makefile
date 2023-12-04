default: out/0201 out/0202
out:
	mkdir -p out
out/0201: src/01.c out
	gcc -o out/0201 src/01.c -Wall -Werror -Wpedantic -Wextra -ggdb
out/0202: src/02.c out
	gcc -o out/0202 src/02.c -Wall -Werror -Wpedantic -Wextra -ggdb
clean:
	rm -r out/
