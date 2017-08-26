SLAVE=slave
SOURCES=$(wildcard [^_]*.c)
GCC=gcc

all: $(SLAVE)

$(SLAVE): $(SOURCES)
	$(GCC) $(SOURCES) -o $(SLAVE)

clean:
	rm -rf *.o

.PHONY: all clean print
