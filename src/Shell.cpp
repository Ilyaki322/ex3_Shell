#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>
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

    while(true) {
        std::cout << getUserAndPath();

        if (!std::getline(std::cin, command)) break;
        if (command.empty()) continue;
        if (command == "exit") break;

        std::vector<std::string> args = split(command, ' ');
        runCommand(args);
    }
}

std::string Shell::getUserAndPath() const
{
    std::string path;
    char cwd[1024];
    std::string currPath(getcwd(cwd, sizeof(cwd)));

    if (currPath == std::string(std::getenv("HOME"))) path = "~";
    else path = currPath;

    return std::string(m_username + "@" + m_host + ":" + path + "$ ");
}

std::filesystem::path Shell::getCurrPath() const
{
    const std::filesystem::path copy(m_currPath);
    return copy;
}

std::vector<std::string> Shell::split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string item;

    while(std::getline(ss, item, delimiter)) {
        tokens.push_back(item);
    }

    return tokens;
}

// make sure to free argv!
char **Shell::vecToArgv(const std::vector<std::string> &args)
{
    char** argv = new char*[args.size() + 1];
    for (int i = 0; i < args.size(); i++) {
        argv[i] = strdup(args[i].c_str());
    }

    argv[args.size()] = nullptr; // argv must be null terminated.
    return argv;
}

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

    for (const std::string& dir : paths) {
        std::string fullpath = dir + "/" + command;
        if (access(fullpath.c_str(), X_OK) == 0) {
            std::vector<std::string> argv = args;
            argv[0] = fullpath;
            char** c_argv = vecToArgv(argv);
            execv(fullpath.c_str(), c_argv);
            perror("execv");

            for (size_t i = 0; i < argv.size(); ++i) free(c_argv[i]);
            delete[] c_argv;
            exit(1);
        }
    }

    std::cerr << "Command not found: " << command << "\n";
}

void Shell::runCommand(std::vector<std::string> command)
{
    // if built in command
    if (m_commandMap.find(command[0]) != m_commandMap.end()) {
        m_commandMap[command[0]]->execute(command);
        return;
    }

    // executable
    if (command[0][0] == '/' || command[0].substr(0, 2) == "./") {
        runExecutable(command);
        return;
    }

    // linux command
    pid_t pid = fork();
    if (pid == 0) {
        executeCommand(command);
        exit(0);
    }
    else if (pid > 0) {
        waitpid(pid, nullptr, 0);
    }
    else perror("fork");
}

void Shell::runExecutable(std::vector<std::string> command)
{
    std::filesystem::path file = command[0];
    std::filesystem::path fullPath = std::filesystem::absolute(file);

    if (!std::filesystem::is_regular_file(fullPath)) {
        std::cout << fullPath.string() << " is not an executable\n";
        return;
    }
    if (access(fullPath.c_str(), X_OK) != 0)  {
        std::cout << "Access Denied\n";
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        std::vector<std::string> argv = command;
        argv[0] = fullPath.string();
        char** c_argv = vecToArgv(argv);
        execv(fullPath.c_str(), c_argv);
        perror("execv");
        for (size_t i = 0; i < argv.size(); ++i) free(c_argv[i]);
        delete[] c_argv;
        exit(1);
    }
    else if (pid > 0 && command[0][command[0].size()-1] != '&') {
        waitpid(pid, nullptr, 0);
    }
    else if (pid == -1) perror("fork");

}
