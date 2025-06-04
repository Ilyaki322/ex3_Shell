#include "cdCommand.h"
#include "CommandFactory.h"
#include "Shell.h"
#include <unistd.h> 
#include <filesystem>
#include <string>
#include <iostream>

/**
 * implements the 'cd' command.
 * 
 * @param args arguments for cd command.
 */
void cdCommand::execute(std::vector<std::string> args)
{
    if (args.size() == 0) return;

    // cd with no args returns us to HOME.
    if (args.size() == 1) {
       chdir(std::getenv("HOME")); 
       return;
    } 

    // cd ..
    if (args[1] == "..") {
        char p[1000];
        std::filesystem::path dir = getcwd(p, sizeof(p));
        chdir(dir.parent_path().c_str());
        return;
    }

    // cd with absolute path
    if (args[1].rfind("/", 0) == 0) {
        std::filesystem::path dir = args[1];
        if (std::filesystem::is_directory(dir)) {
            chdir(args[1].c_str());
            return;
        }
        std::cout << args[1] << " is not a directory\n";
        return;
    }

    // cd to relative path
    char p[1000];
    std::filesystem::path dir = getcwd(p, sizeof(p));
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.path().filename().string() == args[1]) {
            if (!entry.is_directory()) {
                std::cout << args[1] << " is not a directory\n";
                return;
            }

            chdir(args[1].c_str());
            return;
        } 
    }

    std::cout << args[1] << " not found\n";
    return;
}

// Register command at factory.
bool cdCommand::m_isRegistered = CommandFactory::registerCommand("cd", [](const Shell& s) {
    return std::make_unique<cdCommand>(s);
});