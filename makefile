.SUFFIXES:
.PHONY: x, clean

x: main_exe
	./$<

main_exe: main.cpp enums_tool.h
	clang++ -std=c++11 -Wall -Wextra  -I. $< -o $@

clean:
	rm -rf *_exe
