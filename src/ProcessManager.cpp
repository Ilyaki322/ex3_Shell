#include "ProcessManager.h"
#include <iostream>
#include <sys/wait.h>
#include <signal.h>

/**
 * Adds a child process to the list
 * @param id pid_t of the child.
 * @param command the command its executing.
 */
void ProcessManager::addProcess(pid_t id, std::string command)
{
    process_data newData = {id, command};
    m_data.push_back(newData);
}

/**
 * prints the status of all children.
 * deletes ones that finished.
 */
void ProcessManager::printStatus()
{
    if (m_data.size() == 0) {
        std::cout << "No Jobs currently running\n";
        return;
    }
    
    for (size_t i = 0; i < m_data.size();) {
        pid_t result = waitpid(m_data[i]._pid, nullptr, WNOHANG);

        if (result == 0) {
            std::cout << "PID " << m_data[i]._pid << " (" << m_data[i]._command << "): Running\n";
            i++;
        }
        else if (result == -1) {
            std::cout << "PID " << m_data[i]._pid << " (" << m_data[i]._command << "): Error checking status\n";
            i++;
        }
        else {
            std::cout << "PID " << m_data[i]._pid << " (" << m_data[i]._command << "): Exited\n";
            m_data.erase(m_data.begin() + i);
        }
    }
}
