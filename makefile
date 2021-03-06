YEL='\033[0;33m'
GRE='\033[0;32m'
RED='\033[0;31m'
NOC='\033[0m'

ECHO=echo -e 
BLANK=@$(ECHO)

BARRIER=@$(ECHO) ----------------------------------------------------------
MSG_CLEAN=removing old artifacts ...
MSG_SCRIPT=running scriptlet ...
MSG_DONE=$(ESCAPES)$(GRE)$@ done $(NOC)
MSG_COMP=compiling $@ ...
MSG_LINK=linking $@ ...
MSG_INST=intalling $@
MSG_WRAP=generating wrappers ...

.DEFAULT_GOAL := all

# include the selected paths
# for installation
include make_path 


#compiler/linker flags
CPPFLAG=-std=c++0x -Wno-deprecated-declarations -fPIC -c -D$(VER_LIBSSH) 
LNKFLAG=-std=c++0x -fPIC 

# product name
PRODNAME=exppert

# list dependencies here
LIBOBJECTS=main.o confly.o ptable.o

# prepend .o binaries with build_path
LIB_PATH_BUILD_OBJECTS=$(patsubst %, $(PATH_BUILD)/%, $(LIBOBJECTS))

#plugin base
PLUGIN=libconfly_rttg_

all: prep $(PRODNAME)


$(PRODNAME): $(LIBOBJECTS)
	@$(ECHO) $(MSG_LINK)
	@g++ -std=c++0x \
	$(LIB_PATH_BUILD_OBJECTS) \
	-L$(PATH_BUILD) \
	-L$(PATH_LIBR) \
	-L$(PATH_LIBSSH_LIB) \
	-ldl -lR -lgomp -lssh -lssl -lcrypto \
	-o $(PATH_BUILD)/$@
	@$(ECHO) $(MSG_INST)
	@cp -p $(PATH_BUILD)/$@ $(PATH_INSTALL_BIN)
	@$(ECHO) $(MSG_DONE)

main.o: $(PATH_SRC)/main.cpp
	@$(ECHO) $(MSG_COMP)
	@g++ $(CPPFLAG) -O2 \
	-fopenmp \
	-I$(PATH_BOOST) \
	-I$(PATH_R) \
	-I$(PATH_LIBSSH_INC) \
	$< -o $(PATH_BUILD)/$@

confly.o: $(PATH_SRC)/confly.cpp
	@$(ECHO) $(MSG_COMP)
	@g++ $(CPPFLAG) -O2 \
	-fopenmp \
	-I$(PATH_BOOST) \
	-I$(PATH_R) \
	-I$(PATH_LIBSSH_INC) \
	$< -o $(PATH_BUILD)/$@

ptable.o: $(PATH_SRC)/ptable.cpp 
	@$(ECHO) $(MSG_COMP)
	@g++ $(CPPFLAG) -O2 \
	-I$(PATH_BOOST) \
	-I$(PATH_R) \
	$< -o $(PATH_BUILD)/$@


prep:
	@$(ECHO) $(MSG_SCRIPT) ;\
	mkdir -p $(PATH_BUILD) ;\
	mkdir -p $(PATH_INSTALL_LIB) ;\
	mkdir -p $(PATH_INSTALL_BIN) ;\
	mkdir -p $(PATH_INSTALL_INC)

clean:
	@$(ECHO) $(MSG_CLEAN)
	@rm -r -f $(PATH_BUILD)
	@rm -f $(PATH_SRC)/libconfly_rttg_*

clean-output:
	@$(ECHO) $(MSG_CLEAN)
	@rm -r -f -v $(PATH_DATA)/output/*

plugin: $(PATH_SRC)/rttg.cpp $(PATH_SRC)/ptable.cpp
	@g++ $(LNKFLAG) -shared \
	-I$(PATH_BOOST) \
	-I$(PATH_R) \
	$^ \
	-L$(PATH_LIBR) \
	-lR \
	-o $(PATH_BUILD)/$(PLUGIN)$(CFLAGS).so
	@$(ECHO)$(YEL)$(PLUGIN)$(CFLAGS)$(NOC) done

test:
	@$(ECHO) PATH_THIS: $(PATH_THIS)
	@$(ECHO) PATH_SRC: $(PATH_SRC)
	@$(ECHO) PATH_BUILD: $(PATH_BUILD)
	@$(ECHO) PATH_DATA: $(PATH_DATA)
	@$(ECHO) PATH_DATA_OUT: $(RED) $(PATH_DATA)/output/ $(NOC)
	@$(ECHO) PATH_INSTALL_BIN: $(PATH_INSTALL_BIN)
	@$(ECHO) PATH_INSTALL_LIB: $(PATH_INSTALL_LIB)
	@$(ECHO) PATH_INSTALL_INC: $(PATH_INSTALL_INC)
	@$(ECHO) PATH_LIBSSH_INC: $(PATH_LIBSSH_INC)
	@$(ECHO) PATH_LIBSSH_LIB: $(PATH_LIBSSH_LIB)
	@$(ECHO) PATH_BOOST: $(PATH_BOOST)
	@$(ECHO) PATH_R: $(PATH_R)
	@$(ECHO) PATH_LIBR: $(PATH_LIBR)
	@$(ECHO) VER_LIBSSH: $(VER_LIBSSH)

doc-template: 
	@doxygen -s -g

.PHONY: doc
doc:
	@doxygen Doxyfile

