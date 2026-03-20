#pragma once
#include <string>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

class GetAndPrint {
	std::string lastpath;
	std::vector<std::string> getfilenames();
	int selected = 0;
	fs::path clipboardPath;
	bool hasClipboard = false;
	void renameSelected();
	void deleteSelected();
	void createNew(bool isFolder);
	void printStatusBar(const std::vector<std::string>& filenames);
	void copySelected();
	void pasteClipboard();
public:
	int getThingToDo();
	void handleInput(int input);
	void print();
};