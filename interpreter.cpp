#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <cctype>

// Token types enumeration
enum class TokenType {
    NUMBER,
    STRING,
    IDENTIFIER,
    KEYWORD,
    PLUS,
    MINUS,
    MULT,
    DIV,
    ASSIGN,
    EQUAL,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    SEMICOLON,
    END,
    UNKNOWN
};

// Token structure
struct Token {
    TokenType type;
    std::string value;
};

// Lexer class
class Lexer {
public:
    Lexer(const std::string& source) : source(source), current(0) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (current < source.length()) {
            char c = source[current];

            // Skip whitespace
            if (isspace(c)) {
                current++;
                continue;
            }

            // Handle numbers
            if (isdigit(c)) {
                std::string value;
                while (current < source.length() && isdigit(source[current])) {
                    value += source[current++];
                }
                tokens.push_back({TokenType::NUMBER, value});
                continue;
            }

            // Handle strings
            if (c == '"') {
                current++;
                std::string value;
                while (current < source.length() && source[current] != '"') {
                    value += source[current++];
                }
                if (current < source.length()) current++; // Skip closing quote
                tokens.push_back({TokenType::STRING, value});
                continue;
            }

            // Handle identifiers and keywords
            if (isalpha(c) || c == '_') {
                std::string value;
                while (current < source.length() && (isalnum(source[current]) || source[current] == '.' || source[current] == '_')) {
                    value += source[current++];
                }
                if (value == "let" || value == "const" || value == "function" || value == "console") {
                    tokens.push_back({TokenType::KEYWORD, value});
                } else {
                    tokens.push_back({TokenType::IDENTIFIER, value});
                }
                continue;
            }

            // Handle single-character symbols
            switch (c) {
                case '+': tokens.push_back({TokenType::PLUS, "+"}); current++; break;
                case '-': tokens.push_back({TokenType::MINUS, "-"}); current++; break;
                case '*': tokens.push_back({TokenType::MULT, "*"}); current++; break;
                case '/': tokens.push_back({TokenType::DIV, "/"}); current++; break;
                case '=':
                    if (current + 1 < source.length() && source[current + 1] == '=') {
                        tokens.push_back({TokenType::EQUAL, "=="});
                        current += 2;
                    } else {
                        tokens.push_back({TokenType::ASSIGN, "="});
                        current++;
                    }
                    break;
                case '(': tokens.push_back({TokenType::LPAREN, "("}); current++; break;
                case ')': tokens.push_back({TokenType::RPAREN, ")"}); current++; break;
                case '{': tokens.push_back({TokenType::LBRACE, "{"}); current++; break;
                case '}': tokens.push_back({TokenType::RBRACE, "}"}); current++; break;
                case ';': tokens.push_back({TokenType::SEMICOLON, ";"}); current++; break;
                default:
                    std::cerr << "Unexpected character: " << c << std::endl;
                    tokens.push_back({TokenType::UNKNOWN, std::string(1, c)});
                    current++;
                    break;
            }
        }
        tokens.push_back({TokenType::END, ""});

        // Debug: Print out tokens
        for (const auto& token : tokens) {
            std::cout << "Token: " << token.value << " (" << static_cast<int>(token.type) << ")\n";
        }

        return tokens;
    }

private:
    std::string source;
    size_t current;
};

// Evaluator class
class Evaluator {
public:
    int evaluate(const std::vector<Token>& tokens) {
        current = 0;
        while (current < tokens.size()) {
            parseStatement(tokens);
        }
        return 0;
    }

private:
    size_t current;
    std::map<std::string, int> variables;

    void parseStatement(const std::vector<Token>& tokens) {
        if (current < tokens.size() && tokens[current].type == TokenType::KEYWORD && tokens[current].value == "let") {
            current++; // Skip 'let'
            if (current < tokens.size() && tokens[current].type == TokenType::IDENTIFIER) {
                std::string name = tokens[current].value;
                current++; // Skip variable name

                if (current < tokens.size() && tokens[current].type == TokenType::ASSIGN) {
                    current++; // Skip '='
                    int value = parseExpression(tokens);
                    variables[name] = value;

                    if (current < tokens.size() && tokens[current].type == TokenType::SEMICOLON) {
                        current++; // Skip ';'
                    } else {
                        throw std::runtime_error("Expected ';' after assignment");
                    }
                } else {
                    throw std::runtime_error("Expected '=' after variable name");
                }
            } else {
                throw std::runtime_error("Expected variable name after 'let'");
            }
        } else if (current < tokens.size() && tokens[current].type == TokenType::KEYWORD && tokens[current].value == "console") {
            current++; // Skip 'console'
            if (current < tokens.size() && tokens[current].type == TokenType::IDENTIFIER && tokens[current].value == "log") {
                current++; // Skip 'log'
                if (current < tokens.size() && tokens[current].type == TokenType::LPAREN) {
                    current++; // Skip '('
                    int value = parseExpression(tokens);
                    std::cout << value << std::endl;
                    if (current < tokens.size() && tokens[current].type == TokenType::RPAREN) {
                        current++; // Skip ')'
                    } else {
                        throw std::runtime_error("Expected closing parenthesis for console.log()");
                    }
                } else {
                    throw std::runtime_error("Expected opening parenthesis after console.log");
                }
            }
        } else {
            parseExpression(tokens);
        }
    }

    int parseExpression(const std::vector<Token>& tokens) {
        return parsePrimary(tokens);
    }

    int parsePrimary(const std::vector<Token>& tokens) {
        if (current < tokens.size()) {
            if (tokens[current].type == TokenType::NUMBER) {
                int value = std::stoi(tokens[current].value);
                current++;
                return value;
            } else if (tokens[current].type == TokenType::STRING) {
                // Just return the length of the string as a placeholder for now
                int value = tokens[current].value.length();
                current++;
                return value;
            } else if (tokens[current].type == TokenType::IDENTIFIER) {
                std::string name = tokens[current].value;
                current++;
                if (variables.find(name) != variables.end()) {
                    return variables[name];
                } else {
                    throw std::runtime_error("Undefined variable: " + name);
                }
            } else {
                throw std::runtime_error("Unexpected token in primary expression.");
            }
        }
        throw std::runtime_error("Unexpected end of tokens in primary expression.");
    }
};


// Function to read a file into a string
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

// Main function
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path-to-.stine-file>" << std::endl;
        return 1;
    }

    try {
        std::string source = readFile(argv[1]);
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();

        Evaluator evaluator;
        evaluator.evaluate(tokens);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
