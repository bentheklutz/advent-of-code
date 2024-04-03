default: out/0801 out/0802

out:
	mkdir out/

out/0801: out src/01.c
	gcc -o out/0801 src/01.c -DSILENT -Wall -Werror -Wpedantic -Wextra -ggdb

out/0802: out src/02.c
	gcc -o out/0802 src/02.c -DSILENT -Wall -Werror -Wpedantic -Wextra -ggdb

clean:
	rm -rf out/
