# CS 3650 Lab 1 - emulate the 3650 CPU

See [CPU Description](cpu-description.pdf) for a detailed description of the instruction set to emulate.

You should modify the following files:
- `emulate.c` - this is your CPU emulator
- `test.c` - this is the start of a test skeleton for your code

Additional files provided:
- `lab1.h` - defines `struct cpu` and a few other things
- `Makefile` - so the `make` command will work
- `asm.py` - assembler, translates assembly language to binary
- `hello.asm`, `hello.bin` - "Hello world!" in 6500 assembler
- `runsim.c` - runs your simulation
- `disasm.c` - disassembler
- `slack.txt` - see slack hours description below

### Deliverable

We will grade the most recent version of your code in your repository as of the assignment due date. Remember to run `git push` to copy your commits to the server.

**late assignments:** You have 120 "slack hours" to use over the course of the semester. If you finish your assignment late, update the `slack.txt` in your repository to account for the extra time used. (Note that your accounting should match the timing of your Git checkins within the nearest hour or so.)

### Git commits / pushes

You are expected to run `git commit` **frequently**, i.e. at least every few hours. This is not only good practice, but also serves as a record that you developed the code yourself rather than copying it from a classmate.

The command `git commit -a` will commit all modified files, starting  up an editor for you to record a commit note. It's good practice to  provide a note which will remind you what you changed, in case you  need to look back through versions; if you don't write anything, it  won't let you commit.

### Academic Integrity

This assignment can be tedious, but isn't that hard once you get the hang of it. If you are in enough trouble that you're thinking of plagiarizing from another student, please contact me (Prof Desnoyers) or post to Instructors on Piazza, rather than trying to borrow code from someone else - there's a high chance that we'll catch you.

It's ok to talk to other students about your code, but do not email, copy, text, post, or otherwise transmit any code from `emulate.c` to another student or receive code yourself. You are free to share code from `test.c` with other students, though, either directly or via Piazza.

### How to compile things

You can use the `make` command to compile the three binaries:
```
~/base-lab1$ make runsim disasm test
cc -g   -c -o emulate.o emulate.c
cc -g   -c -o runsim.o runsim.c
cc -g   -c -o disasm.o disasm.c
cc -g -o runsim emulate.o runsim.o disasm.o
cc -g -o disasm -DSTANDALONE disasm.c
cc -g   -c -o test.o test.c
cc -g -o test test.o emulate.o
```

You can delete the object files and executables with the `make clean` command. (e.g. before uploading to gradescope)

### `runsim`
The `runsim` command takes a binary file as an argument, and two optional parameters:
```
    runsim [-v] [-r] filename.bin
```

If given the `-v` option ("verbose") it will disassemble each instruction and print it to the terminal before executing it.

The `-r` ("registers") option causes it to print all registers after the instruction runs. 

### testing

You are expected to test your own code to make sure it works on all instructions, not just the ones in `hello.asm`. 

The file `test.c` shows how you might be able to do this. Note that you may find it helpful to use the assembler to create these examples, using the `-l` flag:
- `-l "string"` - assemble the provided string as a single line of code and write hex words to terminal
 e.g. here's how I created the tests in `test.c`:
```
pjd@mini hw1 % python3 asm.py -l 'ADD R3 + R6 -> R1'
ADD R3 + R6 -> R1 : 5073 
pjd@mini hw1 % python3 asm.py -l 'STORE.B R2 -> *0x0008'
STORE.B R2 -> *0x0008 : 3402 0008 
```
