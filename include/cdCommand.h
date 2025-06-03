#pragma once

#include "Command.h"

/*
* This encapsulates the 'cd' command,
* since cd uses program internal state, we had to implement it ourselves
*/
class cdCommand : public Command {
public:
    cdCommand(const Shell& s) : Command(s) {}
    void execute(std::vector<std::string> args) override;

private:
    static bool m_isRegistered;
};