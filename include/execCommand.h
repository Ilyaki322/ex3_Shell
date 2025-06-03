#pragma once
#include "Command.h"

/*
* Encapsulates logic for './' or '/' commands
*/
class execCommand : public Command {
public:
    execCommand(const Shell& s) : Command(s) {}
    void execute(std::vector<std::string> args) override;

private:
    static bool m_isRegistered;
};