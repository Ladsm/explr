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
#ifdef min
#undef min
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

int GetAndPrint::getThingToDo() {
    int returner = 0;
	InputType input = GetPlayerInput();
	switch (input) {
	case InputType::Enter:
        returner = 1;
        break;
    case InputType::MoveDown:
        returner = 2;
        break;
    case InputType::MoveUp:
        returner = 3;
        break;
    case InputType::R:
        returner = 4;
        break;
    case InputType::D:
        returner = 5;
        break;
    case InputType::n:
        returner = 6;
        break;
    case InputType::N:
        returner = 7;
        break;
    case InputType::Q:
        returner = 8;
        break;
    case InputType::C:
        returner = 9;
        break;
    case InputType::P:
        returner = 10;
        break;
    case InputType::V:
        returner = 11;
        break;
    default:
        returner = 0;
        break;
	}
    return returner;
}

void GetAndPrint::copySelected() {
    auto filenames = getfilenames();
    filenames.insert(filenames.begin(), "..");
    if (selected >= filenames.size() || filenames[selected] == "..") return;
    clipboardPath = fs::current_path() / filenames[selected];
    hasClipboard = true;
    std::cout << "\033[H\033[J";
    std::cout << "Copied: " << clipboardPath.filename().string() << "\nPress any key...";
    readKey();
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

void GetAndPrint::renameSelected() {
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
    auto filenames = getfilenames();
    filenames.insert(filenames.begin(), "..");
    if (selected >= filenames.size() || filenames[selected] == "..") return;
    fs::path target = fs::current_path() / filenames[selected];
    std::cout << "\033[H\033[J";
    std::cout << "ARE YOU SURE? Delete: " << filenames[selected] << " [y/N]: ";
    int confirm =
#ifdef _WIN32
        readKey();
#else
        readKey();
#endif

    if (confirm == 'y' || confirm == 'Y') {
        try {
            fs::remove_all(target);
            if (selected > 0) selected--;
        }
        catch (const fs::filesystem_error& e) {
            std::cout << "\nError: " << e.what() << "\nPress any key...";
#ifdef _WIN32
            readKey();
#else
            readKey();
#endif
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
void GetAndPrint::handleInput(int input) {
    std::vector<std::string> filenames = getfilenames();
    filenames.insert(filenames.begin(), "..");
    if (input == 2 && selected < filenames.size() - 1) {
        selected++;
    } else if (input == 4) { renameSelected(); }
    else if (input == 5) { copySelected(); deleteSelected(); }
    else if (input == 6) { createNew(false); }
    else if (input == 7) { createNew(true); }
    else if (input == 8) { 
        lastpath = fs::current_path().string();
        std::cout << "\033[?1049l\033[?25h" << lastpath; 
        std::cout.flush(); 
        std::exit(0); 
    }
    else if (input == 9) { copySelected(); }
    else if (input == 10) { pasteClipboard(); }
    else if (input == 11) {
        std::cout << ".____.------.\n";
        std::cout << "|   explr   |\n";
        std::cout << "| by: ladsm |\n";
        std::cout << "|    1.0    |\n";
        std::cout << "`-----------'\n";
    }
    else if (input == 3 && selected > 0) {
        selected--;
    }
    else if (input == 1) {
        std::string chosen = filenames[selected];
        fs::path new_path = fs::current_path() / chosen;
        if (chosen == "..") {
            std::cout << "\033[2J\033[H";
            fs::current_path(fs::current_path().parent_path());
            selected = 0;
        }
        if (fs::is_directory(new_path)) {
            try {
                std::cout << "\033[2J\033[H";
                fs::current_path(new_path);
                selected = 0;
            }
            catch (const fs::filesystem_error& e) {
                std::cout << "Error: Cannot access directory: " << e.what() << "\nPress any key to continue...";
                readKey();
            }
        }
        else {
            std::string systemdotslash =
#ifdef _WIN32
                ".\\";
#else
                "./";
#endif
            std::cout << "Selected file: " << chosen << std::endl;
            system((systemdotslash + chosen).c_str());
        }
    }
}

void GetAndPrint::printStatusBar(const std::vector<std::string>& filenames) {
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
    std::stringstream buffer;
    std::cout << "\033[H";
    fs::path full_path = fs::current_path();
    int termHeight = getConsoleHeight();
    buffer << "\033[7m"
              << "[Arrows]: Nav  | [Enter]: Open   | [R]: Rename | [D]: Delete | [C]: Copy | [P]: Paste\n"
              << "     [n]: File | [N]    : Folder | [Q]: exit   | [V]: Version \033[0m\033[K" << std::endl;
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
        bool isExe = entry_path.extension() == ".exe";
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