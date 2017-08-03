CC=g++
CFLAGS=-std=c++11 -Wall -Iinc/

TEST_SRCS = $(wildcard test/*.cc)
TEST_PROGS = $(patsubst %.cc,%,$(TEST_SRCS))
TEST_LIST = $(patsubst test%,bin%,$(TEST_PROGS))

#$(info TEST_SRCS is $(TEST_SRCS))
#$(info TEST_PROGS is $(TEST_PROGS))
#$(info TEST_LIST is $(TEST_LIST))

tests: $(TEST_LIST)

SRC_SRCS = $(wildcard test/*.cc)
SRC_PROGS = $(patsubst %.cc,%,$(SRC_SRCS))
SRC_LIST = $(patsubst test%,bin%,$(SRC_PROGS))

#$(info SRC_SRCS is $(SRC_SRCS))
#$(info SRC_PROGS is $(SRC_PROGS))
#$(info SRC_LIST is $(SRC_LIST))

tests: $(TEST_LIST)
srcs: $(SRC_LIST)

bin/%: src/%.cc
	$(CC) $(CFLAGS) -o $@ $<
bin/%: test/%.cc
	$(CC) $(CFLAGS) -o $@ $<
clean:
	rm -rf $(TEST_LIST)
	rm -rf $(SRC_LIST)
