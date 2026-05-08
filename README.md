# Custom Unix Shell

A custom Unix shell written in C that supports command execution, process management, built-in commands, batch mode execution, and output redirection. This project was built to strengthen understanding of Linux system calls, operating systems concepts, and process control.

---

## Features

- Interactive shell prompt (`msh>`)
- Batch mode execution from files
- Built-in commands:
  - `cd`
  - `exit`
  - `quit`
- Command execution using:
  - `fork()`
  - `execv()`
  - `waitpid()`
- Output redirection using `>`
- Command parsing with argument handling
- Path searching through:
  - `/bin`
  - `/usr/bin`
  - `/usr/local/bin`
  - current directory
- Error handling for invalid commands and syntax

---

## Technologies Used

- C
- Linux
- Git/GitHub
- Unix System Calls

---

## Concepts Learned

This project helped reinforce:
- Process creation and management
- Parent/child process behavior
- System calls and low-level OS interaction
- Shell command parsing
- File descriptors and redirection
- Linux programming fundamentals

---

## Example Commands

```bash
msh> ls -la
msh> cd ..
msh> cat file.txt
msh> ls > output.txt
```

---

## Building the Project

Compile using:

```bash
make
```

---

## Running the Shell

Interactive mode:

```bash
./msh
```

Batch mode:

```bash
./msh batch.txt
```

---

## Project Structure

```text
msh/
├── msh.c
├── Makefile
├── test-msh.sh
└── README.md
```

---

## Future Improvements

- Command history support
- Pipe support (`|`)
- Background process execution (`&`)
- Environment variable support
- Auto-completion

---

## Author

Muhammad Zahid
