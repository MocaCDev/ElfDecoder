.PHONY: build
.PHONY: bin/elf_header.o
.PHONY: clean_elf_header
.PHONY: bin/elf_sections.o
.PHONY: clean_elf_sections
.PHONY: bin/elf_segments.o
.PHONY: clean_elf_segments
.PHONY: bin/elf_program_header.o
.PHONY: clean_elf_program_header
.PHONY: clean_elf_bin_data
.PHONY: bin/elf_bin_data.o
.PHONY: clean
.PHONY: run

CC = g++
FLAGS = -std=c++20 -fsanitize=leak
elf_bin=main.o

build: bin/elf_program_header.o bin/elf_bin_data.o
	$(CC) $(FLAGS) main.cpp -o bin/main.o bin/program_header.o

run: build
	./bin/main.o $(elf_bin)

clean_elf_header:
	rm -rf bin/elf_header.o

bin/elf_header.o: clean_elf_header
	$(CC) $(FLAGS) -I include/ -c src/elf_header.cpp -o bin/elf_header.o

clean_elf_program_header:
	rm -rf bin/elf_program_header.o
	rm -rf bin/program_header.o

bin/elf_program_header.o: bin/elf_header.o
	$(CC) $(FLAGS) -I include/ -c src/elf_program_header.cpp -o bin/elf_program_header.o
	ld -relocatable bin/elf_header.o bin/elf_program_header.o -o bin/program_header.o

clean_elf_sections:
	rm -rf bin/elf_sections.o

bin/elf_sections.o: clean_elf_sections
	$(CC) $(FLAGS) -I include/ -c src/elf_sections.cpp -o bin/elf_sections.o

clean_elf_segments:
	rm -rf bin/elf_segments.o
	rm -rf bin/segments.o

bin/elf_segments.o: clean_elf_segments bin/elf_sections.o
	$(CC) $(FLAGS) -I include/ -c src/elf_segments.cpp -o bin/elf_segments.o
	ld -relocatable bin/elf_sections.o bin/elf_segments.o -o bin/segments.o

clean_elf_bin_data:
	rm -rf bin/elf_bin_data.o
	rm -rf bin/elf_data.o

bin/elf_bin_data.o: bin/elf_segments.o
	$(CC) $(FLAGS) -I include/ -c src/elf_data.cpp -o bin/elf_bin_data.o
	ld -relocatable bin/segments.o bin/elf_bin_data.o -o bin/elf_data.o

clean:
	rm -rf bin/*.o