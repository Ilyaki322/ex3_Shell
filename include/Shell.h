#pragma once
#include <filesystem>
#include <string>
#include <map>

#include "Command.h"
#include "ProcessManager.h"

class Shell {
public:
    Shell();

    void run();
    std::string getUserAndPath() const;
    char** vecToArgv(const std::vector<std::string>& args) const;
    std::vector<std::string> split(const std::string& s, char delimiter) const;

    ProcessManager& getManager() { return m_pcManager;}
    const ProcessManager& getManager() const { return m_pcManager;}

private:
    void executeCommand(const std::vector<std::string>& args);
    void runCommand(std::vector<std::string> command);
    //void runExecutable(std::vector<std::string> command);

    std::string m_username;
    std::string m_host;

    std::map<std::string, std::unique_ptr<Command>> m_commandMap;

    ProcessManager m_pcManager;
};