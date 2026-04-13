CC = cc
DEBUG_FLAGS = -std=c99 -O0 -g

encoder: clean
	$(CC) $(DEBUG_FLAGS) encoder.c test/encoder.c -o test/bin/encoder

clean:
	@rm -rf test/bin
	@mkdir -p test/bin
