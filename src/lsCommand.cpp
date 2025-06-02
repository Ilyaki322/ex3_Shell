#include "lsCommand.h"
#include <iostream>
#include <filesystem>
#include "CommandFactory.h"
#include "Shell.h"

void lsCommand::execute(std::vector<std::string> args)
{
    auto path = m_shell.getCurrPath();

    std::cout << std::endl;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::cout << entry.path() << std::endl;
    }
}

bool lsCommand::m_isRegistered = CommandFactory::registerCommand("lss", [](const Shell& s) {
    return std::make_unique<lsCommand>(s);
});