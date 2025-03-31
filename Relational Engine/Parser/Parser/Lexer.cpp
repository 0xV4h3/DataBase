#include "Lexer.h"
#include <iostream>

//==================== Create keyword map ====================
std::unordered_map<std::string, Keyword> Lexer::createKeywordMap() {
    std::unordered_map<std::string, Keyword> map;
    static const std::pair<const char*, Keyword> keywordList[] = {
        {"SELECT", Keyword::SELECT},
        {"FROM", Keyword::FROM},
        {"WHERE", Keyword::WHERE},
        {"INSERT", Keyword::INSERT},
        {"UPDATE", Keyword::UPDATE},
        {"DELETE", Keyword::DELETE},
        {"CREATE", Keyword::CREATE},
        {"DROP", Keyword::DROP},
        {"ALTER", Keyword::ALTER},
        {"JOIN", Keyword::JOIN},
        {"ON", Keyword::ON},
        {"GROUP", Keyword::GROUP},
        {"BY", Keyword::BY},
        {"HAVING", Keyword::HAVING},
        {"ORDER", Keyword::ORDER},
        {"LIMIT", Keyword::LIMIT},
        {"OFFSET", Keyword::OFFSET},
        {"UNION", Keyword::UNION},
        {"ALL", Keyword::ALL},
        {"DISTINCT", Keyword::DISTINCT},
        {"AS", Keyword::AS},
        {"IN", Keyword::IN},
        {"IS", Keyword::IS},
        {"NULL", Keyword::NULL_KEYWORD},
        {"TRUE", Keyword::TRUE_KEYWORD},
        {"FALSE", Keyword::FALSE_KEYWORD}
    };

    for (const auto& [str, kw] : keywordList) {
        map[str] = kw;
    }
    return map;
}

const std::unordered_map<std::string, Keyword> Lexer::keywordMap = Lexer::createKeywordMap();

Keyword Lexer::getKeyword(const std::string& word) {
    std::string upperWord = word;
    std::transform(upperWord.begin(), upperWord.end(), upperWord.begin(), ::toupper);
    auto it = keywordMap.find(upperWord);
    return (it != keywordMap.end()) ? it->second : Keyword::UNKNOWN;
}

//==================== Constructor and helper functions ====================
Lexer::Lexer(const std::string& query) : input(query), pos(0) {}

void Lexer::skipWhitespace() {
    while (pos < input.size() && std::isspace(input[pos])) {
        pos++;
    }
}

std::string Lexer::tokenTypeToString(TokenType type) {
    switch (type) {
    case TokenType::KEYWORD:      return "KEYWORD";
    case TokenType::IDENTIFIER:   return "IDENTIFIER";
    case TokenType::LITERAL:      return "LITERAL";
    case TokenType::OPERATOR:     return "OPERATOR";
    case TokenType::SYMBOL:       return "SYMBOL";
    case TokenType::END_OF_FILE:  return "END_OF_FILE";
    case TokenType::UNKNOWN:      return "UNKNOWN";
    }
    return "UNKNOWN";
}

std::string Lexer::literalTypeToString(LiteralType type) {
    switch (type) {
    case LiteralType::STRING:     return "STRING";
    case LiteralType::CHAR:       return "CHAR";
    case LiteralType::INTEGER:    return "INTEGER";
    case LiteralType::FLOAT:      return "FLOAT";
    case LiteralType::BINARY:     return "BINARY";
    case LiteralType::HEX:        return "HEX";
    case LiteralType::DATE:       return "DATE";
    case LiteralType::TIME:       return "TIME";
    case LiteralType::DATETIME:   return "DATETIME";
    case LiteralType::JSON:       return "JSON";
    case LiteralType::BOOLEAN:    return "BOOLEAN";
    case LiteralType::NULL_VALUE: return "NULL_VALUE";
    case LiteralType::UNKNOWN:    return "UNKNOWN";
    }
    return "UNKNOWN";
}

std::string Lexer::operatorToString(Operator op) {
    switch (op) {
    case Operator::PLUS:           return "PLUS";
    case Operator::MINUS:          return "MINUS";
    case Operator::MULTIPLY:       return "MULTIPLY";
    case Operator::DIVIDE:         return "DIVIDE";
    case Operator::MOD:            return "MOD";
    case Operator::ASSIGN:         return "ASSIGN";
    case Operator::LESS:           return "LESS";
    case Operator::GREATER:        return "GREATER";
    case Operator::LESS_EQUAL:     return "LESS_EQUAL";
    case Operator::GREATER_EQUAL:  return "GREATER_EQUAL";
    case Operator::NOT_EQUAL:      return "NOT_EQUAL";
    case Operator::UNKNOWN:        return "UNKNOWN";
    }
    return "UNKNOWN";
}

std::string Lexer::symbolToString(Symbol sym) {
    switch (sym) {
    case Symbol::COMMA:      return "COMMA";
    case Symbol::SEMICOLON:  return "SEMICOLON";
    case Symbol::LPAREN:     return "LPAREN";
    case Symbol::RPAREN:     return "RPAREN";
    case Symbol::LBRACE:     return "LBRACE";
    case Symbol::RBRACE:     return "RBRACE";
    case Symbol::LBRACKET:   return "LBRACKET";
    case Symbol::RBRACKET:   return "RBRACKET";
    case Symbol::UNKNOWN:    return "UNKNOWN";
    }
    return "UNKNOWN";
}

//==================== Determine TokenType, Keyword, LiteralType =====================

TokenType Lexer::determineTokenType(const std::string& word) {
    LiteralType litType = determineLiteralType(word);
    if (litType != LiteralType::UNKNOWN)
        return TokenType::LITERAL;

    Keyword kw = getKeyword(word);
    if (kw != Keyword::UNKNOWN)
        return TokenType::KEYWORD;

    return TokenType::IDENTIFIER;
}

TokenType Lexer::determineTokenType(const size_t& start) {
    size_t temp = start;
    std::string word;
    while (temp < input.size() && (std::isalnum(input[temp]) || input[temp] == '_')) {
        word.push_back(input[temp]);
        temp++;
    }
    return determineTokenType(word);
}

Keyword Lexer::determineKeywordType(const size_t& start) {
    size_t posTemp = start;
    std::string word;
    while (posTemp < input.size() && (std::isalnum(input[posTemp]) || input[posTemp] == '_')) {
        word.push_back(input[posTemp]);
        posTemp++;
    }
    return determineKeywordType(word);
}

Keyword Lexer::determineKeywordType(const std::string& word) {
    std::string upperWord = word;
    std::transform(upperWord.begin(), upperWord.end(), upperWord.begin(), ::toupper);
    auto it = keywordMap.find(upperWord);
    return (it != keywordMap.end()) ? it->second : Keyword::UNKNOWN;
}

LiteralType Lexer::determineLiteralType(const size_t& start) {
    size_t posTemp = start;
    std::string word;
    while (posTemp < input.size() && !std::isspace(input[posTemp]) && !std::ispunct(input[posTemp])) {
        word.push_back(input[posTemp]);
        posTemp++;
    }
    return determineLiteralType(word);
}

LiteralType Lexer::determineLiteralType(const std::string& word) {
    if (isBinaryLiteral(word))    return LiteralType::BINARY;
    if (isHexLiteral(word))       return LiteralType::HEX;
    if (isDateLiteral(word))      return LiteralType::DATE;
    if (isTimeLiteral(word))      return LiteralType::TIME;
    if (isDateTimeLiteral(word))  return LiteralType::DATETIME;
    if (isJSONLiteral(word))      return LiteralType::JSON;
    if (isBooleanLiteral(word))   return LiteralType::BOOLEAN;
    if (isNullLiteral(word))      return LiteralType::NULL_VALUE;
    if (isFloatLiteral(word))     return LiteralType::FLOAT;
    if (isIntegerLiteral(word))   return LiteralType::INTEGER;
    if (word.size() >= 2 && word.front() == '"' && word.back() == '"')
        return LiteralType::STRING;
    if (word.size() >= 2 && word.front() == '\'' && word.back() == '\'') {
        return (word.size() == 3) ? LiteralType::CHAR : LiteralType::STRING;
    }
    return LiteralType::UNKNOWN;
}

Operator Lexer::determineOperatorType(const size_t& start) {
    size_t temp = start;
    std::string op;
    while (temp < input.size() && std::ispunct(input[temp])) {
        op.push_back(input[temp]);
        if (op.size() == 2) break;
        temp++;
    }
    return determineOperatorType(op);
}

Operator Lexer::determineOperatorType(const std::string& word) {
    if (word == "+")   return Operator::PLUS;
    if (word == "-")   return Operator::MINUS;
    if (word == "*")   return Operator::MULTIPLY;
    if (word == "/")   return Operator::DIVIDE;
    if (word == "%")   return Operator::MOD;
    if (word == "=")   return Operator::ASSIGN;
    if (word == "<")   return Operator::LESS;
    if (word == ">")   return Operator::GREATER;
    if (word == "<=")  return Operator::LESS_EQUAL;
    if (word == ">=")  return Operator::GREATER_EQUAL;
    if (word == "<>")  return Operator::NOT_EQUAL;
    return Operator::UNKNOWN;
}

Symbol Lexer::determineSymbolType(const size_t& start) {
    size_t temp = start;
    std::string sym;
    if (temp < input.size()) {
        sym.push_back(input[temp]);
    }
    return determineSymbolType(sym);
}

Symbol Lexer::determineSymbolType(const std::string& word) {
    if (word == ",")  return Symbol::COMMA;
    if (word == ";")  return Symbol::SEMICOLON;
    if (word == "(")  return Symbol::LPAREN;
    if (word == ")")  return Symbol::RPAREN;
    if (word == "{")  return Symbol::LBRACE;
    if (word == "}")  return Symbol::RBRACE;
    if (word == "[")  return Symbol::LBRACKET;
    if (word == "]")  return Symbol::RBRACKET;
    return Symbol::UNKNOWN;
}

//==================== Next token logic =====================
std::unique_ptr<Token> Lexer::nextToken() {
    skipWhitespace();

    if (pos >= input.size()) {
        return std::make_unique<Token>(Token{ TokenType::END_OF_FILE, "", static_cast<int>(pos) });
    }

    size_t start = pos;
    char current = input[pos];
    std::string word;

    if (std::isdigit(current) || (current == '-' && pos + 1 < input.size() && std::isdigit(input[pos + 1]))) {
        size_t oldPos = pos;
        if (input[pos] == '-') pos++; 
        while (pos < input.size() && std::isdigit(input[pos])) {
            pos++;
        }
        if (pos < input.size() && input[pos] == '.') {
            pos++; 
            while (pos < input.size() && std::isdigit(input[pos])) {
                pos++;
            }
        }
        word = input.substr(oldPos, pos - oldPos);
        LiteralType litType = (word.find('.') != std::string::npos) ? LiteralType::FLOAT : LiteralType::INTEGER;
        return std::make_unique<Token>(Token{ TokenType::LITERAL, word, static_cast<int>(oldPos), litType });
    }
    else if ((current == 'B' || current == 'b') && pos + 1 < input.size() && input[pos + 1] == '\'') {
        size_t oldPos = pos;
        pos += 2; 
        while (pos < input.size() && input[pos] != '\'') {
            pos++;
        }
        if (pos >= input.size() || input[pos] != '\'') {
            throw std::runtime_error("Unterminated binary literal at position " + std::to_string(oldPos));
        }
        pos++; 
        word = input.substr(oldPos, pos - oldPos);
        return std::make_unique<Token>(Token{ TokenType::LITERAL, word, static_cast<int>(oldPos), LiteralType::BINARY });
    }
    else if ((current == 'X' || current == 'x') && pos + 1 < input.size() && input[pos + 1] == '\'') {
        size_t oldPos = pos;
        pos += 2; 
        while (pos < input.size() && input[pos] != '\'') {
            pos++;
        }
        if (pos >= input.size() || input[pos] != '\'') {
            throw std::runtime_error("Unterminated hex literal at position " + std::to_string(oldPos));
        }
        pos++; 
        word = input.substr(oldPos, pos - oldPos);
        return std::make_unique<Token>(Token{ TokenType::LITERAL, word, static_cast<int>(oldPos), LiteralType::HEX });
    }
    else if (current == '"' || current == '\'') {
        char quoteChar = current;
        size_t oldPos = pos;
        pos++; 
        while (pos < input.size() && input[pos] != quoteChar) {
            if (input[pos] == '\\' && pos + 1 < input.size()) {
                pos += 2;
            }
            else {
                pos++;
            }
        }
        if (pos >= input.size() || input[pos] != quoteChar) {
            throw std::runtime_error("Unterminated literal at position " + std::to_string(oldPos));
        }
        pos++; 
        word = input.substr(oldPos, pos - oldPos);
    }
    else if (current == '{') {
        size_t oldPos = pos;
        int braceCount = 0;
        bool inString = false;
        char stringQuote = '\0';
        while (pos < input.size()) {
            char ch = input[pos];
            if (inString) {
                if (ch == '\\' && pos + 1 < input.size()) {
                    pos += 2;
                    continue;
                }
                else if (ch == stringQuote) {
                    inString = false;
                    pos++;
                    continue;
                }
                else {
                    pos++;
                    continue;
                }
            }
            else {
                if (ch == '"' || ch == '\'') {
                    inString = true;
                    stringQuote = ch;
                    pos++;
                    continue;
                }
                else if (ch == '{') {
                    braceCount++;
                }
                else if (ch == '}') {
                    braceCount--;
                    if (braceCount == 0) {
                        pos++; 
                        break;
                    }
                }
                pos++;
            }
        }
        if (braceCount != 0) {
            throw std::runtime_error("Unterminated JSON literal at position " + std::to_string(oldPos));
        }
        word = input.substr(oldPos, pos - oldPos);
    }
    else if (std::ispunct(current)) {
        size_t oldPos = pos;
        while (pos < input.size() && std::ispunct(input[pos]) && !std::isspace(input[pos])) {
            pos++;
        }
        word = input.substr(oldPos, pos - oldPos);
        Operator op = determineOperatorType(word);
        if (op != Operator::UNKNOWN) {
            return std::make_unique<Token>(Token{ TokenType::OPERATOR, word, static_cast<int>(oldPos) });
        }
        Symbol sym = determineSymbolType(word);
        if (sym != Symbol::UNKNOWN) {
            return std::make_unique<Token>(Token{ TokenType::SYMBOL, word, static_cast<int>(oldPos) });
        }
        return std::make_unique<Token>(Token{ TokenType::UNKNOWN, word, static_cast<int>(oldPos) });
    }
    else {
        while (pos < input.size() && (std::isalnum(input[pos]) || input[pos] == '_')) {
            pos++;
        }
        word = input.substr(start, pos - start);
    }


    LiteralType litType = determineLiteralType(word);
    if (litType != LiteralType::UNKNOWN) {
        return std::make_unique<Token>(Token{ TokenType::LITERAL, word, static_cast<int>(start), litType });
    }

    TokenType tokenType = determineTokenType(word);
    return std::make_unique<Token>(Token{ tokenType, word, static_cast<int>(start) });
}

std::unique_ptr<Token> Lexer::peekToken() {
    size_t oldPos = pos;
    auto token = nextToken();
    pos = oldPos;
    return token;
}

//==================== Literal format checking functions =====================

bool Lexer::isStringLiteral(const std::string& word) {
    if (word.size() >= 2 && word.front() == '"' && word.back() == '"')
        return true;
    if (word.size() > 3 && word.front() == '\'' && word.back() == '\'')
        return true;
    return false;
}

bool Lexer::isCharLiteral(const std::string& word) {
    return word.size() == 3 && word.front() == '\'' && word.back() == '\'';
}

bool Lexer::isIntegerLiteral(const std::string& word) {
    if (word.empty()) return false;
    size_t startIdx = 0;
    if (word[0] == '-') {
        if (word.size() == 1) return false;
        startIdx = 1;
    }
    for (size_t i = startIdx; i < word.size(); i++) {
        if (!std::isdigit(word[i])) return false;
    }
    return true;
}

bool Lexer::isFloatLiteral(const std::string& word) {
    if (word.empty()) return false;
    size_t startIdx = 0;
    if (word[0] == '-') {
        if (word.size() == 1) return false;
        startIdx = 1;
    }
    bool dotFound = false;
    for (size_t i = startIdx; i < word.size(); i++) {
        if (word[i] == '.') {
            if (dotFound) return false;
            dotFound = true;
        }
        else if (!std::isdigit(word[i])) {
            return false;
        }
    }
    return dotFound; 
}

bool Lexer::isBinaryLiteral(const std::string& word) {
    if (word.size() < 3) return false;
    if (!(word[0] == 'B' || word[0] == 'b')) return false;
    if (word[1] != '\'') return false;
    if (word.back() != '\'') return false;
    for (size_t i = 2; i < word.size() - 1; i++) {
        if (word[i] != '0' && word[i] != '1') return false;
    }
    return true;
}

bool Lexer::isHexLiteral(const std::string& word) {
    if (word.size() < 3) return false;
    if (!(word[0] == 'X' || word[0] == 'x')) return false;
    if (word[1] != '\'') return false;
    if (word.back() != '\'') return false;
    for (size_t i = 2; i < word.size() - 1; i++) {
        if (!std::isxdigit(word[i])) return false;
    }
    return true;
}

bool Lexer::isDateLiteral(const std::string& word) {
    // Expected format 'YYYY-MM-DD' -> 12 characters (10 inside quotes + 2 quotes)
    if (word.size() != 12) return false;
    if (word.front() != '\'' || word.back() != '\'') return false;
    std::string inner = word.substr(1, word.size() - 2);
    if (inner[4] != '-' || inner[7] != '-') return false;
    for (size_t i = 0; i < inner.size(); i++) {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(inner[i])) return false;
    }
    return true;
}

bool Lexer::isTimeLiteral(const std::string& word) {
    // Expected format 'HH:MM:SS' -> 10 characters
    if (word.size() != 10) return false;
    if (word.front() != '\'' || word.back() != '\'') return false;
    std::string inner = word.substr(1, word.size() - 2);
    if (inner[2] != ':' || inner[5] != ':') return false;
    for (size_t i = 0; i < inner.size(); i++) {
        if (i == 2 || i == 5) continue;
        if (!std::isdigit(inner[i])) return false;
    }
    return true;
}

bool Lexer::isDateTimeLiteral(const std::string& word) {
    // Expected format 'YYYY-MM-DD HH:MM:SS' -> 21 characters (19 inside quotes + 2 quotes)
    if (word.size() != 21) return false;
    if (word.front() != '\'' || word.back() != '\'') return false;
    std::string inner = word.substr(1, word.size() - 2);
    if (inner[4] != '-' || inner[7] != '-' || inner[10] != ' ' || inner[13] != ':' || inner[16] != ':') return false;
    for (size_t i = 0; i < inner.size(); i++) {
        if (i == 4 || i == 7 || i == 10 || i == 13 || i == 16) continue;
        if (!std::isdigit(inner[i])) return false;
    }
    return true;
}

bool Lexer::isJSONLiteral(const std::string& word) {
    return word.size() >= 2 && word.front() == '{' && word.back() == '}';
}

bool Lexer::isBooleanLiteral(const std::string& word) {
    std::string upper = word;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    return (upper == "TRUE" || upper == "FALSE");
}

bool Lexer::isNullLiteral(const std::string& word) {
    std::string upper = word;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    return (upper == "NULL");
}
