# Nome do executável
TARGET = my_lxc_program

# Fonte
SRC = main.c

# Diretivas do compilador
CC = gcc
CFLAGS = -Wall -I/usr/include/lxc
LDFLAGS = -llxc

# Regra padrão
all: $(TARGET)

# Regras de compilação
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Regra de limpeza
clean:
	rm -f $(TARGET)

.PHONY: all clean