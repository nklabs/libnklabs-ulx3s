# Name of this project, final target

NAME := ulx3s

all: fpga.bin $(NAME)_impl1.mcs $(NAME).bin

# Where should we put object and dependency files?
# This directory is deleted by "make clean"
OBJ_DIR := obj/

# Get application version number

NK_VERSION_MAJOR := $(shell cat VERSION_MAJOR)
NK_VERSION_MINOR := $(shell cat VERSION_MINOR)

# Get date/time

NK_DATE := $(shell date -u -Iminute)
NK_YEAR := $(shell expr $(shell echo $(NK_DATE) | cut -b 1-4) + 0)
NK_MONTH := $(shell expr $(shell echo $(NK_DATE) | cut -b 6-7) + 0)
NK_DAY := $(shell expr $(shell echo $(NK_DATE) | cut -b 9-10) + 0)
NK_HOUR := $(shell expr $(shell echo $(NK_DATE) | cut -b 12-13) + 0)
NK_MINUTE := $(shell expr $(shell echo $(NK_DATE) | cut -b 15-16) + 0)

# A define for the platform
NK_PLATFORM := NK_PLATFORM_RISCV

# Get git hash as a string
# It is postfixed with -dirty if there are uncommitted changed; otherwise, it is postfixed with -clean.
NK_GIT_REV := \"$(shell git rev-parse HEAD)-$(shell if git diff-index --quiet HEAD --; then echo clean; else echo dirty; fi)\"

# Path to Lattice Diamond: usually diamond environement setup will have this
FOUNDRY ?= /opt/lattice/radiant/2.2/ispfpga

# GCC bin directory
TOOL_DIR := /opt/riscv32imc/bin/

CC := $(TOOL_DIR)riscv32-unknown-elf-gcc
CPP := $(TOOL_DIR)riscv32-unknown-elf-g++
AS := $(TOOL_DIR)riscv32-unknown-elf-as
LD := $(TOOL_DIR)riscv32-unknown-elf-ld
OBJCOPY := $(TOOL_DIR)riscv32-unknown-elf-objcopy
OBJDUMP := $(TOOL_DIR)riscv32-unknown-elf-objdump

CFLAGS := -I libnklabs/inc -I config -I . -O -march=rv32imc -Wall -Wconversion -Wunused -Wwrite-strings \
 -Wstrict-overflow=4 -Wmissing-include-dirs -Winit-self -Wcast-qual \
 -Wundef -Wlogical-op -Wmissing-declarations -Wformat \
 -Wmissing-format-attribute -Wformat-nonliteral \
 -Wswitch-enum -Wshadow -Wextra -Wformat-security \
 -D$(NK_PLATFORM) -DNK_PLATFORM=\"$(NK_PLATFORM)\" -DNK_VERSION_MAJOR=$(NK_VERSION_MAJOR)  -DNK_VERSION_MINOR=$(NK_VERSION_MINOR) -DNK_YEAR=$(NK_YEAR) -DNK_MONTH=$(NK_MONTH) -DNK_DAY=$(NK_DAY) -DNK_HOUR=$(NK_HOUR) -DNK_MINUTE=$(NK_MINUTE) -DNK_GIT_REV=$(NK_GIT_REV)

# -ffreestanding

LDFLAGS := $(MCU) -specs=nano.specs -Tsections.lds $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

OBJS := \
  basic_cmds.o \
  database.o \
  fpga.o \
  info_cmd.o \
  libnklabs/src/nkchecked.o \
  libnklabs/src/nkcli.o \
  libnklabs/src/nkcrclib.o \
  libnklabs/src/nkdbase.o \
  libnklabs/src/nkdirect.o \
  libnklabs/src/nkinfile.o \
  libnklabs/src/nkmcuflash.o \
  libnklabs/src/nkoutfile.o \
  libnklabs/src/nkprintf.o \
  libnklabs/src/nkreadline.o \
  libnklabs/src/nkscan.o \
  libnklabs/src/nksched.o \
  libnklabs/src/nkserialize.o \
  libnklabs/src/nkstring.o \
  libnklabs/src/nkymodem.o \
  main.o \
  nkarch_riscv.o \
  nkuart_riscv.o \
  nkymodem_cmd.o \
  start.o \
  tunnel_fpga.o \

# Keep them in a subdirectory
MOST_OBJS := $(addprefix $(OBJ_DIR), $(OBJS))

SUBDIR_OBJS := $(MOST_OBJS) $(OBJ_DIR)version.o

# Rebuild version.o if any other file changed
$(OBJ_DIR)version.o: $(MOST_OBJS) VERSION_MAJOR VERSION_MINOR

# Use Diamond "deployment tool" to combine FPGA configuration file with firmware
# Note that byte mirroring must be enabled for the firmware.
$(NAME)_impl1.mcs: ../impl1/$(NAME)_impl1.bit $(NAME).bin
	$(FOUNDRY)/../bin/lin64/ddtcmd -oft -advanced -dev LFE5U-85F \
		-if ../impl1/$(NAME)_impl1.bit -format int -flashsize 128 \
		-usermirror -userdata 2 \
		-userfile $(NAME).bin -address 0x00E00000 \
		-userfile $(NAME).bin -address 0x00600000 \
		-of $(NAME)_impl1.mcs
	cp $(NAME)_impl1.mcs ../impl1/$(NAME)_impl1.mcs

# create fpga.bin from .bit file

fpga.bin: ../impl1/$(NAME)_impl1.bit ./exotobin ./brev
	/bin/rm -f fpga.exo
	$(FOUNDRY)/../bin/lin64/ddtcmd -oft -mot -dev LFE5U-85F -if ../impl1/$(NAME)_impl1.bit -of fpga.exo
	$(FOUNDRY)/../bin/lin64/ddtcmd -oft -int -dev LFE5U-85F -if ../impl1/$(NAME)_impl1.bit -of fpga.mcs
	./exotobin fpga.exo -o fpga.rbin
	./brev <fpga.rbin >fpga.bin

# .exo file to .bin file converter

exotobin: exotobin.c
	cc -o exotobin exotobin.c

# utility to reverse bits in each byte

brev: brev.c
	cc -o brev brev.c

$(NAME).elf: sections.lds $(SUBDIR_OBJS)
	$(LD) -Bstatic -T sections.lds --strip-debug -o $(NAME).elf -Map=$(NAME).map --cref $(SUBDIR_OBJS) $(TOOL_DIR)../lib/gcc/riscv32-unknown-elf/8.2.0/libgcc.a

# Word hex file for bus_rom.v
$(NAME).mem: $(NAME).bin bintov
	./bintov $(NAME).bin $(NAME).mem

# Byte hex file for simulation
$(NAME).hex: $(NAME).bin bintob
	./bintob $(NAME).bin $(NAME).hex

bintov: bintov.c
	cc -o bintov bintov.c

bintob: bintob.c
	cc -o bintob bintob.c

$(NAME).bin: $(NAME).elf
	$(OBJCOPY) -O binary $(NAME).elf $(NAME).bin

# Commmands / phony targets

.PHONY: help
help:
	@echo
	@echo "make                     Build $(NAME).bin"
	@echo
	@echo "make clean               Delete intermediate files"
	@echo
	@echo "make cleaner             Delete intermediate files and final image"
	@echo

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)

.PHONY: cleaner
cleaner: clean
	rm -f $(NAME).elf $(NAME).hex $(NAME).bin $(NAME).map

# Bump version numbers...

.PHONY: bump_minor
bump_minor:
	@expr `cat VERSION_MINOR` + 1 >VERSION_MINOR
	@echo
	@echo New version V`cat VERSION_MAJOR`R`cat VERSION_MINOR`
	@echo

.PHONY: bump_major
bump_major:
	@expr `cat VERSION_MAJOR` + 1 >VERSION_MAJOR
	@echo 0 > VERSION_MINOR
	@echo
	@echo New version V`cat VERSION_MAJOR`R`cat VERSION_MINOR`
	@echo

# include dependancy files if they exist
-include $(SUBDIR_OBJS:.o=.d)

# Compile and generate dependency info, C files
$(OBJ_DIR)%.o: %.c
	@echo
	@mkdir -p $(OBJ_DIR)$(shell dirname $*)
	$(CC) -c $(CFLAGS) -MT $@ -MMD -MP -MF $(OBJ_DIR)$*.d $*.c -o $(OBJ_DIR)$*.o

# Compile and generate dependency info, CPP files
$(OBJ_DIR)%.o: %.cpp
	@echo
	@mkdir -p $(OBJ_DIR)$(shell dirname $*)
	$(CPP) -c $(CFLAGS) -MT $@ -MMD -MP -MF $(OBJ_DIR)$*.d $*.cpp -o $(OBJ_DIR)$*.o

# Assemble
$(OBJ_DIR)%.o: %.S
	@echo
	@mkdir -p $(OBJ_DIR)$(shell dirname $*)
	$(CC) -c $(CFLAGS) -MT $@ -MMD -MP -MF $(OBJ_DIR)$*.d $*.S -o $(OBJ_DIR)$*.o
