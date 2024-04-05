# Имя компилятора
CC = gcc

# Флаги компиляции
CFLAGS = -Wall -Wextra -g

# Библиотеки, если они используются
LIBS = -lcurl

# Исходные файлы для сервера и клиента
SERVER_SRC = server/server.c common/common.c
CLIENT_SRC = client/client.c common/common.c

# Объектные файлы для сервера и клиента
SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

# Имя исполняемого файла сервера и клиента
SERVER_EXEC = server
CLIENT_EXEC = client

# Правило для сборки всего проекта
all: $(SERVER_EXEC) $(CLIENT_EXEC)

# Правило для сборки сервера
$(SERVER_EXEC): $(SERVER_OBJ)
	$(CC) $(CFLAGS) $(SERVER_OBJ) -o $(SERVER_EXEC) $(LIBS)

# Правило для сборки клиента
$(CLIENT_EXEC): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) $(CLIENT_OBJ) -o $(CLIENT_EXEC) $(LIBS)

# Правило для компиляции исходных файлов сервера и клиента
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Правило для очистки сгенерированных файлов
clean:
	rm -f $(SERVER_OBJ) $(CLIENT_OBJ) $(SERVER_EXEC) $(CLIENT_EXEC)
