#include "ProcessManager.h"
#include <iostream>
#include <sys/wait.h>
#include <signal.h>

void ProcessManager::addProcess(pid_t id, std::string command)
{
    process_data newData = {id, command};
    m_data.push_back(newData);
}

void ProcessManager::printStatus() const
{
    if (m_data.size() == 0) {
        std::cout << "No Jobs currently running\n";
        return;
    }
    
    for (const auto& process : m_data) {
        pid_t result = waitpid(process._pid, nullptr, WNOHANG);

        if (result == 0) 
            std::cout << "PID " << process._pid << " (" << process._command << "): Running\n";
        
        else if (result == -1) {
            std::cout << "PID " << process._pid << " (" << process._command << "): Error checking status\n";
        }
        else 
            std::cout << "PID " << process._pid << " (" << process._command << "): Exited\n";
    }
}
