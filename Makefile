CC=g++
CFLAGS=-Wall -Iinc/

TEST_SRCS = $(wildcard test/*.cc)
TEST_PROGS = $(patsubst %.cc,%,$(TEST_SRCS))
TEST_LIST = $(patsubst test%,bin%,$(TEST_PROGS))

$(info TEST_SRCS is $(TEST_SRCS))
$(info TEST_PROGS is $(TEST_PROGS))
$(info TEST_LIST is $(TEST_LIST))

tests: $(TEST_LIST)

bin/%: test/%.cc
	$(CC) $(CFLAGS) -o $@ $<
clean:
	rm -rf $(TEST_LIST)
