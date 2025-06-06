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
    void checkRedirection(std::vector<std::string>& args);
    void redirect();

    static bool m_isRegistered;

    bool m_redirectIn;
    bool m_redirectOut;
    std::string m_inPath;
    std::string m_outPath;
};