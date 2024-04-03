default: out/0701 out/0702

out:
	mkdir out/

out/0701: out src/01.c
	gcc -o out/0701 src/01.c -DSILENT -Wall -Werror -Wpedantic -Wextra -ggdb

out/0702: out src/02.c
	gcc -o out/0702 src/02.c -DSILENT -Wall -Werror -Wpedantic -Wextra -ggdb

clean:
	rm -rf out/
