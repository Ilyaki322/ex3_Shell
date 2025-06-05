#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>
#include "Shell.h"
#include "CommandFactory.h"

Shell::Shell()
    : m_username(std::getenv("USER"))
{
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    m_host = hostname;

    CommandFactory cf;
    m_commandMap = cf.createAllCommands(*this);
}

/**
 * Main loop of the Program.
 */
void Shell::run()
{
    std::string command;

    while(true) {
        std::cout << getUserAndPath();

        if (!std::getline(std::cin, command)) break;
        if (command.empty()) continue;
        if (command == "exit") break;

        std::vector<std::string> args = split(command, ' ');
        runCommand(args);
    }
}

/**
 * @return string with {username}@{system} as gotten from system calls
 */
std::string Shell::getUserAndPath() const
{
    std::string path;
    char cwd[1024];
    std::string currPath(getcwd(cwd, sizeof(cwd)));

    if (currPath == std::string(std::getenv("HOME"))) path = "~";
    else path = currPath;

    return std::string(m_username + "@" + m_host + ":" + path + "$ ");
}

/**
 * Utility function to split a line by delimiter
 * 
 * @param s string to split
 * @param delimiter char to split by
 * @return vector of strings that got split.
 */
std::vector<std::string> Shell::split(const std::string &s, char delimiter) const
{
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string item;

    while(std::getline(ss, item, delimiter)) {
        tokens.push_back(item);
    }

    return tokens;
}

/**
 * Utility function.
 * System calls use char**
 * **REQUIRED TO BE FREED!**
 * 
 * @param args a vector of strings to convert to char**
 * @return char** of given vector of strings.
 */
char **Shell::vecToArgv(const std::vector<std::string> &args) const
{
    char** argv = new char*[args.size() + 1];
    for (int i = 0; i < args.size(); i++) {
        argv[i] = strdup(args[i].c_str());
    }

    argv[args.size()] = nullptr; // argv must be null terminated.
    return argv;
}

/**
 * executes a linux command, searches in PATH for the command and execs it.
 * 
 * @param args full command with arguments
 */
void Shell::executeCommand(const std::vector<std::string> &args)
{
    if (args.empty()) return;

    const char* path = std::getenv("PATH");
    if (!path) {
        std::cerr << "PATH not set \n";
        return;
    }

    std::vector<std::string> paths = split(path, ':');
    std::string command = args[0];

    bool amp = false;
    if (command.back() == '&') {
        amp = true;
        command.pop_back();
    }

    for (const std::string& dir : paths) {
        std::string fullpath = dir + "/" + command;
        if (access(fullpath.c_str(), X_OK) == 0) {
            std::vector<std::string> argv = args;
            if (amp) command += "&";
            argv[0] = fullpath;
            std::cout << fullpath << std::endl;
            m_commandMap["/"]->execute(argv);
            return;
        }
    }

    std::cerr << "Command not found: " << command << "\n";
}

/**
 * Sends the command to correct 'executor'
 * 
 * @param command full command with arguments.
 */
void Shell::runCommand(std::vector<std::string> command)
{
    // built in command
    if (m_commandMap.find(command[0]) != m_commandMap.end()) {
        m_commandMap[command[0]]->execute(command);
        return;
    }

    // executable
    if (command[0][0] == '/' || command[0].substr(0, 2) == "./") {
        m_commandMap["/"]->execute(command);
        return;
    }

    // not buildin and not executable, search $PATH
    executeCommand(command);
}
