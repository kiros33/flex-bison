CC = gcc
CFLAGS += -x c++ -Wno-deprecated-register -std=gnu++11 
CXX = g++
CXXFLAGS += -x c++ -Wno-deprecated-register -std=gnu++11
LFLAGS += 
ECHO = @echo
CD = cd
RM = rm -rf

#SRC_DIR = src
#OBJ_DIR = obj
#BIN_DIR = bin
#TEST_DIR = test
SRC_DIR = .
OBJ_DIR = .
BIN_DIR = .
TEST_DIR = .

#INC_DIR = inc
#LIB_DIR = lib
INC_DIR = .
LIB_DIR = .

#MAKE_DIR = $(CURDIR)
#FLEX_SRC_DIR = $(MAKE_DIR)/src
#BISON_SRC_DIR = $(MAKE_DIR)/src
MAKE_DIR = $(CURDIR)
FLEX_SRC_DIR = $(MAKE_DIR)
BISON_SRC_DIR = $(MAKE_DIR)

#MAIN_SRC = $(SRC_DIR)/main.cpp
MAIN_SRC = 

_FLEXBISON_OBJS = fb1-1.opp
FLEXBISON_OBJS = $(patsubst %,$(OBJ_DIR)/%,$(_FLEXBISON_OBJS))
_OBJS = 
OBJS = $(patsubst %,$(OBJ_DIR)/%,$(_OBJS))

all:
	$(info ---------- $(FLEXBISON_OBJS) -----------)
	$(MAKE) fb

grammar:
	$(ECHO) "############################## Generating... parser"
#	$(CD) "$(BISON_SRC_DIR)" && bison addb_parser.y
	$(ECHO)

lexical:
	$(ECHO) "############################## Generating... scanner"
	$(CD) "$(FLEX_SRC_DIR)" && flex fb1-1.l
	$(ECHO)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(ECHO) "############################## Compiling... $<"
	$(CC) -c $(CFLAGS) -I$(INC_DIR) -o $@ $<  
	$(ECHO)

$(OBJ_DIR)/%.opp: $(SRC_DIR)/%.cpp
	$(ECHO) "############################## Compiling... $<"
	$(CXX) -c $(CXXFLAGS) -I$(INC_DIR) -o $@ $<
	$(ECHO)

flexbison: grammar lexical $(FLEXBISON_OBJS)
	$(ECHO) "############################## Arhive library... $@"
	ar rvs $(LIB_DIR)/flexbison.a $(FLEXBISON_OBJS)

fb: grammar lexical flexbison $(OBJS)
	$(ECHO) "############################## Linking... $@"
	g++ -o $(BIN_DIR)/$@ $(MAIN_SRC) $(OBJS) $(LIB_DIR)/flexbison.a $(CXXFLAGS)
#	$(MAKE) test

test: $(BIN_DIR)/addb
	$(ECHO) "############################## Testing..."
	@$<
	$(ECHO)

test0: $(BIN_DIR)/addb
	$(ECHO) "############################## Testing..."
	@$< $(TEST_DIR)/sample0.gmd
	$(ECHO)

enter: test/$@
	$(ECHO) "############################## Compiling... $@"
	$(CXX) $(CXXFLAGS) test/$@.cpp -o test/$@
	$(ECHO)
	$(ECHO) "############################## Executing... $@"
	@test/$@
	$(ECHO)

.PHONY: clean

clean:
	$(ECHO) "############################## cleaning lexcial output files..."
	$(RM) "$(FLEX_SRC_DIR)/fb1-1.cpp" "$(FLEX_SRC_DIR)/fb1-1.h" "$(FLEX_SRC_DIR)/fb1-1.output"

	$(ECHO) "############################## cleaning grammar output files..."
#	$(RM) "$(BISON_SRC_DIR)/addb_parser.cpp" "$(BISON_SRC_DIR)/addb_parser.h" "$(BISON_SRC_DIR)/addb_parser.output"

	$(ECHO) "############################## cleaning flexbison library files"
	$(RM) $(FLEXBISON_OBJS) $(OBJS) $(LIB_DIR)/flexbison.a $(BIN_DIR)/fb




