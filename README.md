# LilShell

The minishell project from 42 School.

This is a simple reimplementation of the bash unix shell, written in C with [yLib](https://github.com/IgorGRBR/ylib) and [readline](https://tiswww.case.edu/php/chet/readline/rltop.html) libraries, in accordance to the 42 School Norm v4.

This shell supports all of the features described within the minishell document:
* Executable/binary invocation based on the `PATH` variable
* Separating commands (and their sequential execution) with `;`
* Forming strings with `'`, and interpolating environment variables into them if formed with `"`
* Redirections with `<`, `>`, `<<`, `>>`
* Pipes (with parallel execution)
* `ctrl-C`, `ctrl-D` and `ctrl-\` signal handling
* Command history
* Logical operators `&&` and `||` that work with program exit codes
* Wildcards
* `echo`, `cd`, `pwd`, `export`, `unset`, `env` and `exit` builtins

## Building instructions

*I only tested building and running this game on Fedora 39 and Ubuntu 20.04.* 

### Building on Linux

Make sure you have installed all the necessary dependencies:

* On Fedora: `sudo dnf install make clang readline-devel`

* On Debian/Ubuntu: `TODO`

After installing dependencies, `cd` into the repository directory, and then run `make`.

## Running the shell

To start the shell in interactive mode, simply run `./minishell` in the repository directory after building it. To exit interactive mode, simply invoke `exit` or press `ctrl-D`. You can also execute a single command and immediately exit the shell by providing such command as a string in the first argument to the program.
