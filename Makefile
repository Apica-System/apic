.PHONY: build run

EXT ?=		.exe
COMP ?=		g++
OUTPUT :=	build
PROJECT :=	apic
SOURCES :=	src/*.cpp src/utils/*.cpp src/common/*.cpp src/core/*.cpp src/nodes/*.cpp src/common/values/*.cpp
INCLUDES :=	-Iinc
OPTIONS ?=	-O2 -std=c++23 -Wall
ARGS :=		# console arguments


build:
	$(COMP) $(INCLUDES) $(SOURCES) -o $(OUTPUT)/$(PROJECT)$(EXT) $(OPTIONS)

run:
	./$(OUTPUT)/$(PROJECT)$(EXT) $(ARGS)