COPY_FILES = bochsConfig/bochsrc.txt

SUBDIRS = \
		logger

all: copyConfigFiles

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.defs

copyConfigFiles:
	cp -f $(COPY_FILES) ${ETC_DIR}/
