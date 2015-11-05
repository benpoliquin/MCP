# Master Control Program

This MCP is a scaled down model of the Burroughs MCP.
  - See: https://en.wikipedia.org/wiki/Burroughs_MCP

## Description
The Master Control Program is a lightweight version of the Burroughs MCP that can fork a process and run the child processes concurrently. (Not truly concurrent but it gives the illusion) Each process is given 1 second at a time and if the process does not complete it pauses it and lets the next process run before cycling back and letting it continue if it has not finished. Once all child processes complete and exit, the parent process finishes and the program exits. For each child process, information from the PROC file is printed out (similar to the 'top' command in unix).

### Additional Libraries Needed
 - Libprocps : Needed to display the PROC information 

Open a terminal and type:

    sudo apt-get install libprocps3-dev

And when you compile, you'll have to use the following flag (just use the Makefile take care of this part):

    gcc mcp.c -lprocps

### To Execute
Open your favorite Terminal and run these commands.

First:
```sh
$ make
```

Second:
```sh
$ ./mcp input_file.txt
```
Note:
* The input_file.txt can contain any bash commands. To run them on different child processes just put different commands on different lines in the text file. You can even add python programs for exampe and run multiple real programs!

### Todos

 - Cleanup comments


   [benpoliquin]: <https://github.com/benpoliquin>

