# Custom-Shell
A simple shell  in C++ that can execute  commands, manage processes,  and handle redirection and piping
📘 Project Overview

This project was developed as part of the Capstone Project (Assignment 2) for the Linux Operating System course.
It implements a console-based custom shell in C++, designed to replicate essential Bash-like functionality including command execution, process management, redirection, and piping. The shell interacts directly with the Linux kernel through low-level system calls, giving practical exposure to OS internals.

🎯 Objectives

Execute Linux commands directly from the shell

Manage foreground and background processes

Support input/output redirection (>, >>, <)

Implement simple command piping (|)

Provide built-in commands: cd, jobs, exit

Handle signals to prevent zombie processes

⚙️ Features

✅ Run standard Linux commands (ls, pwd, date, echo, etc.)
✅ Background job handling with & and jobs
✅ Input/output redirection
✅ Command piping between two processes
✅ Built-in commands (cd, exit, jobs)
✅ Colored prompt displaying current directory
✅ Signal handling for background process completion



📂 Project Structure
custom_shell/
 ├── main.cpp
 ├── Makefile
 ├── README.md
 └── screenshots/
      ├── compile_success.png
      ├── run_demo.png
      ├── cd_navigation.png
      ├── background_process.png
      ├── redirection_piping.png
