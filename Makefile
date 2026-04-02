ifeq ($(OS),Windows_NT)
    PLATFORM := Windows
    RM       := del /Q /F
    MKDIR    := mkdir
    INSTALL  := cp
    EXT      := .exe
    LIB_EXT  := .a
    IS_FILE  := if exist
else
    PLATFORM := $(shell uname -s)
    RM       := rm -rf
    MKDIR    := mkdir -p
    INSTALL  := install
    LIB_EXT  := .a
    IS_FILE  := test -f

    # Install prefix (overrideable)
    PREFIX  ?= /usr/local

    # Staging directory (for packaging)
    DESTDIR ?=

    # Optional privilege escalation
    SUDO ?=
endif

LIB_NAME  := libstdoc$(LIB_EXT)
SUBDIRS   := io
INC_DIR   := $(CURDIR)/include
BUILD_DIR := $(CURDIR)/build
TARGET    := $(BUILD_DIR)/$(LIB_NAME)

CC      ?= gcc
AR      ?= ar
ARFLAGS := rcs

BUILD ?= debug
ifeq ($(BUILD),debug)
    CFLAGS  := -std=c11 -fPIC -I$(INC_DIR) -g -O0 -Wall -Wextra -Wpedantic -Wstrict-prototypes
    MODETXT := DEBUG
else
    CFLAGS  := -std=c11 -fPIC -I$(INC_DIR) -O2 -w
    MODETXT := RELEASE
endif

export CC CFLAGS AR ARFLAGS BUILD BUILD_DIR INC_DIR RM MKDIR

V ?= 0
ifeq ($(V),1)
    Q :=
else
    Q := @
endif

define log
    @printf "  %-10s %s\n" "$(1)" "$(2)"
endef

.PHONY: all info subdirs install uninstall clear rebuild

all: info $(TARGET)

info:
	@echo "OS detected: $(PLATFORM)"
	@echo "Build mode : $(MODETXT)"
	@echo "Prefix     : $(PREFIX)"
	@echo "Destdir    : $(DESTDIR)"

$(TARGET): subdirs
	$(call log,AR,$(LIB_NAME))
	@$(MKDIR) $(BUILD_DIR)
	$(Q)$(AR) $(ARFLAGS) $(TARGET) $(shell find $(BUILD_DIR) -name '*.o' 2>/dev/null)

subdirs:
	$(Q)for dir in $(SUBDIRS); do \
		printf "  %-10s %s\n" "MAKE" "$$dir"; \
		$(MAKE) -C $$dir V=$(V) --no-print-directory || exit 1; \
	done

install:
	@$(IS_FILE) "$(TARGET)" || (echo "Error: $(LIB_NAME) not built. Run 'make' first." && exit 1)
	$(call log,INSTALL,$(DESTDIR)$(PREFIX))

ifeq ($(PLATFORM),Windows)
	@if not exist "$(PREFIX)\lib" $(MKDIR) "$(PREFIX)\lib"
	@if not exist "$(PREFIX)\include" $(MKDIR) "$(PREFIX)\include"
	$(Q)copy /Y "$(subst /,\,$(TARGET))" "$(subst /,\,$(PREFIX)\lib\)"
	$(Q)copy /Y "$(subst /,\,$(INC_DIR)\stdoc.h)" "$(subst /,\,$(PREFIX)\include\)"
	$(Q)xcopy /E /I /Y "$(subst /,\,$(INC_DIR)\stdoc)" "$(subst /,\,$(PREFIX)\include\stdoc)"
else
	@$(SUDO) $(INSTALL) -d $(DESTDIR)$(PREFIX)/lib
	@$(SUDO) $(INSTALL) -d $(DESTDIR)$(PREFIX)/include

	@if [ -f $(DESTDIR)$(PREFIX)/include/stdoc.h ]; then \
		echo "[WARNING] stdoc.h already exists, overwriting..."; \
	fi

	$(Q)$(SUDO) $(INSTALL) -m 644 $(TARGET) $(DESTDIR)$(PREFIX)/lib/
	$(Q)$(SUDO) $(INSTALL) -m 644 $(INC_DIR)/stdoc.h $(DESTDIR)$(PREFIX)/include/
	$(Q)$(SUDO) cp -r $(INC_DIR)/stdoc $(DESTDIR)$(PREFIX)/include/
endif
	@echo "Installation complete on $(PLATFORM)."

uninstall:
	$(call log,UNINSTALL,$(DESTDIR)$(PREFIX))

ifeq ($(PLATFORM),Windows)
	$(Q)$(RM) $(PREFIX)\lib\$(LIB_NAME)
	$(Q)$(RM) $(PREFIX)\include\stdoc.h
	$(Q)$(RM) $(PREFIX)\include\stdoc
else
	$(Q)$(SUDO) rm -f  $(DESTDIR)$(PREFIX)/lib/$(LIB_NAME)
	$(Q)$(SUDO) rm -f  $(DESTDIR)$(PREFIX)/include/stdoc.h
	$(Q)$(SUDO) rm -rf $(DESTDIR)$(PREFIX)/include/stdoc
endif
	@echo "Uninstall complete."

clear:
	$(call log,CLEAR,$(BUILD_DIR))
	$(Q)$(RM) $(BUILD_DIR)

rebuild: clear all
