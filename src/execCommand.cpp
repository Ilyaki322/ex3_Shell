#include "execCommand.h"
#include <iostream>
#include <filesystem>
#include "CommandFactory.h"
#include "Shell.h"

void execCommand::execute(std::vector<std::string> args)
{
    
}

bool execCommand::m_isRegistered = CommandFactory::registerCommand("/", [](const Shell& s) {
    return std::make_unique<execCommand>(s);
});