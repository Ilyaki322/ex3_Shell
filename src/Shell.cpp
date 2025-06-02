#include <iostream>
#include <unistd.h>
#include "Shell.h"
#include "CommandFactory.h"

Shell::Shell()
    : m_currPath(std::getenv("HOME")), m_username(std::getenv("USER"))
{
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    m_host = hostname;

    CommandFactory cf;
    m_commandMap = cf.createAllCommands(*this);
}

void Shell::run()
{
    std::string command;

    std::cout << getUserAndPath();
    std::cin >> command;
    runCommand(command);
}

std::string Shell::getUserAndPath() const
{
    std::string path;
    if (m_currPath == std::string(std::getenv("HOME"))) path = "~";

    return std::string(m_username + "@" + m_host + ":" + path + "$ ");
}

std::filesystem::path Shell::getCurrPath() const
{
    const std::filesystem::path copy(m_currPath);
    return copy;
}

void Shell::runCommand(std::string command)
{
    std::vector<std::string> args;
    m_commandMap[command]->execute(args);
}
