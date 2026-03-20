#pragma once
#include <string>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

struct tab {
	fs::path pathoftab;
	int tabnumber;
	size_t selectedIndex;
	tab() : tabnumber(0), selectedIndex(0) {}
};

class GetAndPrint {
	std::string lastpath;
	std::vector<std::string> getfilenames();
	std::vector<tab> tabs;
	int currenttab = 0;
	int selected = 0;
	fs::path clipboardPath;
	bool hasClipboard = false;
	void renameSelected();
	void deleteSelected();
	void createNew(bool isFolder);
	void printStatusBar(const std::vector<std::string>& filenames);
	void copySelected();
	void pasteClipboard();
	void closeCurrentTab();
	void switchTab(int tabIndex);
public:
	void addTab(const fs::path& p);
	void handleInput();
	void print();
};