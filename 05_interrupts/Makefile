MKDIR_P := mkdir -p
OUT_DIR := $(PWD)/build
export TOP_DIR=$(PWD)
export LIB_DIR=${OUT_DIR}/lib
export BIN_DIR=${OUT_DIR}/bin
export ETC_DIR=${OUT_DIR}/etc
export LD_LIBRARY_PATH:=${LIB_DIR}:$(LD_LIBRARY_PATH)

SUBDIRS = \
	src

MAKE_UTILS_DIR := $(PWD)/makeUtils
export MAKE_UTILS_DIR

all: $(OUT_DIR)

$(OUT_DIR):
	${MKDIR_P} $(LIB_DIR) $(BIN_DIR) $(ETC_DIR)

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.defs
