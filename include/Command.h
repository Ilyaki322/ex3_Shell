#pragma once
#include <vector>
#include <string>

class Shell;

class Command {
public:
    Command(const Shell& s) : m_shell(s) {}
    virtual void execute(std::vector<std::string> args) = 0;

protected:
    const Shell& m_shell;
};