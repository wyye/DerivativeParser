cmake_minimum_required(VERSION 2.6)
project(parser)

add_executable(parser
	debug.cpp
	main.cpp
	parser.cpp
	tree_node.cpp
)

target_compile_options(parser PUBLIC 
	-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused)

target_compile_options(parser PUBLIC -g -std=c++14)
