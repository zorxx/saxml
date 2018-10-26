# \copyright 2017-2018 Zorxx Software. All rights reserved.
# \license This file is released under the MIT License. See the LICENSE file for details.
# \brief Embedded XML Parser
APP = saxml
SRC = saxml.c main.c 
VPATH = ..

LINKFILE=
CC = gcc
CXX = g++
LD = ld

CDEFS = DEBUG_PRINT
CFLAGS = -O3 -Wall -pedantic
CFLAGS += $(foreach def,$(CDEFS),-D${def})
CFLAGS += -I..
LFLAGS = -lm -lrt -lc -Wl,-Map=$(APP).map

all: $(APP)

$(APP): $(patsubst %.c,%.o,$(SRC))
	@echo "LD $@"
	@$(CXX) $(LFLAGS) $^ -o $@

%.o: %.cpp
	@echo "CC $^ -> $@"
	@$(CXX) -c -o $@ $(CFLAGS) $^

%.o: %.c
	@echo "CC $^ -> $@"
	@$(CC) -c -o $@ $(CFLAGS) $^

clean:
	@echo "Cleaning ${APP}"
	@rm -f *.o $(APP).map $(APP)
.PHONY: clean
