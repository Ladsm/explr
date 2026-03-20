#include <userinput.hpp>
#include <GetAndPrint.hpp>
#include <init.hpp>
#include <filesystem>
#include <iostream>

GetAndPrint gap;

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
	init();
	gap.addTab(fs::current_path());
	while (true) {
		gap.print();
		gap.handleInput();
	}
	return 0;
}
