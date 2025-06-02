#pragma once

#include "Command.h"

class cdCommand : public Command {
public:
    cdCommand(const Shell& s) : Command(s) {}
    void execute(std::vector<std::string> args) override;

private:
    static bool m_isRegistered;
};