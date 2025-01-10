# PseudoLang Compiler
### NOT COMPLETE - IN PROGRESS
This project is a pre-course exploration of compiler design, created to benchmark my current understanding of compilers against the skills I'll aquire by the end of my upcoming compiler design course. By building this project now, I aim to identify areas for growth and demonstrate progress over the semester.

## Overview
The **PseudoLang Language Compiler** is a simple compiler that translates a custom programming language, PseudoLang,  into C++ code. It performs lexical and syntax analysis on the input and generates equivalent C++ source code, which can be compiled and executed using standard tools like GCC or Clang.

### Features
- Tokenizes PseudoLang
- Parses basic programming constructs (e.g., variable declarations and arithmetic operations).
- Ouputs valid C++ source code

## Purpose
The main objective of this project is to serve as a **baseline** for evaluating my growth in compiler desing and give me a surface-level understanding of compilers before taking the course.
- **Before the course**: Focus on foundational conepts such as lexical analysis, parsing, and basic code generation.
- **After the course**: Expand functionality, incorporate advanced concepts like semantic analysis, and optimization.

This comparison wil highlight how much I've learned throughout the semester.

## Example
### Input (PseudoLang):
```
x <- 5;
y <- 10;
z <- x + y;
```

### Output (Generated C++):
```cpp
#include <iostream>

int main() {
  int x = 5;
  int y = 10;
  int z = x + y;
  return 0;
}
```
Compile the generated C++ code with:  
```
g++ output.cpp -o output
./output
```

## Project Goals
1. Develop a foundational understanding of compiler design ocncepts.
2. Provide a point of comparison for assessing progress after completing the compiler design course
3. Showcase versatility in programming through a project in C++.

## File Structure  
```plaintext
📂 my-first-compiler  
├── 📂 src  
│   ├── lexer.cpp           # Tokenizer implementation
│   ├── lexer.h             # Tokenizer header
│   ├── parser.cpp          # Syntax parser
│   ├── parser.h            # Syntax parser header
│   ├── codegen.cpp         # C++ code generation
│   ├── codegen.h           # C++ code generation header
│   ├── token.h             # Token definitions
│   ├── token_scanner.cpp   # Token scanner implementation
│   ├── token_scanner.h     # Token scanner header
│   ├── keyword_manager.cpp # Keyword manager implementation
│   ├── keyword_manager.h   # Keyword manager header
│   └── main.cpp            # Entry point of the compiler
├── 📂 examples  
│   ├── example1.pseudo     # Sample PseudoLang file
│   ├── example2.pseudo
│   └── example3.pseudo
├── 📂 docs
│   ├── syntax.md           # Syntax for PseudoLang language
│   └── AST.md              # Abstract Syntax Tree
├── README.md               # Project overview    
```

## Getting started
### Prerequisites
- **C++ compiler**: GCC, Clang, or MSVC.
- **Build Tools**: Make (optional)

### Build and Run
1. Clone the repository:
```
git clone https://github.com/wowitsalison/my-first-compiler.git
cd my-first-compiler
```
2. Build the compiler:
```
make
```
3. Compile a sample PseudoLang file:
```
./my-first-compiler examples/example1.pseudo -o output.cpp
g++ output.cpp -o output
./output
```

## License
This project is open-source and available under the MIT License.
