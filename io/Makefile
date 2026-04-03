OUT := $(BUILD_DIR)
SRC := $(wildcard *.c)
OBJ := $(patsubst %.c,$(OUT)/%.o,$(SRC))
DEP := $(OBJ:.o=.d)

BUILD_HASH := $(shell git rev-parse HEAD)

V ?= 0
ifeq ($(V),1)
    Q :=
else
    Q := @
endif

.PHONY: all

all: $(OBJ)

$(OUT)/%.o: %.c
	@$(MKDIR) $(OUT)
	@printf "  %-7s %s\n" "CC" "$<"
	$(Q)$(CC) $(CFLAGS) -DSTDOC_BUILD_HASH=\"$(BUILD_HASH)\" -MMD -MP -c $< -o $@

-include $(DEP)
