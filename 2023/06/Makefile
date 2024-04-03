default: out/0601 out/0602

out:
	mkdir out/

out/0601: out src/01.c
	gcc -o out/0601 src/01.c -DSILENT -Wall -Werror -Wpedantic -Wextra -ggdb

out/0602: out src/02.c
	gcc -o out/0602 src/02.c -DSILENT -Wall -Werror -Wpedantic -Wextra -ggdb

clean:
	rm -rf out/
