CC = gcc
WARN_FLAGS = \
	-pedantic-errors \
    -Wall -Wextra -Werror \
	-Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition \
	-Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type \
    -Wshadow
C_FLAGS = -std=c99 -O3 $(WARN_FLAGS)
DEBUG_FLAGS = -std=c99 -O0 -g $(WARN_FLAGS)
FUZZ_FLAGS = -std=c99 -O3 -g -fsanitize=address,undefined $(WARN_FLAGS)

debug: clean
	$(CC) $(DEBUG_FLAGS) encoder.c decoder.c test/test_codec.c -o test/bin/test_codec

release: clean
	$(CC) $(C_FLAGS) encoder.c decoder.c test/test_codec.c -o test/bin/test_codec

fuzz: clean
	$(CC) $(FUZZ_FLAGS) encoder.c decoder.c test/fuzz_codec.c -o test/bin/fuzz_codec

fuzz-run: fuzz
	@echo "Running codec fuzzer..."
	./test/bin/fuzz_codec

test: debug
	./test/bin/test_codec

run: release
	./test/bin/test_codec

clean:
	@rm -rf test/bin
	@mkdir -p test/bin
