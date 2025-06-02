#include "CommandFactory.h"

bool CommandFactory::registerCommand(std::string key, func f)
{
    if (m_factoryMap.find(key) != m_factoryMap.end()) return false;

    m_factoryMap[key] = f;
    return true;
}

std::map<std::string, std::unique_ptr<Command>> CommandFactory::createAllCommands(const Shell& s)
{
    std::map<std::string, std::unique_ptr<Command>> m;

    for(const auto& [key, f] : m_factoryMap) {
        m[key] = f(s);
    }

    return m;
}

std::unique_ptr<Command> CommandFactory::createCommand(std::string key, const Shell& s)
{
    auto it = m_factoryMap.find(key);
    if (it == m_factoryMap.end()) {
	    return nullptr;
	}
    return it->second(s);
}

std::map<std::string, func> CommandFactory::m_factoryMap;
