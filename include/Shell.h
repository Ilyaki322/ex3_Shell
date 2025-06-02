#pragma once
#include <filesystem>
#include <string>
#include <map>

#include "Command.h"

class Shell {
public:
    Shell();

    void run();
    std::string getUserAndPath() const;
    std::filesystem::path getCurrPath() const;

private:
    void runCommand(std::string command);


    std::filesystem::path m_currPath;
    std::string m_username;
    std::string m_host;

    std::map<std::string, std::unique_ptr<Command>> m_commandMap;
};