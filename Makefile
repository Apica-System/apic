.PHONY: build debug run clean

CXX :=		g++
CXXFLAGS :=	-Wall -Wextra -Wpedantic -Wold-style-cast -std=c++20 -O2
DBGFLAGS :=	-Wall -Wextra -Wpedantic -Wold-style-cast -std=c++20 -g

INCLUDES :=	-Iinc -Icommon/inc -Ilibs/inc
OBJDIR :=	temp
EXT ?=		.exe
ARGS ?=		# console arguments

SOURCES := \
	$(wildcard src/main.cpp) \
	$(wildcard src/utils/*.cpp) \
	$(wildcard src/core/*.cpp) \
	$(wildcard src/nodes/*.cpp) \
	$(wildcard src/nodes/data/*.cpp) \
	$(wildcard src/lsp/*.cpp) \
	$(wildcard common/src/*.cpp) \
	$(wildcard common/src/values/*.cpp)

OBJECTS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SOURCES))

build: $(OBJECTS)
	$(CXX) $(INCLUDES) $(OBJECTS) $(CXXFLAGS) -o apic$(EXT)

debug: $(OBJECTS)
	$(CXX) $(INCLUDES) $(OBJECTS) $(DBGFLAGS) -o apic$(EXT)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

run:
	./apic$(EXT) $(ARGS)

clean:
	rm -rf $(OBJDIR) apic$(EXT)