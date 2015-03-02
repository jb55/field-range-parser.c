
DEPS = $(wildcard deps/*/*.c)

all: test

test: test.c field-range-parser.c
	@$(CC) $(DEPS) $^ -Ideps/tap.c -o test
	./test

clean:
	rm -f test

.PHONY: test
