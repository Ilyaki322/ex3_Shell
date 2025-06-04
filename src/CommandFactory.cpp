#include "CommandFactory.h"

/**
 * Registers a command at the factory.
 * 
 * @param key key for the command in the map.
 * @param f function that constructs and returns a unique_ptr to wanted command.
 * @return true on success.
 */
bool CommandFactory::registerCommand(std::string key, func f)
{
    if (m_factoryMap.find(key) != m_factoryMap.end()) return false;

    m_factoryMap[key] = f;
    return true;
}

/**
 * creates all commands in the map.
 * 
 * @param s Referance to the Shell class, required for all commands.
 * @return map of string --> unique_ptr<Command>.
 */
std::map<std::string, std::unique_ptr<Command>> CommandFactory::createAllCommands(const Shell& s)
{
    std::map<std::string, std::unique_ptr<Command>> m;

    for(const auto& [key, f] : m_factoryMap) {
        m[key] = f(s);
    }

    return m;
}

/**
 * creates a single command.
 * 
 * @param key key for the command in the map
 * @param s Referance to the Shell class, required for all commands.
 * @return unique_ptr for wanted command, nullptr if not found.
 */
std::unique_ptr<Command> CommandFactory::createCommand(std::string key, const Shell& s)
{
    auto it = m_factoryMap.find(key);
    if (it == m_factoryMap.end()) {
	    return nullptr;
	}
    return it->second(s);
}

// init static map.
std::map<std::string, func> CommandFactory::m_factoryMap;
