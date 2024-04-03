default: out/0501 out/0502

out:
	mkdir out/

out/0501: out src/01.c
	gcc -o out/0501 src/01.c -Wall -Werror -Wpedantic -Wextra -ggdb

out/0502: out src/02.c
	gcc -o out/0502 src/02.c -Wall -Werror -Wpedantic -Wextra -ggdb

clean:
	rm -rf out/
