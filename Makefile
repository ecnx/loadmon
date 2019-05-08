# Portable Load Monitor Makefile
INCLUDES=-I include
INDENT_FLAGS=-br -ce -i4 -bl -bli0 -bls -c4 -cdw -ci4 -cs -nbfda -l100 -lp -prs -nlp -nut -nbfde -npsl -nss

OBJS = \
	release/main.o

all: host

internal: prepare
	@echo "  CC    src/main.c"
	@$(CC) $(CFLAGS) $(INCLUDES) src/main.c -o release/main.o
	@echo "  LD    release/loadmon"
	@$(LD) -o release/loadmon $(OBJS) $(LDFLAGS)

prepare:
	@mkdir -p release

host:
	@make internal \
		CC=gcc \
		LD=gcc \
		CFLAGS='-c -Wall -Wextra -Os -ffunction-sections -fdata-sections' \
		LDFLAGS='-s -Wl,--gc-sections -Wl,--relax'

install:
	@cp -v release/loadmon /usr/bin/loadmon

uninstall:
	@rm -vf /usr/bin/loadmon

indent:
	@indent $(INDENT_FLAGS) ./*/*.h
	@indent $(INDENT_FLAGS) ./*/*.c
	@rm -rf ./*/*~

clean:
	@echo "  CLEAN ."
	@rm -rf release

analysis:
	@scan-build make
	@cppcheck --force */*.h
	@cppcheck --force */*.c
