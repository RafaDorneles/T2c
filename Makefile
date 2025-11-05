# Nome do executável final
TARGET = mymemory

# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude

# Diretórios
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

# Lista de arquivos fonte e objetos
SRCS = $(SRC_DIR)/mymemory.c $(SRC_DIR)/main.c
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Regra padrão: compilar tudo
all: $(TARGET)

# Como gerar o executável
$(TARGET): $(OBJS)
	@echo "🔧 Linking..."
	$(CC) $(OBJS) -o $(TARGET)

# Como compilar cada arquivo .c em .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "🧩 Compiling $< ..."
	$(CC) $(CFLAGS) -c $< -o $@

# Criar o diretório de objetos se não existir
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Executar o programa
run: $(TARGET)
	@echo "🚀 Running program..."
	./$(TARGET)

# Limpar arquivos temporários
clean:
	@echo "🧹 Cleaning project..."
	rm -rf $(OBJ_DIR) $(TARGET)
