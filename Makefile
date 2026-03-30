SHELL := /bin/bash

LIB_NAME  := libstdoc.a
SUBDIRS   := io
BUILD_DIR := $(CURDIR)/build
INC_DIR   := $(CURDIR)/include
TARGET    := $(BUILD_DIR)/$(LIB_NAME)

CC      := gcc
AR      := ar
ARFLAGS := rcs
BUILD   ?= debug

ifeq ($(BUILD),debug)
    CFLAGS  := -std=gnu11 -fPIC -I$(INC_DIR) -g -O0 -Wall -Wextra -Wpedantic -Wstrict-prototypes
    MODETXT := DEBUG
else
    CFLAGS  := -std=gnu11 -fPIC -I$(INC_DIR) -O2 -w
    MODETXT := RELEASE
endif

export CC CFLAGS AR ARFLAGS BUILD BUILD_DIR INC_DIR

V ?= 0
ifeq ($(V),1)
    Q :=
else
    Q := @
endif

define log
	@printf "  %-7s %s\n" "$(1)" "$(2)"
endef

.PHONY: all info subdirs archive clear install uninstall rebuild

all: info subdirs archive

info:
	@echo "Build mode : $(MODETXT)"
	@echo "PCH        : disabled"

subdirs:
	$(Q)for dir in $(SUBDIRS); do \
		printf "  %-7s %s\n" "MAKE" "$$dir"; \
		$(MAKE) -C $$dir V=$(V) --no-print-directory || exit 1; \
	done

archive:
	$(call log,AR,$(LIB_NAME))
	@mkdir -p $(BUILD_DIR)
	$(Q)$(AR) $(ARFLAGS) $(TARGET) $(shell find $(BUILD_DIR) -name '*.o' 2>/dev/null)


install: all
	@echo "Installing stdoc to $(PREFIX)..."
	@mkdir -p $(PREFIX)/lib $(PREFIX)/include/stdoc
	$(Q)cp $(TARGET) $(PREFIX)/lib/
	$(Q)cp include/stdoc.h $(PREFIX)/include/
	$(Q)cp include/stdoc/*.h $(PREFIX)/include/stdoc/ 2>/dev/null || true
	@echo "Installation done."

uninstall:
	$(Q)rm -f $(PREFIX)/lib/$(LIB_NAME)
	$(Q)rm -f $(PREFIX)/include/stdoc.h
	$(Q)rm -rf $(PREFIX)/include/stdoc
	@echo "uninstalling stdoc done. Run git pull && make if you want update the library."

clear:
	$(call log,CLEAR,build/)
	$(Q)rm -rf $(BUILD_DIR)

rebuild: clear all
