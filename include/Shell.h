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

private:
    std::vector<std::string> split(const std::string& s, char delimiter);
    char** vecToArgv(const std::vector<std::string>& args);
    void executeCommand(const std::vector<std::string>& args);
    void runCommand(std::vector<std::string> command);

    void runExecutable(std::vector<std::string> command);


    std::filesystem::path m_currPath;
    std::string m_username;
    std::string m_host;

    std::map<std::string, std::unique_ptr<Command>> m_commandMap;
};