#pragma once
#include "Command.h"

/*
* Custom 'myjobs' command to see all running children.
*/
class myjobsCommand : public Command {
public:
    myjobsCommand(const Shell& s) : Command(s) {}
    void execute(std::vector<std::string> args) override;
private:
    static bool m_isRegistered;
};