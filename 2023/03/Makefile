default: out/0301 out/0302
out:
	mkdir -p out/
out/0301: src/01.c out
	gcc -o out/0301 src/01.c -Wall -Werror -Wpedantic -Wextra -ggdb
out/0302: src/02.c out
	gcc -o out/0302 src/02.c -Wall -Werror -Wpedantic -Wextra -ggdb
clean:
	rm -r out
