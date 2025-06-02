#pragma once
#include <functional>
#include <map>
#include <memory>

#include "Command.h"

using func = std::function<std::unique_ptr<Command>(const Shell& s)>;

class CommandFactory {
public:
    static bool registerCommand(std::string key, func f);
    std::map<std::string, std::unique_ptr<Command>> createAllCommands(const Shell& s);
    
private:
    std::unique_ptr<Command> createCommand(std::string key, const Shell& s);
    static std::map<std::string, func> m_factoryMap;
};