.PHONY: dev
dev:
	mkdir -p build && gcc -o build/my_ftp src/main.c src/render.c src/server.c && cd build && ./my_ftp

.PHONY: build
build:
	mkdir -p build && gcc -o build/my_ftp src/main.c src/render.c src/server.c
