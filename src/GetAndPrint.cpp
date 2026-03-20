#include <userinput.hpp>
#include <GetAndPrint.hpp>
#include <getwh.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <set>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
std::vector<std::string> GetAndPrint::getfilenames() {
	fs::path p = fs::current_path();
	std::vector<std::string> files;
	if (fs::exists(p) && fs::is_directory(p)) {
		for (const auto& entry : fs::directory_iterator(p)) {
			if (entry.is_regular_file() || entry.is_directory()) {
				files.push_back(entry.path().filename().string());
			}
		}
	}
	std::sort(files.begin(), files.end(), [](const std::string& a, const std::string& b) { return a < b; });
	std::stable_partition(files.begin(), files.end(), [](const std::string& s) { return fs::is_directory(fs::current_path() / s); });
	return files;
}
void GetAndPrint::copySelected(bool show) {
	size_t& selected = tabs[currenttab].selectedIndex;
	auto filenames = getfilenames();
	filenames.insert(filenames.begin(), "..");
	if (selected >= filenames.size() || filenames[selected] == "..") return;
	clipboardPath = fs::current_path() / filenames[selected];
	hasClipboard = true;
	if (show) {
		std::cout << "\033[H\033[J";
		std::cout << "Copied: " << clipboardPath.filename().string() << "\nPress any key...";
		readKey();
	}
}
void GetAndPrint::pasteClipboard() {
	if (!hasClipboard || !fs::exists(clipboardPath)) return;
	fs::path destination = fs::current_path() / clipboardPath.filename();
	try {
		if (fs::is_directory(clipboardPath)) {
			fs::copy(clipboardPath, destination,
				fs::copy_options::recursive | fs::copy_options::overwrite_existing);
		}
		else {
			fs::copy_file(clipboardPath, destination,
				fs::copy_options::overwrite_existing);
		}
	}
	catch (const fs::filesystem_error& e) {
		std::cout << "Error: " << e.what() << "\nPress any key...";
		readKey();
	}
}
void GetAndPrint::closeCurrentTab() {
	if (tabs.size() <= 1) {
		std::cout << "\033[H\033[J";
		std::cout << "Cannot close the last remaining tab!" << std::endl;
		std::cout << "Press any key to continue...";
		readKey();
		return;
	}
	std::cout << "\033[H\033[J";
	tabs.erase(tabs.begin() + currenttab);
	if (currenttab >= (int)tabs.size()) {
		currenttab = (int)tabs.size() - 1;
	}
	fs::current_path(tabs[currenttab].pathoftab);
}

void GetAndPrint::switchTab(int tabIndex) {
	std::cout << "\033[H\033[J";
	if (tabIndex < 0 || tabIndex >= (int)tabs.size()) return;
	currenttab = tabIndex;
	fs::current_path(tabs[currenttab].pathoftab);
}
void GetAndPrint::addTab(const fs::path& p) {
	tab t;
	t.pathoftab = p;
	t.selectedIndex = 0;
	tabs.push_back(t);
	currenttab = (int)tabs.size() - 1;
	fs::current_path(p);
}
void GetAndPrint::renameSelected() {
	size_t& selected = tabs[currenttab].selectedIndex;
	std::vector<std::string> filenames = getfilenames();
	filenames.insert(filenames.begin(), "..");
	if (selected >= filenames.size() || filenames[selected] == "..") return;
	fs::path oldPath = fs::current_path() / filenames[selected];
	std::cout << "\033[H\033[J";
	std::cout << "Rename [" << filenames[selected] << "] to: ";
	std::string newName;
	std::getline(std::cin, newName);
	if (!newName.empty()) {
		try {
			fs::path newPath = oldPath.parent_path() / newName;
			fs::rename(oldPath, newPath);
		}
		catch (const fs::filesystem_error& e) {
			std::cout << "Error: " << e.what() << "\nPress any key...";
			readKey();
		}
	}
}
void GetAndPrint::deleteSelected() {
	size_t& selected = tabs[currenttab].selectedIndex;
	auto filenames = getfilenames();
	filenames.insert(filenames.begin(), "..");
	if (selected >= filenames.size() || filenames[selected] == "..") {
		return;
	}
	fs::path target = fs::current_path() / filenames[selected];
	std::cout << "\033[H\033[J";
	std::cout << "ARE YOU SURE? Delete: " << filenames[selected] << " [y/N]: ";
	int confirm = readKey();
	if (confirm == 'y' || confirm == 'Y') {
		try {
			fs::remove_all(target);
			if (selected > 0) selected--;
		}
		catch (const fs::filesystem_error& e) {
			std::cout << "\nError: " << e.what() << "\nPress any key...";
			readKey();
		}
	}
}
void GetAndPrint::createNew(bool isFolder) {
	std::cout << "\033[H\033[J";
	std::cout << "Create " << (isFolder ? "Folder" : "File") << " Name: ";
	std::string name;
	std::getline(std::cin, name);
	if (name.empty()) return;
	try {
		fs::path newPath = fs::current_path() / name;
		if (isFolder) {
			fs::create_directory(newPath);
		}
		else {
			std::ofstream outfile(newPath);
		}
	}
	catch (const fs::filesystem_error& e) {
		std::cout << "Error: " << e.what() << "\nPress any key...";
		readKey();
	}
}
void GetAndPrint::handleInput() {
	std::vector<std::string> asciiart = {
		".____.------.",
		"| explr     |",
		"| by: ladsm |",
		"| 1.0       |",
		"`-----------'",
		"Press any key..."
	};
	int startRow = 0;
	int startCol = 0;
	InputType Input = GetPlayerInput();
	switch (Input) {
	case InputType::E:
		addTab(fs::current_path());
		return;
	case InputType::T:
		closeCurrentTab();
		return;
	case InputType::Top1: switchTab(0); return;
	case InputType::Top2: switchTab(1); return;
	case InputType::Top3: switchTab(2); return;
	case InputType::Top4: switchTab(3); return;
	case InputType::Top5: switchTab(4); return;
	case InputType::Top6: switchTab(5); return;
	case InputType::Top7: switchTab(6); return;
	case InputType::Top8: switchTab(7); return;
	case InputType::Top9: switchTab(8); return;
	}
	if (tabs.empty() || currenttab >= tabs.size()) return;
	tab& current = tabs[currenttab];
	size_t& selected = current.selectedIndex;
	auto filenames = getfilenames();
	filenames.insert(filenames.begin(), "..");
	if (selected >= filenames.size()) {
		selected = filenames.empty() ? 0 : filenames.size() - 1;
	}
	switch (Input) {
	case InputType::MoveDown:
		if (selected < filenames.size() - 1) selected++;
		break;
	case InputType::MoveUp:
		if (selected > 0) selected--;
		break;
	case InputType::R:
		renameSelected();
		break;
	case InputType::D:
		copySelected(false);
		deleteSelected();
		break;
	case InputType::n:
		createNew(false);
		break;
	case InputType::N:
		createNew(true);
		break;
	case InputType::C:
		copySelected(true);
		break;
	case InputType::P:
		pasteClipboard();
		break;
	case InputType::Q:
		lastpath = fs::current_path().string();
		std::cout << "\033[?1049l\033[?25h" << lastpath;
		std::cout.flush();
		std::exit(0);
		break;
	case InputType::V:
		std::cout << "\033[H\033[J";
		startRow = (getConsoleHeight() / 2) - ((int)asciiart.size() / 2);
		startCol = (getConsoleWidth() / 2) - ((int)asciiart[0].size() / 2);

		for (int i = 0; i < startRow; i++) std::cout << '\n';

		for (const std::string& line : asciiart) {
			std::cout << std::string(startCol, ' ') << line << '\n';
		}
		readKey();
		break;
	case InputType::Enter: {
		std::string chosen = filenames[selected];
		fs::path new_path = fs::current_path() / chosen;
		if (chosen == "..") {
			std::cout << "\033[H\033[J";
			fs::current_path(fs::current_path().parent_path());
			selected = 0;
		}
		else if (fs::is_directory(new_path)) {
			std::cout << "\033[H\033[J";
			fs::current_path(new_path);
			selected = 0;
		}
		else {
#ifdef _WIN32
			std::string systemdotslash = ".\\";
#else
			std::string systemdotslash = "./";
#endif
			system((systemdotslash + chosen).c_str());
		}
		break;
		}
	}
	current.pathoftab = fs::current_path();
}
void GetAndPrint::printStatusBar(const std::vector<std::string>& filenames) {
	size_t& selected = tabs[currenttab].selectedIndex;
	if (selected >= filenames.size()) return;
	fs::path selectedPath = fs::current_path() / filenames[selected];
	std::string info = " [Selected: " + filenames[selected] + "]";
	if (fs::exists(selectedPath) && fs::is_regular_file(selectedPath)) {
		auto size = fs::file_size(selectedPath) / 1024;
		info += " | Size: " + std::to_string(size) + " KB";
		auto ftime = fs::last_write_time(selectedPath);
		auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
			ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
		);
		std::time_t ctime = std::chrono::system_clock::to_time_t(sctp);
		std::string timeStr = std::ctime(&ctime);
		if (!timeStr.empty() && timeStr.back() == '\n') {
			timeStr.pop_back();
		}
		info += " | Mod: " + timeStr;
		std::cout << "\n\033[7m" << info << " \033[0m\033[K";
	}
	else {
		std::cout << "\n\033[7m" << info << " | Type: Directory \033[0m\033[K";
	}

}
void GetAndPrint::print() {
	size_t& selected = tabs[currenttab].selectedIndex;
	std::stringstream buffer;
	std::cout << "\033[H";
	buffer << "\033[7m";
	for (size_t i = 0; i < tabs.size(); ++i) {
		if ((int)i == currenttab) {
			buffer << " [* " << tabs[i].pathoftab.filename().string() << " *] ";
		}
		else {
			buffer << " [ " << tabs[i].pathoftab.filename().string() << " ] ";
		}
	}
	buffer << "\033[0m\033[K\n";
	fs::path full_path = fs::current_path();
	int termHeight = getConsoleHeight();
	buffer << "\033[7m"
		<< "[Arrows]: Nav  | [Enter]: Open   | [R]: Rename | [D]: Delete  | [C]: Copy    |   [P]: Paste      |               \n"
		<< "     [n]: File |     [N]: Folder | [Q]: exit   | [V]: Version | [E]: New Tab | [1-9]: Switch Tab | [T]: Close Tab\033[0m\033[K" << std::endl;
	std::vector<std::string> filenames = getfilenames();
	filenames.insert(filenames.begin(), "..");
	int reservedLines = 5;
	int maxDisplay = (termHeight > reservedLines) ? (termHeight - reservedLines) : 1;
	size_t startIndex = 0;
	if (selected >= (size_t)maxDisplay) {
		startIndex = selected - (maxDisplay / 2);
	}
	if (startIndex + maxDisplay > filenames.size()) {
		startIndex = (filenames.size() > (size_t)maxDisplay) ? (filenames.size() - maxDisplay) : 0;
	}
	size_t endIndex = std::min(startIndex + maxDisplay, filenames.size());
	for (size_t i = startIndex; i < endIndex; ++i) {
		std::string prefix = (i == filenames.size() - 1) ? "└── " : "├── ";
		fs::path entry_path = full_path / filenames[i];
		bool isDir = fs::is_directory(entry_path);
		static const std::set<std::string> exeExtensions = { ".exe", ".cmd", ".sh", ".ps1", ".bat" };
		bool isExe = exeExtensions.count(entry_path.extension().string());
		std::string displayName = filenames[i] + (isDir ? "/" : "");
		buffer << prefix;
		if (i == selected) {
			buffer << "\033[47;30m" << displayName << "\033[0m";
		}
		else if (isDir) {
			buffer << "\033[1;34m" << displayName << "\033[0m";
		}
		else if (isExe) {
			buffer << "\033[32m" << displayName << "\033[0m";
		}
		else {
			buffer << displayName;
		}
		buffer << "\033[K\n";
	}
	int linesDrawn = (int)(endIndex - startIndex);
	for (int i = linesDrawn; i < maxDisplay; ++i) {
		buffer << "\n";
	}
	if (selected < filenames.size()) {
		fs::path selectedPath = full_path / filenames[selected];
		buffer << "\033[7m [Selected: " << filenames[selected];
		if (fs::exists(selectedPath) && fs::is_regular_file(selectedPath)) {
			auto size = fs::file_size(selectedPath) / 1024;
			buffer << " | Size: " << size << " KB";
		}
		else {
			buffer << " | Type: Directory";
		}
		buffer << " ]\033[0m\033[K" << std::flush;
	}
	std::cout << buffer.str() << std::flush;
}