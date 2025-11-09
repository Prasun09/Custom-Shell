#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
using namespace std;

vector<pid_t> bgProcesses;

void handle_sigchld(int sig) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        cout << "\n[+] Background process " << pid << " finished\n";
    }
}

void printPrompt() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    cout << "\033[1;32mmyshell:\033[0m" << cwd << "$ ";
}

vector<string> tokenize(string input) {
    vector<string> tokens;
    stringstream ss(input);
    string token;
    while (ss >> token) tokens.push_back(token);
    return tokens;
}

bool isBackground(vector<string>& args) {
    if (!args.empty() && args.back() == "&") {
        args.pop_back();
        return true;
    }
    return false;
}

void executeCommand(vector<string>& args, bool bg) {
    if (args.empty()) return;

    // Built-in commands
    if (args[0] == "exit") {
        cout << "Exiting myshell...\n";
        exit(0);
    } else if (args[0] == "cd") {
        if (args.size() > 1) chdir(args[1].c_str());
        else chdir(getenv("HOME"));
        return;
    } else if (args[0] == "jobs") {
        cout << "Background Processes:\n";
        for (pid_t pid : bgProcesses) cout << "PID: " << pid << endl;
        return;
    }

    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        // Handle I/O redirection
        for (int i = 0; i < args.size(); i++) {
            if (args[i] == "<") {
                int fd = open(args[i+1].c_str(), O_RDONLY);
                dup2(fd, STDIN_FILENO);
                close(fd);
                args.erase(args.begin() + i, args.begin() + i + 2);
                i--;
            } else if (args[i] == ">") {
                int fd = open(args[i+1].c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                dup2(fd, STDOUT_FILENO);
                close(fd);
                args.erase(args.begin() + i, args.begin() + i + 2);
                i--;
            } else if (args[i] == ">>") {
                int fd = open(args[i+1].c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
                dup2(fd, STDOUT_FILENO);
                close(fd);
                args.erase(args.begin() + i, args.begin() + i + 2);
                i--;
            }
        }

        // Handle pipe
        auto it = find(args.begin(), args.end(), "|");
        if (it != args.end()) {
            vector<string> left(args.begin(), it);
            vector<string> right(it + 1, args.end());
            int fd[2];
            pipe(fd);

            pid_t pid1 = fork();
            if (pid1 == 0) {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
                vector<char*> argv;
                for (auto &a : left) argv.push_back(&a[0]);
                argv.push_back(NULL);
                execvp(argv[0], argv.data());
                perror("execvp failed");
                exit(1);
            }

            pid_t pid2 = fork();
            if (pid2 == 0) {
                dup2(fd[0], STDIN_FILENO);
                close(fd[0]);
                close(fd[1]);
                vector<char*> argv;
                for (auto &a : right) argv.push_back(&a[0]);
                argv.push_back(NULL);
                execvp(argv[0], argv.data());
                perror("execvp failed");
                exit(1);
            }

            close(fd[0]);
            close(fd[1]);
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
            exit(0);
        }

        // Execute normal command
        vector<char*> argv;
        for (auto &a : args) argv.push_back(&a[0]);
        argv.push_back(NULL);
        execvp(argv[0], argv.data());
        perror("execvp failed");
        exit(1);
    }
    else if (pid > 0) {
        if (bg) {
            bgProcesses.push_back(pid);
            cout << "[+] Process running in background (PID: " << pid << ")\n";
        } else {
            waitpid(pid, NULL, 0);
        }
    } else {
        perror("fork failed");
    }
}

int main() {
    signal(SIGCHLD, handle_sigchld);
    string input;

    while (true) {
        printPrompt();
        getline(cin, input);
        if (input.empty()) continue;

        vector<string> args = tokenize(input);
        bool bg = isBackground(args);
        executeCommand(args, bg);
    }
    return 0;
}
