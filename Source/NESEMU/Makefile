SOURCES = $(wildcard *.swift)
EXECUTABLE = nesemu

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	swiftc -o $@ $^ -framework SpriteKit

clean:
	rm -f $(EXECUTABLE)

.PHONY: all clean