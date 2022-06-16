cc = clang
cc_opts = -Wall -Wextra -g

PREFIX ?= /usr/local

test: test.c vector.h static_vector.h
	$(cc) $(cc_opts) -o $@ $<

install:
	@cp -v vector.h $(PREFIX)/include/vector.h
	@cp -v static_vector.h $(PREFIX)/include/static_vector.h

clean:
	rm -f test

.PHONY: install clean

