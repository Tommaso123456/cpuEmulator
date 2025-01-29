# CPU Emulator

There is only one commit to this project because it is a class project and was done on a university account.

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
