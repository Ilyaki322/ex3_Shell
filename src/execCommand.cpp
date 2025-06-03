#include "execCommand.h"
#include "CommandFactory.h"
#include "Shell.h"
#include <filesystem>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>
#include <iostream>

void execCommand::execute(std::vector<std::string> args)
{
    std::filesystem::path file = args[0];
    std::filesystem::path fullPath = std::filesystem::absolute(file);

    std::string pathStr = fullPath.string();
    if (!pathStr.empty() && pathStr.back() == '&') {
        pathStr.pop_back();
        fullPath = pathStr;
    }

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
        std::vector<std::string> argv = args;
        argv[0] = fullPath.string();
        char** c_argv = m_shell.vecToArgv(argv);
        execv(fullPath.c_str(), c_argv);
        perror("execv");
        for (size_t i = 0; i < argv.size(); ++i) free(c_argv[i]);
        delete[] c_argv;
        exit(1);
    }
    else if (pid > 0 && args[0][args[0].size()-1] != '&') {
        waitpid(pid, nullptr, 0);
    }
    else if (pid == -1) perror("fork");

    const_cast<Shell&>(m_shell).getManager().addProcess(pid, args[0]);
}

bool execCommand::m_isRegistered = CommandFactory::registerCommand("/", [](const Shell& s) {
    return std::make_unique<execCommand>(s);
});