#pragma once
#include "Terminal.h"

void Terminal::COMMAND_CD(int argc, std::vector<std::string> argv) {
	
	std::string out = "";

	if (argc != 2) {
		out = "Invalid number of arguments for command 'cd'.\n\n";
		Print(out, LIGHT_RED);
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
			out = "\"" + newdir.string() + "\" is not a directory.\n\n";
			Print(out, LIGHT_RED);
		}
	} else {
		out = "\"" + newdir.string() + "\" does not exist.\n\n";
		Print(out, LIGHT_RED);
	}

}

void Terminal::COMMAND_LS(int argc, std::vector<std::string> argv) {

	std::string out = "";

	if (argc != 1) {
		out = "Invalid number of arguments for command 'ls'.\n\n";
		Print(out, LIGHT_RED);
		return;
	}

	for (auto& entry : fs::directory_iterator(currentdir)) {
		if (entry.is_directory()) {
			Print(entry.path().filename().string(), LIGHT_BLUE);
		} else {
			Print(entry.path().filename().string());
		}
	}

	Print("\n");

}

void Terminal::COMMAND_CLS(int argc, std::vector<std::string> argv) {
	
	std::string out = "";
	
	if (argc != 1) {
		out = "Invalid number of arguments for command 'cls'.\n\n";
		Print(out, LIGHT_RED);
		return;
	}

	history.clear();

}

void Terminal::COMMAND_MKDIR(int argc, std::vector<std::string> argv) {

	std::string out = "";

	if (argc != 2) {
		out = "Invalid number of arguments for command 'mkdir'.\n\n";
		Print(out, LIGHT_RED);
		return;
	}

	fs::path newdir = argv[1];
	if (!newdir.is_absolute()) {
		newdir = currentdir / newdir;
	}

	newdir = fs::weakly_canonical(newdir);

	if (fs::exists(newdir)) {

		out = "\"" + newdir.string() + "\" already exist.\n\n";
		Print(out, LIGHT_RED);

	} else {

		fs::create_directory(newdir);

		if (!fs::exists(newdir)) {
			out = "Failed to create \"" + newdir.string() + "\".\n\n";
			Print(out, LIGHT_RED);
		}

	}

}

void Terminal::COMMAND_RM(int argc, std::vector<std::string> argv) {

	std::string out = "";

	if (argc != 2) {
		out = "Invalid number of arguments for command 'rmdir'.\n\n";
		Print(out, LIGHT_RED);
		return;
	}

	fs::path newdir = argv[1];
	if (!newdir.is_absolute()) {
		newdir = currentdir / newdir;
	}

	newdir = fs::weakly_canonical(newdir);

	if (!fs::exists(newdir)) {

		out = "\"" + newdir.string() + "\" does not exist.\n\n";
		Print(out, LIGHT_RED);

	} else {
		
		fs::remove(newdir);

		if (fs::exists(newdir)) {
			out = "Failed to remove \"" + newdir.string() + "\".\n\n";
			Print(out, LIGHT_RED);
		}
	}

}
