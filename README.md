# EE209 Assignment 5: A Unix Shell (`ish`)

## Name  
- Kwon Larry

## Partner  
- Worked individually (eligible for Extra Credit 2)

### Score
- 110 out of 100 (With Extra credit)

---
## Description
This project is a minimal but functional Unix shell called `ish`, developed for KAIST EE209: Programming Structures for EE. It supports interactive command parsing and execution, including:

- Lexical analysis of user input
- Parsing and execution of built-in commands: `cd`, `exit`, `setenv`, `unsetenv`
- Execution of external commands using `fork` and `execvp`
- Pipe (`|`) support to chain multiple commands
- Signal handling for `SIGINT` and `SIGQUIT`
- Initialization with `.ishrc` file from `$HOME`
- Error handling with informative messages

**Extra Credit**
- ✅ Input/output redirection using `<` and `>` (Extra Credit 1)
- ✅ Worked individually (Extra Credit 2)

---

## File Structure

```
.
├── ish.c                 # Main shell logic
├── lexer.c/.h           # Tokenizer module
├── parser.c/.h          # Command parser
├── executor.c/.h        # Process and execution logic
├── builtin.c/.h         # Built-in command handlers
├── redirection.c/.h     # I/O redirection handling
├── signal.c/.h          # Signal handling (SIGINT, SIGQUIT)
├── dynarray.c/.h        # Dynamic array (from course materials)
├── Makefile             # Build automation script
└── README.md            # Project information
```

---

## Build Instructions

To compile the project, run:

```bash
make
```

To clean up build artifacts:

```bash
make clean
```

This will generate the `ish` executable using `gcc209`.

---

## Usage

Start the shell with:

```bash
./ish
```

If a readable `.ishrc` file exists in `$HOME`, `ish` will execute its contents at launch.

After that, you can interactively type commands at the `% ` prompt.

---

## Features Implemented

| Feature                           | Implemented |
|----------------------------------|-------------|
| Built-in commands                | ✅          |
| Command execution via `execvp`   | ✅          |
| Pipe (`|`) support                | ✅          |
| Redirection `<`, `>`             | ✅ (Extra Credit 1) |
| `.ishrc` parsing                  | ✅          |
| Signal Handling (`SIGINT`, `SIGQUIT`) | ✅    |
| Memory management (no leaks)     | ✅          |
| Worked alone (Extra Credit 2)    | ✅          |