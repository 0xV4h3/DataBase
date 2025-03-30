#include "Lexer.h"

Lexer::Lexer(const std::string& query) : input(query), pos(0) {}

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

TokenType Lexer::determineTokenType(const std::string& word) {
    Keyword kw = getKeyword(word);
    return (kw != Keyword::UNKNOWN) ? TokenType::KEYWORD : TokenType::IDENTIFIER;
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
    if (it != keywordMap.end()) {
        return it->second;
    }
    return Keyword::UNKNOWN;
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
    if (isStringLiteral(word)) {
        return LiteralType::STRING;
    }
    else if (isCharLiteral(word)) {
        return LiteralType::CHAR;
    }
    else if (isIntegerLiteral(word)) {
        return LiteralType::INTEGER;
    }
    else if (isFloatLiteral(word)) {
        return LiteralType::FLOAT;
    }
    else if (isBinaryLiteral(word)) {
        return LiteralType::BINARY;
    }
    else if (isHexLiteral(word)) {
        return LiteralType::HEX;
    }
    else if (isDateLiteral(word)) {
        return LiteralType::DATE;
    }
    else if (isTimeLiteral(word)) {
        return LiteralType::TIME;
    }
    else if (isDateTimeLiteral(word)) {
        return LiteralType::DATETIME;
    }
    else if (isJSONLiteral(word)) {
        return LiteralType::JSON;
    }
    else if (isBooleanLiteral(word)) {
        return LiteralType::BOOLEAN;
    }
    else if (isNullLiteral(word)) {
        return LiteralType::NULL_VALUE;
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
    if (word == "+")  return Operator::PLUS;
    if (word == "-")  return Operator::MINUS;
    if (word == "*")  return Operator::MULTIPLY;
    if (word == "/")  return Operator::DIVIDE;
    if (word == "%")  return Operator::MOD;
    if (word == "=")  return Operator::ASSIGN;
    if (word == "<")  return Operator::LESS;
    if (word == ">")  return Operator::GREATER;
    if (word == "<=") return Operator::LESS_EQUAL;
    if (word == ">=") return Operator::GREATER_EQUAL;
    if (word == "<>") return Operator::NOT_EQUAL;
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
    if (word == ",") return Symbol::COMMA;
    if (word == ";") return Symbol::SEMICOLON;
    if (word == "(") return Symbol::LPAREN;
    if (word == ")") return Symbol::RPAREN;
    if (word == "{") return Symbol::LBRACE;
    if (word == "}") return Symbol::RBRACE;
    if (word == "[") return Symbol::LBRACKET;
    if (word == "]") return Symbol::RBRACKET;
    return Symbol::UNKNOWN;
}

void Lexer::skipWhitespace() {
    while (pos < input.size() && std::isspace(input[pos])) {
        pos++;
    }
}

std::unique_ptr<Token> Lexer::nextToken() {
    skipWhitespace();

    if (pos >= input.size()) {
        return std::make_unique<Token>(Token{ TokenType::END_OF_FILE, "", static_cast<int>(pos) });
    }

    size_t start = pos;
    char current = input[pos];
    std::string word;

    if (current == '"' || current == '\'') {
        char quoteChar = current;
        pos++; 
        while (pos < input.size() && input[pos] != quoteChar) {
            pos++;
        }
        if (pos >= input.size() || input[pos] != quoteChar) {
            throw std::runtime_error("Unterminated literal at position " + std::to_string(start));
        }
        pos++; 
        word = input.substr(start, pos - start);
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
        while (pos < input.size() && !std::isspace(input[pos]) && !std::ispunct(input[pos])) {
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

bool Lexer::isStringLiteral(const std::string& word) {
    return word.size() >= 2 && word.front() == '"' && word.back() == '"';
}

bool Lexer::isCharLiteral(const std::string& word) {
    return word.size() >= 3 && word.front() == '\'' && word.back() == '\'';
}

bool Lexer::isIntegerLiteral(const std::string& word) {
    if (word.empty()) return false;
    size_t start = 0;
    if (word[0] == '-') {
        if (word.size() == 1) return false;
        start = 1;
    }
    for (size_t i = start; i < word.size(); i++) {
        if (!std::isdigit(word[i])) return false;
    }
    return true;
}

bool Lexer::isFloatLiteral(const std::string& word) {
    if (word.empty()) return false;
    size_t start = 0;
    if (word[0] == '-') {
        if (word.size() == 1) return false;
        start = 1;
    }
    bool dotFound = false;
    for (size_t i = start; i < word.size(); i++) {
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