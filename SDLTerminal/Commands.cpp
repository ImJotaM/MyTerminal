#pragma once
#include "Terminal.h"

void Terminal::COMMAND_CD(int argc, std::vector<std::string> argv) {
	
	std::string out = "";

	if (argc != 2) {
		out = "Invalid number of arguments for command 'cd'.\n";
		Print(out);
		return;
	}

	fs::path newdir = argv[1];
	if (!newdir.is_absolute()) {
		newdir = currentdir / argv[1];
	}

	newdir = fs::weakly_canonical(newdir);

	if (fs::exists(newdir)) {
		if (fs::is_directory(newdir)) {
			currentdir = newdir;
		} else {
			out = "\"" + newdir.string() + "\" is not a directory.";
			Print(out);
		}
	} else {
		out = "The directory \"" + newdir.string() + "\" does not exist.\n";
		Print(out);
	}

}

void Terminal::COMMAND_LS(int argc, std::vector<std::string>) {

	std::string out = "";

	if (argc != 1) {
		out = "Invalid number of arguments for command 'ls'.\n";
		Print(out);
		return;
	}

	for (auto& entry : fs::directory_iterator(currentdir)) {
		if (entry.is_directory()) {
			Print(entry.path().filename().string(), BLUE);
		} else {
			Print(entry.path().filename().string());
		}
	}

	Print("\n");

}

void Terminal::COMMAND_CLS(int argc, std::vector<std::string>) {
	
	std::string out = "";
	
	if (argc != 1) {
		out = "Invalid number of arguments for command 'cls'.\n";
		Print(out);
		return;
	}

	history.clear();

}

void Terminal::COMMAND_MKDIR(int argc, std::vector<std::string>) {

	std::string out = "";

	if (argc < 2 || argc > 3) {
		out = "Invalid number of arguments for command 'mkdir'.\n";
		Print(out);
		return;
	}

}

void Terminal::COMMAND_RM(int argc, std::vector<std::string>) {

}
