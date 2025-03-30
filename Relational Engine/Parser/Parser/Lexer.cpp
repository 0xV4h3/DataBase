#include "Lexer.h"

explicit Lexer::Lexer(const std::string& query) : input(query), pos(0) {}

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

TokenType Lexer::determineKeywordType(const std::string& word) {
    Keyword kw = getKeyword(word);
    return (kw != Keyword::UNKNOWN) ? TokenType::KEYWORD : TokenType::IDENTIFIER;
}

TokenType Lexer::determineKeywordType(const size_t& start) {
    size_t temp = start;
    std::string word;
    while (temp < input.size() && (std::isalnum(input[temp]) || input[temp] == '_')) {
        word.push_back(input[temp]);
        temp++;
    }
    return determineKeywordType(word);
}

TokenType Lexer::determineTokenType(const std::string& word) {

}

void Lexer::skipWhitespace() {
    while (pos < input.size() && std::isspace(input[pos])) {
        pos++;
    }
}

std::unique_ptr<Token> Lexer::readKeywordOrIdentifier() {
    size_t start = pos;
    while (pos < input.size() && (std::isalnum(input[pos]) || input[pos] == '_')) {
        pos++;
    }
    std::string value = input.substr(start, pos - start);
    TokenType type = TokenType::IDENTIFIER;
    if (value == "SELECT" || value == "FROM" || value == "WHERE") {
        type = TokenType::KEYWORD;
    }
    return std::make_unique<Token>(Token{ type, value, static_cast<int>(start) });
}

std::unique_ptr<Token> Lexer::nextToken() {
    skipWhitespace();
    if (pos >= input.size()) {
        return std::make_unique<Token>(Token{ TokenType::END_OF_FILE, "", static_cast<int>(pos) });
    }
    char current = input[pos];
    if (std::isalpha(current) || current == '_') {
        return readKeywordOrIdentifier();
    }
    pos++;
    return std::make_unique<Token>(Token{ TokenType::UNKNOWN, std::string(1, current), static_cast<int>(pos - 1) });
}

std::unique_ptr<Token> Lexer::peekToken() {
    size_t oldPos = pos;
    auto token = nextToken();
    pos = oldPos;
    return token;
}

std::unique_ptr<Token> Lexer::readStringLiteral() {
    int start = pos;
    pos++; 
    std::string result;
    while (pos < input.size() && input[pos] != '"') {
        result.push_back(input[pos]);
        pos++;
    }
    if (pos < input.size() && input[pos] == '"') {
        pos++; 
    }
    else {
        throw std::runtime_error("Unterminated string literal at position " + std::to_string(start));
    }
    return std::make_unique<Token>(Token{ TokenType::LITERAL, result, start, LiteralType::STRING });
}

std::unique_ptr<Token> Lexer::readCharLiteral() {
    int start = pos;
    pos++; 
    if (pos >= input.size()) {
        throw std::runtime_error("Unexpected end of input in char literal at position " + std::to_string(start));
    }
    char ch = input[pos++];
    if (pos >= input.size() || input[pos] != '\'') {
        throw std::runtime_error("Unterminated char literal at position " + std::to_string(start));
    }
    pos++; 
    return std::make_unique<Token>(Token{ TokenType::LITERAL, std::string(1, ch), start, LiteralType::CHAR });
}

std::unique_ptr<Token> Lexer::readIntegerLiteral() {
    int start = pos;
    std::string result;
    if (input[pos] == '-') {
        result.push_back(input[pos]);
        pos++;
    }
    while (pos < input.size() && std::isdigit(input[pos])) {
        result.push_back(input[pos]);
        pos++;
    }
    return std::make_unique<Token>(Token{ TokenType::LITERAL, result, start, LiteralType::INTEGER });
}

std::unique_ptr<Token> Lexer::readFloatLiteral() {
    int start = pos;
    std::string result;
    if (input[pos] == '-') {
        result.push_back(input[pos]);
        pos++;
    }
    while (pos < input.size() && std::isdigit(input[pos])) {
        result.push_back(input[pos]);
        pos++;
    }
    if (pos < input.size() && input[pos] == '.') {
        result.push_back(input[pos]);
        pos++;
        while (pos < input.size() && std::isdigit(input[pos])) {
            result.push_back(input[pos]);
            pos++;
        }
    }
    return std::make_unique<Token>(Token{ TokenType::LITERAL, result, start, LiteralType::FLOAT });
}

std::unique_ptr<Token> Lexer::readBinaryLiteral() {
    int start = pos;
    std::string result;
    while (pos < input.size() && (input[pos] == '0' || input[pos] == '1')) {
        result.push_back(input[pos]);
        pos++;
    }
    return std::make_unique<Token>(Token{ TokenType::LITERAL, result, start, LiteralType::BINARY });
}

std::unique_ptr<Token> Lexer::readHexLiteral() {
    int start = pos;
    std::string result;
    if (pos + 1 < input.size() && (input[pos] == 'X' || input[pos] == 'x') && input[pos + 1] == '\'') {
        pos += 2; 
        while (pos < input.size() && input[pos] != '\'') {
            result.push_back(input[pos]);
            pos++;
        }
        if (pos < input.size() && input[pos] == '\'') {
            pos++; 
        }
        else {
            throw std::runtime_error("Unterminated hex literal at position " + std::to_string(start));
        }
    }
    else {
        throw std::runtime_error("Invalid hex literal at position " + std::to_string(start));
    }
    return std::make_unique<Token>(Token{ TokenType::LITERAL, result, start, LiteralType::HEX });
}

std::unique_ptr<Token> Lexer::readDateLiteral() {
    int start = pos;
    pos++; 
    std::string result;
    while (pos < input.size() && input[pos] != '\'') {
        result.push_back(input[pos]);
        pos++;
    }
    if (pos < input.size() && input[pos] == '\'') {
        pos++; 
    }
    else {
        throw std::runtime_error("Unterminated date literal at position " + std::to_string(start));
    }
    // Optional: validate format YYYY-MM-DD here.
    return std::make_unique<Token>(Token{ TokenType::LITERAL, result, start, LiteralType::DATE });
}

std::unique_ptr<Token> Lexer::readTimeLiteral() {
    int start = pos;
    pos++; 
    std::string result;
    while (pos < input.size() && input[pos] != '\'') {
        result.push_back(input[pos]);
        pos++;
    }
    if (pos < input.size() && input[pos] == '\'') {
        pos++; 
    }
    else {
        throw std::runtime_error("Unterminated time literal at position " + std::to_string(start));
    }
    // Optional: validate format HH:MM:SS
    return std::make_unique<Token>(Token{ TokenType::LITERAL, result, start, LiteralType::TIME });
}

std::unique_ptr<Token> Lexer::readDateTimeLiteral() {
    int start = pos;
    pos++; 
    std::string result;
    while (pos < input.size() && input[pos] != '\'') {
        result.push_back(input[pos]);
        pos++;
    }
    if (pos < input.size() && input[pos] == '\'') {
        pos++; 
    }
    else {
        throw std::runtime_error("Unterminated datetime literal at position " + std::to_string(start));
    }
    // Optional: validate format "YYYY-MM-DD HH:MM:SS"
    return std::make_unique<Token>(Token{ TokenType::LITERAL, result, start, LiteralType::DATETIME });
}

std::unique_ptr<Token> Lexer::readJSONLiteral() {
    int start = pos;
    if (input[pos] != '{') {
        throw std::runtime_error("JSON literal must start with '{' at position " + std::to_string(pos));
    }
    int braceCount = 0;
    std::string result;
    while (pos < input.size()) {
        char current = input[pos];
        if (current == '{') {
            braceCount++;
        }
        else if (current == '}') {
            braceCount--;
        }
        result.push_back(current);
        pos++;
        if (braceCount == 0) break;
    }
    if (braceCount != 0) {
        throw std::runtime_error("Unterminated JSON literal starting at position " + std::to_string(start));
    }
    return std::make_unique<Token>(Token{ TokenType::LITERAL, result, start, LiteralType::JSON });
}

std::unique_ptr<Token> Lexer::readBooleanLiteral() {
    int start = pos;
    std::string result;
    while (pos < input.size() && std::isalpha(input[pos])) {
        result.push_back(input[pos]);
        pos++;
    }
    std::string upperResult = result;
    std::transform(upperResult.begin(), upperResult.end(), upperResult.begin(), ::toupper);
    if (upperResult == "TRUE" || upperResult == "FALSE") {
        return std::make_unique<Token>(Token{ TokenType::LITERAL, result, start, LiteralType::BOOLEAN });
    }
    throw std::runtime_error("Invalid boolean literal at position " + std::to_string(start));
}

std::unique_ptr<Token> Lexer::readNullLiteral() {
    int start = pos;
    std::string result;
    while (pos < input.size() && std::isalpha(input[pos])) {
        result.push_back(input[pos]);
        pos++;
    }
    std::string upperResult = result;
    std::transform(upperResult.begin(), upperResult.end(), upperResult.begin(), ::toupper);
    if (upperResult == "NULL") {
        return std::make_unique<Token>(Token{ TokenType::LITERAL, result, start, LiteralType::NULL_VALUE });
    }
    throw std::runtime_error("Invalid NULL literal at position " + std::to_string(start));
}