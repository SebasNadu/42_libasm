NAME = libasm.a
TEST_NAME = test_bin

AS = nasm
CC = cc

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
    ASFLAGS = -f elf64
    CFLAGS = -Wall -Wextra -Werror
else
    ASFLAGS = -f macho64
    UNAME_M := $(shell uname -m)
    ifeq ($(UNAME_M), arm64)
        CFLAGS = -Wall -Wextra -Werror -arch x86_64
    else
        CFLAGS = -Wall -Wextra -Werror
    endif
endif

SRCS = src/ft_strlen.s \
       src/ft_strcpy.s \
       src/ft_strcmp.s \
       src/ft_write.s \
       src/ft_read.s \
       src/ft_strdup.s

BONUS_SRCS = src_bonus/ft_list_push_front_bonus.s \
             src_bonus/ft_list_size_bonus.s \
             src_bonus/ft_list_sort_bonus.s \
             src_bonus/ft_list_remove_if_bonus.s \
             src_bonus/ft_atoi_base_bonus.s

OBJ_DIR = obj
OBJS = $(patsubst src/%.s, $(OBJ_DIR)/%.o, $(SRCS))
BONUS_OBJS = $(patsubst src_bonus/%_bonus.s, $(OBJ_DIR)/%_bonus.o, $(BONUS_SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Creating library $(NAME)..."
	ar rcs $(NAME) $(OBJS)

$(OBJ_DIR)/%.o: src/%.s | $(OBJ_DIR)
	@echo "Assembling $<..."
	$(AS) $(ASFLAGS) $< -o $@

$(OBJ_DIR)/%_bonus.o: src_bonus/%_bonus.s | $(OBJ_DIR)
	@echo "Assembling $<..."
	$(AS) $(ASFLAGS) $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

bonus: $(OBJS) $(BONUS_OBJS)
	@echo "Creating library $(NAME) with bonus..."
	ar rcs $(NAME) $(OBJS) $(BONUS_OBJS)

test: $(TEST_NAME)
	@echo "Executing test suite..."
	@./$(TEST_NAME)

$(TEST_NAME): bonus main.c
	@echo "Compiling test binary $(TEST_NAME)..."
	$(CC) $(CFLAGS) main.c $(NAME) -o $(TEST_NAME)

clean:
	@echo "Removing object files..."
	@rm -rf $(OBJ_DIR)
	@rm -f src/*.o src_bonus/*.o

fclean: clean
	@echo "Removing library and binaries..."
	@rm -f $(NAME) $(TEST_NAME) test

re: fclean all

.PHONY: all clean fclean re test bonus
