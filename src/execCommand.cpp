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
 * checks for pipes and runs pipeExec or single exec.
 * 
 * @param args command or chain of commands chained with pipes |
 */
void execCommand::execute(std::vector<std::string> args) {   
    for (const auto& s : args) {
        if (s == "|") {
            pipeExec(args);
            return;
        }
    }

    execSingle(args);
}

/**
 * runs |args| commands in parallel connecting following ones with pipe.
 * 
 * @param ags chain of commands chained with pipes |
 */
void execCommand::pipeExec(std::vector<std::string> args)
{
    std::vector<std::vector<std::string>> commands;
    std::vector<std::string> temp;

    // split commands by '|'
    for (int i = 0; i < args.size(); i++) {
        if (args[i] != "|") {
            temp.push_back(args[i]);
            continue;
        }
        commands.push_back(temp);
        temp.clear();
    }
    commands.push_back(temp);

    // make sure all commands can be run
    for (int i = 0; i < commands.size(); i++) {
        if (!checkFile(commands[i][0])) return;
    }

    // loop through the commands and fork for each, while managing pipes.
    int prevPipe[2] = {-1, -1};
    std::vector<pid_t> pids;
    for (int i = 0; i < commands.size(); i++) {
        int newPipe[2] = {-1, -1};
        if (i < commands.size() - 1) {
            if (pipe(newPipe) == -1) {
                perror("pipe");
                return;
            }
        }

        m_redirectIn = m_redirectOut = false;
        if (i == commands.size() - 1) checkRedirection(commands[i]);

        pid_t pid = fork();
        if (pid == 0) {
            if (i > 0) {
                dup2(prevPipe[0], STDIN_FILENO);
                close(prevPipe[0]);
                close(prevPipe[1]);
            }
            if (i < commands.size() - 1) {
                close(newPipe[0]);
                dup2(newPipe[1], STDOUT_FILENO);
                close(newPipe[1]);                
            }

            std::vector<std::string> argv = commands[i];
            exec(argv);
        }
        else if (pid < 0) {
            perror("fork");
            return;
        }

        pids.push_back(pid);

        if (i > 0) {
            close(prevPipe[0]);
            close(prevPipe[1]);
        }

        prevPipe[0] = newPipe[0];
        prevPipe[1] = newPipe[1];
    }

    for (const auto& id : pids) {
        waitpid(id, nullptr, 0);
    }
}

/**
 * helper function to convert vector to char** and run execv
 * 
 * @param argv command to run
 */
void execCommand::exec(std::vector<std::string> argv)
{
    if (m_redirectIn || m_redirectOut) redirect();
    char** c_argv = m_shell.vecToArgv(argv);
    execv(argv[0].c_str(), c_argv);

    //execv failed
    perror("execv");
    for (size_t i = 0; i < argv.size(); ++i) free(c_argv[i]);
    delete[] c_argv;
    exit(1);
}

/**
 * executes a given command, if command ends with &, will run in background.
 * will wait otherwise.
 *
 * @param args args[0] is command, others are arguments for the execution.
 */
void execCommand::execSingle(std::vector<std::string> args)
{
    m_redirectIn = false;
    m_redirectOut = false;
    checkRedirection(args);

    bool background = false;
    if (args.back() == "&") {
        background = true;
        args.pop_back();
    }
    if (args.back().back() == '&') {
        background = true;
        args.back().pop_back();
    }

    if (!checkFile(args[0])) return;
    std::filesystem::path fullPath = args[0];


    pid_t pid = fork();
    if (pid == 0) {
        std::vector<std::string> argv = args;
        argv[0] = fullPath.string();
        exec(argv);
    }
    else if (!background) {
        waitpid(pid, nullptr, 0);
    }
    else if (pid == -1) perror("fork");

    if (background)
        const_cast<Shell&>(m_shell).getManager().addProcess(pid, args[0]);
}

/**
 * Searches for '>' and '<', removes them from args
 * and turns on m_redirect flags accordingly
 * 
 * @param args command to check.
 */
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

/**
 * Redirects stdin or stdout to m_path based on m_redirect flags.
 */
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

/**
 * Checks if 'file' is an executable the user can run.
 * Modifies file to its full path
 * 
 * @param file file to execute
 * @return true if user can run file.
 */
bool execCommand::checkFile(std::string& file)
{
    std::filesystem::path fullPath;
    std::string checkPATH = m_shell.findInPATH(file);

    if (!checkPATH.empty()) {
        fullPath = checkPATH;
    } 
    else {
        fullPath = std::filesystem::absolute(file);

        if (!std::filesystem::is_regular_file(fullPath)) {
            std::cerr << fullPath.string() << " is not an executable\n";
            return false;
        }
    }

    if (access(fullPath.c_str(), X_OK) != 0) {
        std::cerr << "Access Denied\n";
        return false;
    }

    file = fullPath.string();
    return true;
}

// Registers the command at the factory.
bool execCommand::m_isRegistered = CommandFactory::registerCommand("/", [](const Shell& s) {
    return std::make_unique<execCommand>(s);
});