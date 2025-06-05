
## EXCELLENTEAM_EX03 - Shell
### Author: Ilya Kirshtein
### ID: 323299362
### Mail: Ilyaki@edu.jmc.ac.il

## Overview:

A simple custom shell program for linux.

capable of running programs in PATH$ ( such as echo, ls, etc...)
running custom made cd, myjobs.
and running executable files

## Created Files:

Shell.h && Shell.cpp:
Responsible for user input and sending the command to the correct executioner, whether its a linux PATH$ command, custom made one
or running an executable.

ProcessManager.h && ProcessManager.cpp:
Responsible for tracking processes that run with '&' at the end of the exec command.

Command.h && Command.cpp:
Abstract base class for all custom made commands.

CommandFactory.h && CommandFactory.cpp:
Factory for custom made commands.

cdCommand, execCommand, myjobsCommand.h && .cpp:
implementation of custom made commands for the shell.

## Remarks for the examiner:

'exit' or ctrl+c to exit.

