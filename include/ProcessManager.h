#pragma once
#include <unistd.h>
#include <string>
#include <vector>

struct process_data {
    pid_t _pid;
    std::string _command;
};

/*
* Responsible for tracking processes of the shell.
*/
class ProcessManager {
public:
    void addProcess(pid_t id, std::string command);
    void printStatus() const;
private:
    std::vector<process_data> m_data;
};