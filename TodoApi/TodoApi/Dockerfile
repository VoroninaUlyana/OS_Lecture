FROM gcc:latest

# Устанавливаем зависимости
RUN apt-get update && apt-get install -y \
    libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Копируем всё содержимое папки в контейнер
COPY . .

# ЗАМЕНЯЕМ main.cpp на TodoApi.cpp
RUN g++ -O3 TodoApi.cpp -I. -lsqlite3 -lpthread -o todo_monster

EXPOSE 18080

CMD ["./todo_monster"]