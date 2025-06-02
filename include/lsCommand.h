#pragma once
#include "Command.h"

class lsCommand : public Command { 
public:
    lsCommand(const Shell& s) : Command(s) {}
    void execute(std::vector<std::string> args) override;

private:
    static bool m_isRegistered;
};