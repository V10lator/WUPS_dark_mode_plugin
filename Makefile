#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPRO")
endif

TOPDIR ?= $(CURDIR)

include $(DEVKITPRO)/wups/share/wups_rules

WUT_ROOT := $(DEVKITPRO)/wut
#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET		:=	$(notdir $(CURDIR))
BUILD		:= 	build
SOURCES		:=	src
DATA		:=	data
INCLUDES	:=	$(SOURCES)

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
COMMON_CFLAGS	:= -Ofast -Wall -ffunction-sections \
			-Wl,-q  $(MACHDEP) $(INCLUDE) -D__WIIU__ \
			-D__WUT__ -D__WUPS__ -DFD_SETSIZE=32

# -x c: compile as c code
# -std=c11: use the c11 standard
CFLAGS	  		:= $(COMMON_CFLAGS) -x c -std=gnu11

# -x c: compile as c++ code
# -std=gnu++11: use the c++11 standard
CXXFLAGS  		:= $(COMMON_CFLAGS) -x c++ -std=gnu++11

ifeq ($(DO_LOGGING), 1)
   CFLAGS += -D__LOGGING__
   CXXFLAGS += -D__LOGGING__
endif

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	:=	-g $(ARCH) $(RPXSPECS) -Wl,-Map,$(notdir $*.map) $(WUPSSPECS)

LIBS	:= -lwups -lwut

Q := @
MAKEFLAGS += --no-print-directory

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS) $(WUPSDIR) $(WUPS_ROOT) $(WUT_ROOT)

#-------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#-------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#-------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#-------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#-------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#-------------------------------------------------------------------------------
	export LD	:=	$(CC)
#-------------------------------------------------------------------------------
else
#-------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#-------------------------------------------------------------------------------
endif
#-------------------------------------------------------------------------------

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SRC	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES 	:=	$(OFILES_BIN) $(OFILES_SRC)
export HFILES_BIN	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
 			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: release debug $(BUILD) clean

#---------------------------------------------------------------------------------
debug:		MAKE_CMD	:=	debug
debug:		$(BUILD)

#-------------------------------------------------------------------------------
release:	MAKE_CMD	:=	all
release:	$(BUILD)

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile $(MAKE_CMD)
	
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).wps $(TARGET).elf

#---------------------------------------------------------------------------------
else
.PHONY: all debug

DEPENDS	:=	$(OFILES:.o=.d)

THIS_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#-------------------------------------------------------------------------------
# main targets
#-------------------------------------------------------------------------------
all : $(OUTPUT).wps

debug : CFLAGS += -D__LOGGING__
#debug : DO_LOGGING := 1
debug : all

$(OUTPUT).wps	: $(OUTPUT).elf
$(OUTPUT).elf	: $(OFILES)
  
#-------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#-------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#-------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#-------------------------------------------------------------------------------
endif
#-------------------------------------------------------------------------------
