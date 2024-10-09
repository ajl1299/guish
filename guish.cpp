#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h>
#include <algorithm>

using namespace std;

void parse_args(string line, vector<string> &cmds)
{
    stringstream liness(line);

    string token;
    while (getline(liness, token, ' '))
    {
        cmds.push_back(token);
    }
}

void executeCmd(const string &cmd)
{
    // fork child and execute program
    pid_t pid = fork();
    int status;
    if (pid < 0)
    {
        // error in forking
        cerr << "Fork Error Occured\n"
             << endl;
    }
    else if (pid == 0)
    // child process
    {
        vector<string> cmd_args;
        parse_args(cmd, cmd_args);

        char *argv[cmd_args.size() + 1];
        for (int i = 0; i < cmd_args.size(); i++)
        {
            argv[i] = &cmd_args[i][0];
        }
        argv[cmd_args.size()] = nullptr;

        if (execvp(argv[0], argv) < 0)
        {
            perror("Error with executing command");
        }
        exit(0);
    }
    else
    {
        // wait for program to finish and print exit status
        waitpid(pid, &status, 0);
    }
}

bool isValidCommand(const string &cmd)
{
    vector<string> validCommands = {"ls", "cd", "pwd", "echo", "cat", "touch", "mkdir", "exit", "hist", "help"};

    return find(validCommands.begin(), validCommands.end(), cmd) != validCommands.end();
}
void addToHistory(vector<string> &history, const string &cmd)
{
    if (isValidCommand(cmd))
    {
        history.push_back(cmd); // add commands to history vector
    }

    if (history.size() > 10)
    {
        history.erase(history.begin()); // if history vector is full remove the least recently used element
    }
}
int main(void)
{
    int argc = 0;
    vector<string> history;
    string rnCommand;

    while (1)
    {
        // prompt
        cout << "ajlsh$ ";

        string cmd;
        getline(cin, cmd);

        if (isValidCommand(cmd))
        {
            addToHistory(history, cmd);
        }

        // ignore empty input
        if (cmd == "" || cmd.size() == 0)
            continue;

        cout << "Received user commands: ";
        cout << cmd << endl;

        // built-in: exit
        if (cmd == "help")
        {
            cout << "Use 'exit' to exit the shell" << endl;
            cout << "Use 'hist' to display up to the last 10 commands used" << endl;
            cout << "Use 'r n' where n is the nth element of the history list to run that command from the history" << endl;

            continue;
        }
        else if (cmd == "exit")
        {
            exit(0);
        }
        // hist
        if (cmd == "hist")
        {
            cout << "History: " << endl;
            for (int i = 0; i < (history.size()); i++)
            {
                cout << i + 1 << " " << history[i] << endl;
            }
            continue;
        }

        // r n
        if (cmd[0] == 'r' && isdigit(cmd[2]))
        {
            int n = stoi(cmd.substr(2)); // store the nth element they wish to access as an int
            if (n > 0 && n <= static_cast<int>(history.size()))
            {
                rnCommand = history[n - 1];
                cout << "Received user commands: " << rnCommand << endl;
                addToHistory(history, rnCommand);
                if (rnCommand == "help")
                {
                    cout << "Use 'exit' to exit the shell" << endl;
                    cout << "Use 'hist' to display up to the last 10 commands used" << endl;
                    cout << "Use 'r n' where n is the nth element of the history list to run that command from the history" << endl;

                    continue;
                }
                else if (rnCommand == "hist")
                {
                    {
                        cout << "History: " << endl;
                        for (int i = 0; i < (history.size()); i++)
                        {
                            cout << i + 1 << " " << history[i] << endl;
                        }
                        continue;
                    }
                }
                else
                {
                    executeCmd(rnCommand);
                }
            }
            else
            {
                cerr << "Nth element is out of bounds. Try again." << endl;
            }
            continue;
        }

        pid_t pid = fork();
        int status;
        if (pid < 0)
        {
            cerr << "Fork Error Has Occurred\n"
                 << endl;
        }
        else if (pid == 0)
        {
            executeCmd(cmd);
            exit(0);
        }
        else
        {
            waitpid(pid, &status, 0);
        }
    }
}
