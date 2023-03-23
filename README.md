# riscv-emul

## RISC-V Emulator
Emulate the behavior of the RISC-V core (`RV32IM`)

## Input
Using cross-compiled binary files from `riscv-software-src/riscv-tests`

## Output
Print values in registers and PC during instruction execution

## Usage
```
$ make
```

Just test a binary file
```
$ ./main --binary=./bin/<name>.bin
```

Test a binary file with print option
```
$ ./main --binary=./bin/<name>.bin --print=true
```

Test all binary files in `bin` folder
```
$ ./test.py bin
```

