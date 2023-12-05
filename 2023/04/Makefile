default: out/0401 out/0402
out:
	mkdir -p out/
out/0401: src/01.c out
	gcc -o out/0401 src/01.c -Wall -Werror -Wpedantic -Wextra -ggdb
out/0402: src/02.c out
	gcc -o out/0402 src/02.c -Wall -Werror -Wpedantic -Wextra -ggdb
clean:
	rm -r out
