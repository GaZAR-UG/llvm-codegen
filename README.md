# llvm-codegen
This is a minimal LLVM codegen example that allows one to experiment with the LLVM infrastructure for generating LLVM IR.

## How to Build the Project?

Build the project using `$ make`.

Delete the auto-generated files using `$ make clean`.

## Running the Program and Generating Code

Use `./codegen out.bc` to run the small codegen program and to produce some LLVM intermediate representation that is written to the `out.bc` bitcode file.

One can compile the generated bitcode file to an executable binary using `$ clang++ -Wall -Wextra out.bc -o out`.

The return code of the program can be checked (after running it) using `$ echo $?`.
