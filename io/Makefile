OUT := $(BUILD_DIR)
SRC := $(wildcard *.c)
OBJ := $(patsubst %.c,$(OUT)/%.o,$(SRC))
DEP := $(OBJ:.o=.d)

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
	$(Q)$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEP)
