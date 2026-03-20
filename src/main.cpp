#include <userinput.hpp>
#include <GetAndPrint.hpp>
#include <init.hpp>
#include <filesystem>
#include <iostream>

GetAndPrint gap;

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
	init();
	while (true) {
		gap.print();
		gap.handleInput(gap.getThingToDo());
	}
	return 0;
}
