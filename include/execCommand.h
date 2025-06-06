#pragma once
#include "Command.h"

/*
* Encapsulates logic for './' or '/' commands
*/
class execCommand : public Command {
public:
    execCommand(const Shell& s);
    void execute(std::vector<std::string> args) override;

private:
    void execSingle(std::vector<std::string> args);
    void pipeExec(std::vector<std::string> args);
    void exec(std::vector<std::string> argv);
    void checkRedirection(std::vector<std::string>& args);
    void redirect();

    bool checkFile(std::string& file);

    static bool m_isRegistered;

    bool m_redirectIn;
    bool m_redirectOut;
    std::string m_inPath;
    std::string m_outPath;
};