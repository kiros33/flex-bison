CC = gcc
CFLAGS += -Wno-deprecated-register
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

_FLEXBISON_OBJS = scanner.opp
FLEXBISON_OBJS = $(patsubst %,$(OBJ_DIR)/%,$(_FLEXBISON_OBJS))
_OBJS = 
OBJS = $(patsubst %,$(OBJ_DIR)/%,$(_OBJS))

.PHONY: clean lexical grammar

all: lexical grammar
	$(info ##############################)
	$(info # Build all)
	$(info ##############################)
	$(MAKE) fb

grammar: 
	$(ECHO) "############################## Generating... parser"
#	$(CD) "$(BISON_SRC_DIR)" && bison parser.y
	$(ECHO)

lexical: 
	$(ECHO) "############################## Generating... scanner"
	$(CD) "$(FLEX_SRC_DIR)" && flex scanner.l
	$(ECHO)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(ECHO) "############################## Compiling... $<"
	$(CC) -c $(CFLAGS) -I$(INC_DIR) -o $@ $<  
	$(ECHO)

$(OBJ_DIR)/%.opp: $(SRC_DIR)/%.cpp
	$(ECHO) "############################## Compiling... $<"
	$(CXX) -c $(CXXFLAGS) -I$(INC_DIR) -o $@ $<
	$(ECHO)

#fb: grammar lexical $(FLEXBISON_OBJS) $(OBJS)
fb: lexical $(FLEXBISON_OBJS) $(OBJS)	
	$(ECHO) "############################## Linking... $@"
#	$(CC) -o $(BIN_DIR)/$@ $(MAIN_SRC) $(FLEXBISON_OBJS) $(OBJS) $(CFLAGS) -ll
#	$(CXX) -o $(BIN_DIR)/$@ $(MAIN_SRC) $(FLEXBISON_OBJS) $(OBJS) $(CXXFLAGS) -ll
	$(CXX) -o $(BIN_DIR)/$@ $(MAIN_SRC) $(FLEXBISON_OBJS) $(OBJS) $(CXXFLAGS)
#	$(MAKE) test

test: $(MAKE_DIR)/$(BIN_DIR)/fb
	$(ECHO) "############################## Testing..."
	@$<
	$(ECHO)

test0: $(MAKE_DIR)/$(BIN_DIR)/fb
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

clean:
	$(ECHO) "############################## cleaning lexcial output files..."
	$(RM) "$(FLEX_SRC_DIR)/scanner.cpp" "$(FLEX_SRC_DIR)/scanner.h" "$(FLEX_SRC_DIR)/scanner.output"

	$(ECHO) "############################## cleaning grammar output files..."
#	$(RM) "$(BISON_SRC_DIR)/addb_parser.cpp" "$(BISON_SRC_DIR)/addb_parser.h" "$(BISON_SRC_DIR)/addb_parser.output"

	$(ECHO) "############################## cleaning flexbison library files"
	$(RM) $(FLEXBISON_OBJS) $(OBJS) $(LIB_DIR)/flexbison.a $(BIN_DIR)/fb

	$(ECHO) "############################## cleaning other files"
	$(RM) a.out




