#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_map>

enum class TokenType {
    IDENTIFIER,         // variable names, function names
    NUMBER,             // integer numbers
    STRING,             // string literals
    END_OF_FILE,        // end of file
    IF,                 // if keyword
    ELSE,               // else keyword
    WHILE,              // while keyword
    DECLARE,            // var declaration
    PLUS,               // +
    MINUS,              // -
    STAR,               // *
    SLASH,              // /
    EQUAL,              // =
    NOT_EQUAL,          // !=
    LESS,               // <
    GREATER,            // >
    LESS_EQUAL,         // <=
    GREATER_EQUAL,      // >=
    ASSIGN,             // <-
    OPEN_PAREN,         // (
    CLOSE_PAREN,        // )
    OPEN_BRACE,         // {
    CLOSE_BRACE,        // }
    COMMA,              // ,
    SEMICOLON,          // ;
    COMMENT,            // // comment
    UNKNOWN             // unrecognized tokens
};

struct Token {
    TokenType type;     // type of token
    std::string value;  // value of token
    int line;           // line number
    int column;         // column number

    Token(TokenType type, const std::string& value, int line, int column) 
        : type(type), value(value), line(line), column(column) {}
};

class Lexer {
public:
    Lexer(const std::string& sourceCode);
    std::vector<Token> tokenize();
private:
    std::string sourceCode;
    int currentPosition;
    int line;
    int column;

    // Create a hashmap to store keywords
    std::unordered_map<std::string, TokenType> keywordMap; // Keyword map
    void initializeKeywordMap();                           // Initialize keyword map

    char peek() const;                                     // Peek at the current character
    char advance();                                        // Advance to the next character
    bool isAtEnd() const;                                  // Check if at the end of the source code
    void skipWhitespace();                                 // Skip whitespace
    Token scanToken();                                     // Process the next token
    Token handleIdentifierOrKeyword(char firstChar);       // Handle identifiers and keywords
    Token handleNumber(char firstChar);                    // Handle numeric literals
    Token handleString();                                  // Handle string literals
};

#endif // LEXER_H