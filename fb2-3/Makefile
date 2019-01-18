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

EXEC_NAME = fb
EXEC_TARGET = $(MAKE_DIR)/$(BIN_DIR)/$(EXEC_NAME)


.PHONY: clean test test0 test1 all

all: 
	$(info ##############################)
	$(info # Build all)
	$(info ##############################)
	$(MAKE) $(EXEC_NAME)

parser.cpp: parser.y
	$(ECHO) "############################## Generating... parser"
	$(CD) "$(BISON_SRC_DIR)" && bison $<
	$(ECHO)

scanner.cpp: scanner.l
	$(ECHO) "############################## Generating... scanner"
	$(CD) "$(FLEX_SRC_DIR)" && flex $<
	$(ECHO)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(ECHO) "############################## Compiling... $<"
	$(CC) -c $(CFLAGS) -I$(INC_DIR) -o $@ $<  
	$(ECHO)

$(OBJ_DIR)/%.opp: $(SRC_DIR)/%.cpp
	$(ECHO) "############################## Compiling... $<"
	$(CXX) -c $(CXXFLAGS) -I$(INC_DIR) -o $@ $<
	$(ECHO)

#$(EXEC_NAME): grammar lexical $(FLEXBISON_OBJS) $(OBJS)
$(EXEC_NAME): $(FLEXBISON_OBJS) $(OBJS)	
	$(ECHO) "############################## Linking... $@"
#	$(CC) -o $(BIN_DIR)/$@ $(MAIN_SRC) $(FLEXBISON_OBJS) $(OBJS) $(CFLAGS) -ll
#	$(CXX) -o $(BIN_DIR)/$@ $(MAIN_SRC) $(FLEXBISON_OBJS) $(OBJS) $(CXXFLAGS) -ll
	$(CXX) -o $(BIN_DIR)/$@ $(MAIN_SRC) $(FLEXBISON_OBJS) $(OBJS) $(CXXFLAGS)
#	$(MAKE) test

test: test1

test0: $(MAKE_DIR)/$(BIN_DIR)/$(EXEC_NAME)
	$(ECHO) "############################## Testing without parameter..."
	@$(EXEC_TARGET)
	$(ECHO)

test1: all $(MAKE_DIR)/$(BIN_DIR)/$(EXEC_NAME)
	$(ECHO) "############################## Testing with test.c file..."
	@$(EXEC_TARGET) $(TEST_DIR)/test.c
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
	$(RM) "$(BISON_SRC_DIR)/parser.cpp" "$(BISON_SRC_DIR)/parser.h" "$(BISON_SRC_DIR)/parser.output"

	$(ECHO) "############################## cleaning flexbison library files"
	$(RM) $(FLEXBISON_OBJS) $(OBJS) $(LIB_DIR)/flexbison.a $(BIN_DIR)/$(EXEC_NAME)

	$(ECHO) "############################## cleaning other files"
	$(RM) a.out




