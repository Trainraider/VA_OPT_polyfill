.PHONY: all test test_godbolt

CC ?= gcc
CFLAGS ?=

godbolt-tester:
	git submodule update --init

godbolt-tester/venv: | godbolt-tester
	python -m venv godbolt-tester/venv
	godbolt-tester/venv/bin/pip install -r godbolt-tester/requirements.txt

va_opt_test: va_opt.h
	$(CC) $(CFLAGS) -x c -DTEST_VA_OPT va_opt.h -o va_opt_test

all: va_opt_test

test: va_opt_test
	./va_opt_test

test_godbolt: all
	godbolt-tester/venv/bin/python godbolt-tester/runner.py test.yaml -T