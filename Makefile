CC = cc
C_FLAGS = -std=gnu89 -O3 \
	-pedantic-errors \
    -Wall -Wextra -Werror \
	-Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition \
	-Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type \
    -Wdeclaration-after-statement \
    -Wshadow
DEBUG_FLAGS = -std=gnu89 -O0 -g

test: clean
	$(CC) $(DEBUG_FLAGS) encoder.c test/encoder.c -o test/bin/encoder

release: clean
	$(CC) $(C_FLAGS) encoder.c test/encoder.c -o test/bin/encoder

run: release
	./test/bin/encoder

clean:
	@rm -rf test/bin
	@mkdir -p test/bin
