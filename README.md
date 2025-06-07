
## EXCELLENTEAM_EX04 - Shell Part II
### Author: Ilya Kirshtein
### ID: 323299362
### Mail: Ilyaki@edu.jmc.ac.il

## Overview:

A simple custom shell program for linux.

capable of running programs in PATH$ ( such as echo, ls, etc...)
running custom made cd, myjobs.
and running executable files

Added functionality of input / output redirection with '>' and '<':

./test > t.txt redirects the output of test to t.txt, will create t.txt if doesnt exist.

./test < t.txt redirects the input of test from t.txt, will fail if t.txt doesnt exist.

./test < t.txt > t2.txt combines the two, test will use t.txt as input and t2.txt as output.

Added functionality of pipes with '|':

Chain commands together like ./a | ./b | ./c

output of a will go to b, output of b will go to c ...

Supports OUTPUT redirection for last command ONLY.

## Created Files:

**Shell.h && Shell.cpp:**

Responsible for user input and sending the command to the correct executioner, whether its a linux PATH$ command, custom made one
or running an executable.

**ProcessManager.h && ProcessManager.cpp:**

Responsible for tracking processes that run with '&' at the end of the exec command.

**Command.h && Command.cpp:**

Abstract base class for all custom made commands.

**CommandFactory.h && CommandFactory.cpp:**

Factory for custom made commands.

**cdCommand, execCommand, myjobsCommand.h && .cpp:**

implementation of custom made commands for the shell.

## Remarks for the examiner:

'exit' or ctrl+c to exit.

& now works correctly, used as last arg or last symbol, for example:

./test& or ./test & or ./test a b& or ./test a b &


## How to compile and run:

in the ex3_Shell directory: create a build directory (mkdir build)
then type in terminal:

cd build

cmake ..

build (or make)

./ex3_shell

