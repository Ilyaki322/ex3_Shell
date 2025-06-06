#include "execCommand.h"
#include "CommandFactory.h"
#include "Shell.h"
#include <filesystem>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>
#include <iostream>
#include <fcntl.h>

execCommand::execCommand(const Shell &s)
    : Command(s), m_redirectIn(false), m_redirectOut(false),
    m_inPath(""), m_outPath("")
{}

/**
 * executes a given command, if command ends with &, will run in background.
 * will wait otherwise.
 *
 * @param args args[0] is command, others are arguments for the execution.
 */
void execCommand::execute(std::vector<std::string> args)
{
    m_redirectIn = false;
    m_redirectOut = false;
    checkRedirection(args);

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
        if (m_redirectIn || m_redirectOut) redirect();
        std::vector<std::string> argv = args;
        argv[0] = fullPath.string();
        char** c_argv = m_shell.vecToArgv(argv);
        execv(fullPath.c_str(), c_argv);

        //execv failed
        perror("execv");
        for (size_t i = 0; i < argv.size(); ++i) free(c_argv[i]);
        delete[] c_argv;
        exit(1);
    }
    else if (pid > 0 && args[0][args[0].size()-1] != '&') {
        waitpid(pid, nullptr, 0);
    }
    else if (pid == -1) perror("fork");

    if (args[0][args[0].size()-1] == '&')
        const_cast<Shell&>(m_shell).getManager().addProcess(pid, args[0]);
}

void execCommand::checkRedirection(std::vector<std::string>& args)
{
    for (int i = 0; i < args.size();) {
        if (args[i] != "<" && args[i] != ">") {
            i++;
            continue;
        }

        if (args[i] == ">") {
            m_outPath = std::filesystem::absolute(args[i + 1]).string();
            m_redirectOut = true;
            args.erase(args.begin() + i);
            args.erase(args.begin() + i);
            continue;
        }

        if (args[i] == "<") {
            m_inPath = std::filesystem::absolute(args[i + 1]).string();
            m_redirectIn = true;
            args.erase(args.begin() + i);
            args.erase(args.begin() + i);
            continue;
        }
    }
}

void execCommand::redirect()
{
    if (m_redirectIn) {
        int in_fd = open(m_inPath.c_str(), O_RDONLY);
        if (in_fd < 0) {
            std::cerr << "redirection failed, exitting\n";
            exit(1);
        }

        dup2(in_fd, STDIN_FILENO);
        close(in_fd);
    }

    if (m_redirectOut) {
        int out_fd = open(m_outPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (out_fd < 0) {
            std::cerr << "redirection failed, exitting\n";
            exit(1);
        }

        dup2(out_fd, STDOUT_FILENO);
        close(out_fd);
    }
}

// Registers the command at the factory.
bool execCommand::m_isRegistered = CommandFactory::registerCommand("/", [](const Shell& s) {
    return std::make_unique<execCommand>(s);
});