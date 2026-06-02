CC = gcc
WARN_FLAGS = \
	-pedantic-errors \
    -Wall -Wextra -Werror \
	-Wno-char-subscripts \
	-Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition \
	-Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type \
    -Wshadow
C_FLAGS = -std=c99 -O3 $(WARN_FLAGS)
DEBUG_FLAGS = -std=c99 -O0 -g $(WARN_FLAGS)
FUZZ_FLAGS = -std=c99 -O3 -g -fsanitize=address,undefined $(WARN_FLAGS)

debug: clean
	$(CC) $(DEBUG_FLAGS) encoder.c test/encoder.c -o test/bin/encoder
	$(CC) $(DEBUG_FLAGS) encoder.c decoder.c test/decoder.c -o test/bin/decoder

release: clean
	$(CC) $(C_FLAGS) encoder.c test/encoder.c -o test/bin/encoder
	$(CC) $(C_FLAGS) encoder.c decoder.c test/decoder.c -o test/bin/decoder

fuzz: clean
	$(CC) $(FUZZ_FLAGS) encoder.c decoder.c test/fuzz_encoder.c -o test/bin/fuzz_encoder
	$(CC) $(FUZZ_FLAGS) encoder.c decoder.c test/fuzz_decoder.c -o test/bin/fuzz_decoder

fuzz-run: fuzz
	@echo "Running encoder fuzzer with 100,000 iterations..."
	./test/bin/fuzz_encoder 100000
	@echo "Running decoder fuzzer..."
	./test/bin/fuzz_decoder

test: debug
	-./test/bin/encoder
	./test/bin/decoder

run: release
	-./test/bin/encoder
	./test/bin/decoder

clean:
	@rm -rf test/bin
	@mkdir -p test/bin
