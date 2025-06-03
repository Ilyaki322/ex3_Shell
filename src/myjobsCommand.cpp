#include "myjobsCommand.h"
#include "CommandFactory.h"
#include "Shell.h"

void myjobsCommand::execute(std::vector<std::string> args)
{
    m_shell.getManager().printStatus();
}

// Registers the command at the factory.
bool myjobsCommand::m_isRegistered = CommandFactory::registerCommand("myjobs", [](const Shell& s) {
    return std::make_unique<myjobsCommand>(s);
});