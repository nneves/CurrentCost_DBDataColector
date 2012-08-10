# rules
export CC=g++
export CLIBDEFS = -g -c -Wall
export CDEFS = -lpq -g -Wall

# directories
BASE :=$(shell pwd)
export SRC=$(BASE)/src
export LIB=$(BASE)/lib
export BIN=$(BASE)/bin

all :
	cd $(LIB) && $(MAKE) all
	cd $(SRC) && $(MAKE) all
	$(CC) $(CDEFS) \
		$(LIB)/LibParser.o \
		$(LIB)/LibPostgresql.o \
		$(SRC)/SerialPortParser.o \
		-o $(BIN)/SerialPortParser

clean :
	cd $(LIB) && $(MAKE) clean
	cd $(SRC) && $(MAKE) clean
	cd $(BIN) && $(RM) SerialPortParser
	
doc  :
#	doxygen

