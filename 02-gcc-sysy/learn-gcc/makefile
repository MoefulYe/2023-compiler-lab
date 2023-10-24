CC := gcc
NAME := sample
MACRO := NEG

pre:
	@$(CC) -E src/$(NAME).c -o target/$(NAME).i

pre-define-macro:
	@$(CC) -E -D $(MACRO) src/$(NAME).c -o target/$(NAME).i

asm:
	@$(CC) -S -m$(M) src/$(NAME).c -o target/$(NAME).s

obj: target/$(NAME).o

target/$(NAME).o:
	@$(CC) -c src/$(NAME).c -o target/$(NAME).o

disasm: target/$(NAME).o
	@objdump -dS target/$(NAME).o

print-symbol: target/$(NAME).o
	@nm target/$(NAME).o

build: target/$(NAME).o
	@ld /usr/lib/crt1.o /usr/lib/crti.o /usr/lib/crtn.o target/$(NAME).o -lc -o target/$(NAME)

clean:
	@rm target/*
