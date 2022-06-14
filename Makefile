cc = clang
cc_opts = -Wall -Wextra -g

ifndef PREFIX
	PREFIX = /usr/local
endif

test: test.c vector.h static_vector.h
	$(cc) $(cc_opts) -o $@ $<

install:
	@cp -v vector.h $(PREFIX)/include/vector.h

clean:
	rm -f test

.PHONY: install clean

