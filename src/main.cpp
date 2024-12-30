#include <iostream>
#include <fstream>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];

    // Open PseudoLang source file
    std::ifstream sourceFile(filename);
    if (!sourceFile) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        return 1;
    }

    // Read the file into a string
    std::string sourceCode((std::istreambuf_iterator<char>(sourceFile)), 
                            std::istreambuf_iterator<char>());

    // Perform lexical analysis (tokenize)
    Lexer lexer(sourceCode);
    auto tokens = lexer.tokenize();

    // Parse the tokens
    Parser parser(tokens);
    auto ast = parser.parse();

    // Generate C++ code
    CodeGenerator codegen(ast);
    std::string cppCode = codegen.generate();

    // Write the C++ code to a file
    std::string outputCppFile = "output.cpp";
    std::ofstream cppFile(outputCppFile);
    if(!cppFile) {
        std::cerr << "Error: Could not open file " << outputCppFile << "\n";
        return 1;
    }
    cppFile << cppCode;
    cppFile.close();

    // Compile the generated C++ code
    std::string compileCommand = "g++ " + outputCppFile + " -o output";
    if (system(compileCommand.c_str()) != 0) {
        std::cerr << "Error: Could not compile the generated C++ code\n";
        return 1;
    }

    std::cout << "Compilation successful! Run the program with '/.output'\n";
    return 0;
}
