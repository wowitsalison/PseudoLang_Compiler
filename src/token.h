#pragma once
#include <string>

enum class TokenType {
    IF,
    ELSE,
    ELSEIF,
    WHILE,
    DECLARE,
    PUT,
    THEN,
    END_IF,
    LOOP,
    END_LOOP,
    PROCEDURE, 
    BEGIN,
    END,
    END_PROCEDURE,
    RETURN,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    EQUAL,
    NOT_EQUAL,
    ASSIGN,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_BRACE,
    CLOSE_BRACE,
    COMMA,
    SEMICOLON,
    STRING,
    NUMBER,
    IDENTIFIER,
    COMMENT,
    UNKNOWN,
    END_OF_FILE,
    UNTERMINATED_STRING
};

class Token {
public:
    Token(TokenType type, const std::string& lexeme, int line, int column)
        : type(type), lexeme(lexeme), line(line), column(column) {}

    TokenType type;
    std::string lexeme;
    int line;
    int column;
};