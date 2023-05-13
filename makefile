.PHONY: dev
dev:
	mkdir -p build && gcc -o build/my_ftp src/*.c -lpthread && cd build && ./my_ftp $(port) $(root)

.PHONY: build
build:
	mkdir -p build && gcc -o build/my_ftp src/*.c
