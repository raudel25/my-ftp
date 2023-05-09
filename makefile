.PHONY: dev
dev:
	mkdir -p build && gcc -o build/my_ftp src/*.c && cd build && ./my_ftp

.PHONY: build
build:
	mkdir -p build && gcc -o build/my_ftp src/*.c
