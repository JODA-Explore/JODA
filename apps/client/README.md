# Joda Client - JSON On-Demand Analysis

A simple client to connect to a Joda server.

## Program Flags
These are options that may be supplied to the program during invokation.

- __-h --help__:        Shows help text and exits the program.
- __--version__:        Displays the version number
- __-a --address__:     The address of the server to connect to. (Required)
- __-p --port__:        The port of the server to connect to .(Required)
- __-q --query__:       Executes the given query in non-interactive mode. Useful for piping  the results to a file.
- __-o --offset__:      (Only with --query) How many results to skip. (Default: 0)
- __-c --count__:       (Only with --query) How many results to retrieve. (Default: all)

## Usage
When starting the client in normal (interactive) mode a query can be entered.
This will be executed on the server and the results will be shown to the user.

Single results can be scrolled with the w,a,s,d keys.
To browse through the results the left and right arrow keys can be used.
'q' quits the result viewer.

For an explanation of possible queries see the main readme.

### Commands
The client understands the following commands (followed by an `;`):

- `quit`        Quits the program.
- `<QUERY>`    Every query understood by the server can be entered.