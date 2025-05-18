#include "Lexer.hpp"

// =======================
// Static Maps for Keywords, Functions, Operators, Symbols, DateTimeParts
// =======================

// --- DML/DDL/Clause/CTE/etc. Keywords ---
const std::unordered_map<std::string, DMLKeyword> Lexer::dmlMap = {
    {"SELECT", DMLKeyword::SELECT}, {"INSERT", DMLKeyword::INSERT},
    {"UPDATE", DMLKeyword::UPDATE}, {"DELETE", DMLKeyword::DELETE},
    {"MERGE", DMLKeyword::MERGE}, {"EXECUTE", DMLKeyword::EXECUTE}
};
const std::unordered_map<std::string, DDLKeyword> Lexer::ddlMap = {
    {"CREATE", DDLKeyword::CREATE}, {"ALTER", DDLKeyword::ALTER},
    {"DROP", DDLKeyword::DROP}, {"TRUNCATE", DDLKeyword::TRUNCATE}
};
const std::unordered_map<std::string, ClauseKeyword> Lexer::clauseMap = {
    {"FROM", ClauseKeyword::FROM}, {"WHERE", ClauseKeyword::WHERE},
    {"JOIN", ClauseKeyword::JOIN}, {"ON", ClauseKeyword::ON},
    {"GROUP", ClauseKeyword::GROUP}, {"BY", ClauseKeyword::BY},
    {"HAVING", ClauseKeyword::HAVING}, {"ORDER", ClauseKeyword::ORDER},
    {"UNION", ClauseKeyword::UNION}, {"DISTINCT", ClauseKeyword::DISTINCT},
    {"TOP", ClauseKeyword::TOP}, {"LIMIT", ClauseKeyword::LIMIT},
    {"OFFSET", ClauseKeyword::OFFSET}, {"LEFT", ClauseKeyword::LEFT},
    {"RIGHT", ClauseKeyword::RIGHT}, {"FULL", ClauseKeyword::FULL},
    {"OUTER", ClauseKeyword::OUTER}, {"INNER", ClauseKeyword::INNER},
    {"CROSS", ClauseKeyword::CROSS}, {"APPLY", ClauseKeyword::APPLY}
};
const std::unordered_map<std::string, CTEKeyword> Lexer::cteMap = {
    {"WITH", CTEKeyword::WITH}
};
const std::unordered_map<std::string, SetOpKeyword> Lexer::setOpMap = {
    {"EXCEPT", SetOpKeyword::EXCEPT}, {"INTERSECT", SetOpKeyword::INTERSECT}
};
const std::unordered_map<std::string, WordOperatorKeyword> Lexer::wordOpMap = {
    {"IN", WordOperatorKeyword::IN}, {"IS", WordOperatorKeyword::IS},
    {"NOT", WordOperatorKeyword::NOT}, {"LIKE", WordOperatorKeyword::LIKE},
    {"BETWEEN", WordOperatorKeyword::BETWEEN},
    {"EXISTS", WordOperatorKeyword::EXISTS},
    {"ALL", WordOperatorKeyword::ALL},
    {"ANY", WordOperatorKeyword::ANY}
};
const std::unordered_map<std::string, LogicalConstantKeyword> Lexer::logicalConstMap = {
    {"NULL", LogicalConstantKeyword::NULL_KEYWORD},
    {"TRUE", LogicalConstantKeyword::TRUE_KEYWORD},
    {"FALSE", LogicalConstantKeyword::FALSE_KEYWORD}
};
const std::unordered_map<std::string, TransactionKeyword> Lexer::tranMap = {
    {"BEGIN", TransactionKeyword::BEGIN}, {"COMMIT", TransactionKeyword::COMMIT},
    {"ROLLBACK", TransactionKeyword::ROLLBACK}, {"SAVEPOINT", TransactionKeyword::SAVEPOINT}
};
const std::unordered_map<std::string, SecurityKeyword> Lexer::secMap = {
    {"GRANT", SecurityKeyword::GRANT}, {"REVOKE", SecurityKeyword::REVOKE},
    {"DENY", SecurityKeyword::DENY}
};
const std::unordered_map<std::string, ProgStmtKeyword> Lexer::progMap = {
    {"DECLARE", ProgStmtKeyword::DECLARE}, {"SET", ProgStmtKeyword::SET},
    {"PRINT", ProgStmtKeyword::PRINT}, {"RETURN", ProgStmtKeyword::RETURN},
    {"THROW", ProgStmtKeyword::THROW}, {"TRY", ProgStmtKeyword::TRY},
    {"CATCH", ProgStmtKeyword::CATCH}
};
const std::unordered_map<std::string, MiscKeyword> Lexer::miscMap = {
    {"AS", MiscKeyword::AS}, {"CASE", MiscKeyword::CASE},
    {"WHEN", MiscKeyword::WHEN}, {"THEN", MiscKeyword::THEN},
    {"ELSE", MiscKeyword::ELSE}, {"END", MiscKeyword::END},
    {"CAST", MiscKeyword::CAST}, {"CONVERT", MiscKeyword::CONVERT},
    {"ASC", MiscKeyword::ASC}, {"DESC", MiscKeyword::DESC}
};

// --- Functions ---
const std::unordered_map<std::string, AggregateFunction> Lexer::aggFuncMap = {
    {"COUNT", AggregateFunction::COUNT}, {"SUM", AggregateFunction::SUM},
    {"AVG", AggregateFunction::AVG}, {"MIN", AggregateFunction::MIN},
    {"MAX", AggregateFunction::MAX}
};
const std::unordered_map<std::string, ScalarFunction> Lexer::scalarFuncMap = {
    {"CAST", ScalarFunction::CAST}, {"CONVERT", ScalarFunction::CONVERT},
    {"COALESCE", ScalarFunction::COALESCE}, {"LENGTH", ScalarFunction::LENGTH}
};
const std::unordered_map<std::string, StringFunction> Lexer::stringFuncMap = {
    {"UPPER", StringFunction::UPPER}, {"LOWER", StringFunction::LOWER},
    {"SUBSTRING", StringFunction::SUBSTRING}, {"TRIM", StringFunction::TRIM}
};
const std::unordered_map<std::string, DateTimeFunction> Lexer::dtFuncMap = {
    {"DATEPART", DateTimeFunction::DATEPART}, {"GETDATE", DateTimeFunction::GETDATE}, {"NOW", DateTimeFunction::NOW}
};
const std::unordered_map<std::string, MathFunction> Lexer::mathFuncMap = {
    {"ABS", MathFunction::ABS}, {"CEILING", MathFunction::CEILING},
    {"FLOOR", MathFunction::FLOOR}, {"ROUND", MathFunction::ROUND}
};
const std::unordered_map<std::string, SystemFunction> Lexer::sysFuncMap = {
    {"SUSER_SNAME", SystemFunction::SUSER_SNAME},
    {"CURRENT_USER", SystemFunction::CURRENT_USER},
    {"TRANCOUNT", SystemFunction::TRANCOUNT}
};

// --- Operators ---
const std::unordered_map<std::string, ArithmeticOp> Lexer::arithMap = {
    {"+", ArithmeticOp::PLUS}, {"-", ArithmeticOp::MINUS},
    {"*", ArithmeticOp::MULTIPLY}, {"/", ArithmeticOp::DIVIDE},
    {"%", ArithmeticOp::MOD}
};
const std::unordered_map<std::string, AssignOp> Lexer::assignMap = {
    {"=", AssignOp::ASSIGN}
};
const std::unordered_map<std::string, ComparisonOp> Lexer::compMap = {
    {"<", ComparisonOp::LESS}, {">", ComparisonOp::GREATER},
    {"<=", ComparisonOp::LESS_EQUAL}, {">=", ComparisonOp::GREATER_EQUAL},
    {"<>", ComparisonOp::NOT_EQUAL}, {"!=", ComparisonOp::NOT_EQUAL},
    {"==", ComparisonOp::EQUAL}
};
const std::unordered_map<std::string, LogicalOp> Lexer::logMap = {
    {"AND", LogicalOp::AND}, {"OR", LogicalOp::OR}, {"NOT", LogicalOp::NOT}
};
const std::unordered_map<std::string, BitwiseOp> Lexer::bitMap = {
    {"&", BitwiseOp::BITWISE_AND}, {"|", BitwiseOp::BITWISE_OR},
    {"^", BitwiseOp::BITWISE_XOR}, {"~", BitwiseOp::BITWISE_NOT},
    {"<<", BitwiseOp::LEFT_SHIFT}, {">>", BitwiseOp::RIGHT_SHIFT}
};
const std::unordered_map<std::string, ConcatOp> Lexer::concatMap = {
    {"||", ConcatOp::CONCAT}
};

// --- Punctuators and Delimiters ---
const std::unordered_map<char, CommonSymbol> Lexer::commonSymMap = {
    {',', CommonSymbol::COMMA}, {';', CommonSymbol::SEMICOLON},
    {'(', CommonSymbol::LPAREN}, {')', CommonSymbol::RPAREN},
    {'{', CommonSymbol::LBRACE}, {'}', CommonSymbol::RBRACE},
    {'[', CommonSymbol::LBRACKET}, {']', CommonSymbol::RBRACKET}
};
const std::unordered_map<std::string, TSQLSymbol> Lexer::tsqlSymMap = {
    {".", TSQLSymbol::DOT}, {":", TSQLSymbol::COLON}, {"::", TSQLSymbol::DOUBLE_COLON}
};
const std::unordered_map<char, StringDelimiter> Lexer::strDelimMap = {
    {'\'', StringDelimiter::SINGLE_QUOTE}, {'"', StringDelimiter::DOUBLE_QUOTE}, {'`', StringDelimiter::BACKTICK}
};

// --- DateTime Parts (for DATEPART, etc.) ---
const std::unordered_map<std::string, DateTimePart> Lexer::dateTimePartsMap = {
    {"YEAR", DateTimePart::YEAR}, {"QUARTER", DateTimePart::QUARTER},
    {"MONTH", DateTimePart::MONTH}, {"DAYOFYEAR", DateTimePart::DAY_OF_YEAR},
    {"DAY", DateTimePart::DAY}, {"WEEK", DateTimePart::WEEK},
    {"ISOWEEK", DateTimePart::ISO_WEEK}, {"WEEKDAY", DateTimePart::WEEKDAY},
    {"HOUR", DateTimePart::HOUR}, {"MINUTE", DateTimePart::MINUTE},
    {"SECOND", DateTimePart::SECOND}, {"MILLISECOND", DateTimePart::MILLISECOND},
    {"MICROSECOND", DateTimePart::MICROSECOND}, {"NANOSECOND", DateTimePart::NANOSECOND},
    {"TIMEZONE_OFFSET", DateTimePart::TIMEZONE_OFFSET}
};

// =======================
// Lexer Implementation
// =======================

/**
 * @brief Constructs a Lexer from the provided SQL query string.
 */
Lexer::Lexer(const std::string& query)
    : input(query), pos(0), tokenIndex(0) {
}

/**
 * @brief Skips whitespace and newlines in the input string, advancing `pos`.
 */
void Lexer::skipWhitespaceAndNewlines() {
    while (pos < input.size() && std::isspace(static_cast<unsigned char>(input[pos]))) {
        ++pos;
    }
}

/**
 * @brief Returns the next token from the input stream.
 *        Handles comments, literals, identifiers, keywords, functions, operators, and punctuators.
 *        Caches tokens for lookahead/backtracking.
 */
std::unique_ptr<Token> Lexer::nextToken() {
    // If we have cached tokens, return from cache
    if (tokenIndex < tokens.size()) {
        return std::make_unique<Token>(*tokens[tokenIndex++]);
    }

    skipWhitespaceAndNewlines();
    if (pos >= input.size()) {
        // End of file/input
        auto tok = std::make_unique<Token>(TokenType::END_OF_FILE, "", static_cast<int>(pos));
        tokens.push_back(std::make_unique<Token>(*tok));
        ++tokenIndex;
        return tok;
    }

    size_t startPos = pos;
    char c = input[pos];
    std::unique_ptr<Token> tok;

    // --- Token type dispatch ---
    if ((pos + 1 < input.size() && input[pos] == '-' && input[pos + 1] == '-') ||
        (pos + 1 < input.size() && input[pos] == '/' && input[pos + 1] == '*')) {
        // Comment
        tok = lexComment();
    }
    else if (c == '[' || c == '"') {
        // Delimited identifier
        tok = lexDelimitedIdentifier();
    }
    else if (c == '\'' ||
        ((c == 'B' || c == 'b' || c == 'X' || c == 'x') &&
            pos + 1 < input.size() && input[pos + 1] == '\'')) {
        // String, binary, or hex literal
        tok = lexString();
    }
    else if (c == '{') {
        // JSON literal
        tok = lexJson();
    }
    else if (tsqlSymMap.find(std::string(1, c)) != tsqlSymMap.end() && c == '.') {
        // Dot notation (e.g. C.CustomerID)
        tok = lexDotOperator();
    }
    else if (std::isdigit(c) || (c == '-' && pos + 1 < input.size() && std::isdigit(input[pos + 1]))) {
        // Numeric literal (including negative)
        tok = lexNumber();
    }
    else if (commonSymMap.find(c) != commonSymMap.end() ||
        tsqlSymMap.find(std::string(1, c)) != tsqlSymMap.end()) {
        // Punctuator (comma, paren, etc.)
        tok = lexPunctuator();
    }
    else if (isIdentifierStart(c)) {
        // Identifier, keyword, or function
        auto funcTok = lexFunction();
        if (funcTok) tok = std::move(funcTok);
        else {
            auto keyTok = lexKeyword();
            if (keyTok) tok = std::move(keyTok);
            else tok = lexIdentifier();
        }
    }
    else {
        // Operator (or unknown)
        auto opTok = lexOperator();
        if (opTok) tok = std::move(opTok);
        else throw std::runtime_error("Unknown character: " + std::string(1, c) + " at position " + std::to_string(pos));
    }

    // Set the position for the token
    if (tok) {
        tok->setPosition(static_cast<int>(startPos));
    }

    tokens.push_back(std::make_unique<Token>(*tok));
    ++tokenIndex;
    return tok;
}

/**
 * @brief Peek at the next token without consuming it.
 * @return Unique pointer to the next token.
 */
std::unique_ptr<Token> Lexer::peekToken() {
    size_t oldIndex = tokenIndex;
    auto tok = nextToken();
    tokenIndex = oldIndex;
    return tok;
}

/**
 * @brief Jump to a specific token index in the token stream.
 * @param offset Index to jump to.
 * @return Unique pointer to the token at new index.
 */
std::unique_ptr<Token> Lexer::jumpToken(int offset) {
    if (offset < 0) throw std::runtime_error("Negative jump offset.");
    while (tokens.size() <= static_cast<size_t>(offset)) {
        nextToken();
    }
    tokenIndex = offset;
    return std::make_unique<Token>(*tokens[tokenIndex]);
}

// ========== Lexing Methods ==========

// --- Comments ---
std::unique_ptr<Token> Lexer::lexComment() {
    size_t start = pos;
    if (pos + 1 < input.size() && input[pos] == '-' && input[pos + 1] == '-') {
        // Single-line comment
        pos += 2;
        while (pos < input.size() && input[pos] != '\n') ++pos;
        return std::make_unique<CommentToken>(CommentType::SINGLE_LINE, input.substr(start, pos - start));
    }
    else if (pos + 1 < input.size() && input[pos] == '/' && input[pos + 1] == '*') {
        // Multi-line comment
        pos += 2;
        while (pos + 1 < input.size() && !(input[pos] == '*' && input[pos + 1] == '/')) ++pos;
        if (pos + 1 >= input.size())
            throw std::runtime_error("Unterminated multi-line comment starting at position " + std::to_string(start));
        pos += 2; // Skip the closing */
        return std::make_unique<CommentToken>(CommentType::MULTI_LINE, input.substr(start, pos - start));
    }
    return nullptr;
}

// --- Dot Notation (C.CustomerID) ---
std::unique_ptr<Token> Lexer::lexDotOperator() {
    char c = input[pos++];
    return std::make_unique<PunctuatorToken>(TSQLSymbol::DOT, ".");
}

// --- Numbers (integer/float, negative, etc.) ---
std::unique_ptr<Token> Lexer::lexNumber() {
    size_t start = pos;
    if (input[pos] == '-') ++pos;
    while (pos < input.size() && std::isdigit(static_cast<unsigned char>(input[pos]))) ++pos;
    if (pos < input.size() && input[pos] == '.') {
        ++pos;
        if (pos == input.size() || !std::isdigit(static_cast<unsigned char>(input[pos])))
            throw std::runtime_error("Invalid float literal at position " + std::to_string(start));
        while (pos < input.size() && std::isdigit(static_cast<unsigned char>(input[pos]))) ++pos;
    }
    std::string word = input.substr(start, pos - start);
    return createLiteralToken(word);
}

// --- Keywords (not functions) ---
std::unique_ptr<Token> Lexer::lexKeyword() {
    size_t start = pos;
    while (pos < input.size() && isIdentifierPart(input[pos])) ++pos;
    std::string word = input.substr(start, pos - start);

    std::string upper = word;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

    // Handle boolean and null as literal tokens
    if (upper == "TRUE" || upper == "FALSE") {
        return std::make_unique<LiteralToken>(LiteralCategory::BOOLEAN, word);
    }
    if (upper == "NULL") {
        return std::make_unique<LiteralToken>(LiteralCategory::NULL_VALUE, word);
    }

    // Recognize date/time parts globally (e.g. YEAR, MONTH, etc.)
    auto itDateTime = dateTimePartsMap.find(upper);
    if (itDateTime != dateTimePartsMap.end()) {
        return std::make_unique<DateTimePartToken>(itDateTime->second, word);
    }

    // Recognize logical operators (AND, OR, NOT)
    auto itLog = logMap.find(upper);
    if (itLog != logMap.end()) {
        return std::make_unique<OperatorToken>(itLog->second, word);
    }

    // Standard SQL keywords
    auto keyTok = createKeywordToken(upper);
    if (keyTok) {
        return keyTok;
    }

    // Not a keyword, logical op, or literal; rewind and let identifier lexer handle
    pos = start;
    return nullptr;
}

// --- Functions (must be followed by '(' and match a function name) ---
std::unique_ptr<Token> Lexer::lexFunction() {
    size_t start = pos;
    while (pos < input.size() && isIdentifierPart(input[pos])) ++pos;
    std::string word = input.substr(start, pos - start);
    std::string upper = word;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

    // Lookahead for '(' (skip whitespace)
    size_t temp = pos;
    skipWhitespaceAndNewlines();
    bool isFunc = (pos < input.size() && input[pos] == '(');
    pos = temp; // Restore position after lookahead

    if (!isFunc) {
        pos = start; // Not a function, rewind to start
        return nullptr;
    }

    // It's followed by '(', let's check if it's a known function
    auto funcTok = createFunctionToken(upper);
    if (funcTok) {
        return funcTok;
    }

    // Not a recognized function, rewind
    pos = start;
    return nullptr;
}

// --- Identifiers ---
std::unique_ptr<Token> Lexer::lexIdentifier() {
    size_t start = pos;
    IdentifierCategory category = IdentifierCategory::UNKNOWN;

    // User/system variable
    if (input[pos] == '@') {
        size_t prefixLen = 1;
        if (pos + 1 < input.size() && input[pos + 1] == '@') {
            prefixLen = 2;
            category = IdentifierCategory::SYSTEM_VARIABLE;
        }
        else {
            category = IdentifierCategory::USER_VARIABLE;
        }
        pos += prefixLen;
        while (pos < input.size() && isIdentifierPart(input[pos]))
            ++pos;
        std::string word = input.substr(start, pos - start);
        return std::make_unique<IdentifierToken>(category, word);
    }
    // Temp/global temp table
    else if (input[pos] == '#') {
        size_t prefixLen = 1;
        if (pos + 1 < input.size() && input[pos + 1] == '#') {
            prefixLen = 2;
            category = IdentifierCategory::GLOBAL_TEMP_TABLE;
        }
        else {
            category = IdentifierCategory::TEMP_TABLE;
        }
        pos += prefixLen;
        while (pos < input.size() && isIdentifierPart(input[pos]))
            ++pos;
        std::string word = input.substr(start, pos - start);
        return std::make_unique<IdentifierToken>(category, word);
    }
    else {
        // Regular identifier
        while (pos < input.size() && isIdentifierPart(input[pos]))
            ++pos;
        std::string word = input.substr(start, pos - start);
        return std::make_unique<IdentifierToken>(IdentifierCategory::UNKNOWN, word);
    }
}

// --- Delimited Identifier: [foo] or "foo" ---
std::unique_ptr<Token> Lexer::lexDelimitedIdentifier() {
    char open = input[pos];
    char close = (open == '[') ? ']' : '"';
    ++pos;
    size_t start = pos;

    while (pos < input.size() && input[pos] != close) ++pos;

    if (pos >= input.size()) {
        throw std::runtime_error("Unterminated delimited identifier starting at position " +
            std::to_string(start - 1));
    }

    std::string word = input.substr(start, pos - start);
    ++pos;
    return std::make_unique<IdentifierToken>(IdentifierCategory::UNKNOWN, word);
}

// --- String, Binary, Hex, Date, Time, Datetime Literals ---
std::unique_ptr<Token> Lexer::lexString() {
    size_t start = pos;
    char first = input[pos];

    // Binary or Hex literal (B'...' or X'...')
    if ((first == 'B' || first == 'b' || first == 'X' || first == 'x') && pos + 1 < input.size() && input[pos + 1] == '\'') {
        pos += 2;
        size_t literalStart = pos;
        while (pos < input.size() && input[pos] != '\'') ++pos;
        if (pos >= input.size())
            throw std::runtime_error("Unterminated binary/hex literal at position " + std::to_string(start));
        ++pos; // skip closing quote
        std::string word = input.substr(start, pos - start);
        return createLiteralToken(word);
    }

    // String/Char/Date/Time/Datetime (single or double quote)
    if (first == '\'' || first == '"') {
        char quote = first;
        ++pos; // skip opening quote
        size_t literalStart = pos;
        while (pos < input.size() && input[pos] != quote) {
            // Handle escape sequences (optional)
            if (input[pos] == '\\' && pos + 1 < input.size()) {
                pos += 2;
            }
            else {
                ++pos;
            }
        }
        if (pos >= input.size())
            throw std::runtime_error("Unterminated string/date/time literal at position " + std::to_string(start));
        ++pos; // skip closing quote
        std::string word = input.substr(start, pos - start);
        return createLiteralToken(word);
    }

    throw std::runtime_error("Unrecognized literal at position " + std::to_string(start));
}

// --- JSON Literal ---
std::unique_ptr<Token> Lexer::lexJson() {
    size_t start = pos;
    int braceCount = 0;
    size_t jsonStart = pos;
    do {
        if (pos >= input.size()) break;
        if (input[pos] == '{') ++braceCount;
        if (input[pos] == '}') --braceCount;
        ++pos;
    } while (braceCount > 0);

    if (braceCount != 0)
        throw std::runtime_error("Unterminated JSON literal at position " + std::to_string(start));
    std::string word = input.substr(jsonStart, pos - jsonStart);
    return createLiteralToken(word);
}

// --- Operators ---
std::unique_ptr<Token> Lexer::lexOperator() {
    size_t maxOpLen = 3; // For operators like '<=>', '!=', etc.
    for (size_t len = maxOpLen; len > 0; --len) {
        if (pos + len <= input.size()) {
            std::string op = input.substr(pos, len);
            // Only accept if all chars are valid operator symbols (no whitespace or letters)
            if (std::all_of(op.begin(), op.end(), [&](char ch) { return isOperatorChar(ch); })) {
                auto opTok = createOperatorToken(op);
                if (opTok->getType() == TokenType::OPERATOR) {
                    pos += len;
                    return opTok;
                }
            }
        }
    }
    // Single char fallback
    if (pos < input.size() && isOperatorChar(input[pos])) {
        std::string op(1, input[pos]);
        auto opTok = createOperatorToken(op);
        if (opTok->getType() == TokenType::OPERATOR) {
            pos++;
            return opTok;
        }
    }
    return nullptr;
}

// --- Punctuator (e.g., comma, parens, etc.) ---
std::unique_ptr<Token> Lexer::lexPunctuator() {
    char c = input[pos];

    // Check for 2-char TSQL symbols (e.g. ::)
    if (pos + 1 < input.size()) {
        std::string twoChar = input.substr(pos, 2);
        auto it = tsqlSymMap.find(twoChar);
        if (it != tsqlSymMap.end()) {
            pos += 2;
            return std::make_unique<PunctuatorToken>(it->second, twoChar);
        }
    }

    // Check for 1-char TSQL symbols (e.g. .)
    std::string oneChar(1, c);
    auto itTsql = tsqlSymMap.find(oneChar);
    if (itTsql != tsqlSymMap.end()) {
        pos += 1;
        return std::make_unique<PunctuatorToken>(itTsql->second, oneChar);
    }

    // Check for 1-char punctuators from common symbols (not dot!)
    auto itCommon = commonSymMap.find(c);
    if (itCommon != commonSymMap.end()) {
        ++pos;
        return std::make_unique<PunctuatorToken>(itCommon->second, std::string(1, c));
    }

    return nullptr;
}

// ========== Literal Recognition Utilities ==========

// Checks if string is a (double-quoted or single-quoted) string literal (not char)
bool Lexer::isStringLiteral(const std::string& word) {
    return (
        (word.size() >= 2 && word.front() == '"' && word.back() == '"') ||
        (word.size() > 3 && word.front() == '\'' && word.back() == '\'')
        );
}

// Checks if string is a single character in single quotes (e.g. 'A')
bool Lexer::isCharLiteral(const std::string& word) {
    return (word.size() == 3 && word.front() == '\'' && word.back() == '\'');
}

// Checks if string is an integer (optionally negative)
bool Lexer::isIntegerLiteral(const std::string& word) {
    if (word.empty()) return false;
    size_t startIdx = (word[0] == '-') ? 1 : 0;
    if (startIdx == word.size()) return false;
    for (size_t i = startIdx; i < word.size(); ++i)
        if (!std::isdigit(static_cast<unsigned char>(word[i]))) return false;
    return true;
}

// Checks if string is a float (optionally negative, must contain one '.')
bool Lexer::isFloatLiteral(const std::string& word) {
    if (word.empty()) return false;
    size_t startIdx = (word[0] == '-') ? 1 : 0;
    if (startIdx == word.size()) return false;
    bool dotFound = false;
    for (size_t i = startIdx; i < word.size(); ++i) {
        if (word[i] == '.') {
            if (dotFound) return false;
            dotFound = true;
        }
        else if (!std::isdigit(static_cast<unsigned char>(word[i]))) {
            return false;
        }
    }
    return dotFound;
}

// Checks if string is a binary literal: B'0101'
bool Lexer::isBinaryLiteral(const std::string& word) {
    if (word.size() < 4) return false;
    if ((word[0] != 'B' && word[0] != 'b') || word[1] != '\'' || word.back() != '\'') return false;
    for (size_t i = 2; i < word.size() - 1; ++i)
        if (word[i] != '0' && word[i] != '1') return false;
    return true;
}

// Checks if string is a hex literal: X'1A2B'
bool Lexer::isHexLiteral(const std::string& word) {
    if (word.size() < 4) return false;
    if ((word[0] != 'X' && word[0] != 'x') || word[1] != '\'' || word.back() != '\'') return false;
    for (size_t i = 2; i < word.size() - 1; ++i)
        if (!std::isxdigit(static_cast<unsigned char>(word[i]))) return false;
    return true;
}

// Checks if string is a date literal: 'YYYY-MM-DD'
bool Lexer::isDateLiteral(const std::string& word) {
    if (word.size() != 12) return false;
    if (word.front() != '\'' || word.back() != '\'') return false;
    const std::string inner = word.substr(1, 10);
    for (size_t i = 0; i < inner.size(); ++i) {
        if (i == 4 || i == 7) {
            if (inner[i] != '-') return false;
        }
        else if (!std::isdigit(static_cast<unsigned char>(inner[i]))) return false;
    }
    return true;
}

// Checks if string is a time literal: 'HH:MM:SS'
bool Lexer::isTimeLiteral(const std::string& word) {
    if (word.size() != 10) return false;
    if (word.front() != '\'' || word.back() != '\'') return false;
    const std::string inner = word.substr(1, 8);
    for (size_t i = 0; i < inner.size(); ++i) {
        if (i == 2 || i == 5) {
            if (inner[i] != ':') return false;
        }
        else if (!std::isdigit(static_cast<unsigned char>(inner[i]))) return false;
    }
    return true;
}

// Checks if string is a datetime literal: 'YYYY-MM-DD HH:MM:SS'
bool Lexer::isDateTimeLiteral(const std::string& word) {
    if (word.size() != 21) return false;
    if (word.front() != '\'' || word.back() != '\'') return false;
    const std::string inner = word.substr(1, 19);
    return (inner.size() == 19 &&
        std::all_of(inner.begin(), inner.end(), [](char c) {
            return std::isdigit(static_cast<unsigned char>(c)) || c == '-' || c == ' ' || c == ':';
            }) &&
        inner[4] == '-' && inner[7] == '-' && inner[10] == ' ' && inner[13] == ':' && inner[16] == ':');
}

// Checks if string is a JSON literal: {...}
bool Lexer::isJSONLiteral(const std::string& word) {
    return (word.size() >= 2 && word.front() == '{' && word.back() == '}');
}

// Checks if string is boolean: TRUE or FALSE (case-insensitive)
bool Lexer::isBooleanLiteral(const std::string& word) {
    if (word.empty()) return false;
    std::string up = word;
    std::transform(up.begin(), up.end(), up.begin(), ::toupper);
    return (up == "TRUE" || up == "FALSE");
}

// Checks if string is NULL literal (case-insensitive)
bool Lexer::isNullLiteral(const std::string& word) {
    if (word.empty()) return false;
    std::string up = word;
    std::transform(up.begin(), up.end(), up.begin(), ::toupper);
    return (up == "NULL");
}

/**
 * @brief Returns true if c is a valid operator symbol.
 */
bool Lexer::isOperatorChar(char c) {
    return std::string("=!<>+-*/%^&|~").find(c) != std::string::npos;
}

// =======================
// Token Creation Methods
// =======================

/**
 * @brief Creates a KeywordToken from a string, or nullptr if not a keyword.
 */
std::unique_ptr<Token> Lexer::createKeywordToken(const std::string& word) {
    std::string upper = word;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    if (auto it = dmlMap.find(upper); it != dmlMap.end()) return std::make_unique<KeywordToken>(it->second, word);
    if (auto it = ddlMap.find(upper); it != ddlMap.end()) return std::make_unique<KeywordToken>(it->second, word);
    if (auto it = clauseMap.find(upper); it != clauseMap.end()) return std::make_unique<KeywordToken>(it->second, word);
    if (auto it = cteMap.find(upper); it != cteMap.end()) return std::make_unique<KeywordToken>(it->second, word);
    if (auto it = setOpMap.find(upper); it != setOpMap.end()) return std::make_unique<KeywordToken>(it->second, word);
    if (auto it = wordOpMap.find(upper); it != wordOpMap.end()) return std::make_unique<KeywordToken>(it->second, word);
    if (auto it = logicalConstMap.find(upper); it != logicalConstMap.end()) return std::make_unique<KeywordToken>(it->second, word);
    if (auto it = tranMap.find(upper); it != tranMap.end()) return std::make_unique<KeywordToken>(it->second, word);
    if (auto it = secMap.find(upper); it != secMap.end()) return std::make_unique<KeywordToken>(it->second, word);
    if (auto it = progMap.find(upper); it != progMap.end()) return std::make_unique<KeywordToken>(it->second, word);
    if (auto it = miscMap.find(upper); it != miscMap.end()) return std::make_unique<KeywordToken>(it->second, word);
    return nullptr;
}

/**
 * @brief Creates a FunctionToken from a string, or nullptr if not a function.
 */
std::unique_ptr<Token> Lexer::createFunctionToken(const std::string& word) {
    std::string upper = word;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

    if (auto it = aggFuncMap.find(upper); it != aggFuncMap.end())
        return std::make_unique<FunctionToken>(it->second, word);
    if (auto it = scalarFuncMap.find(upper); it != scalarFuncMap.end())
        return std::make_unique<FunctionToken>(it->second, word);
    if (auto it = stringFuncMap.find(upper); it != stringFuncMap.end())
        return std::make_unique<FunctionToken>(it->second, word);
    if (auto it = dtFuncMap.find(upper); it != dtFuncMap.end())
        return std::make_unique<FunctionToken>(it->second, word);
    if (auto it = mathFuncMap.find(upper); it != mathFuncMap.end())
        return std::make_unique<FunctionToken>(it->second, word);
    if (auto it = sysFuncMap.find(upper); it != sysFuncMap.end())
        return std::make_unique<FunctionToken>(it->second, word);

    return nullptr;
}

/**
 * @brief Creates a LiteralToken of the appropriate type from a string.
 */
std::unique_ptr<Token> Lexer::createLiteralToken(const std::string& word) {
    if (isBinaryLiteral(word))    return std::make_unique<LiteralToken>(LiteralCategory::BINARY, word);
    if (isHexLiteral(word))       return std::make_unique<LiteralToken>(LiteralCategory::HEX, word);
    if (isDateTimeLiteral(word))  return std::make_unique<LiteralToken>(LiteralCategory::DATETIME, word);
    if (isDateLiteral(word))      return std::make_unique<LiteralToken>(LiteralCategory::DATE, word);
    if (isTimeLiteral(word))      return std::make_unique<LiteralToken>(LiteralCategory::TIME, word);
    if (isJSONLiteral(word))      return std::make_unique<LiteralToken>(LiteralCategory::JSON, word);
    if (isBooleanLiteral(word))   return std::make_unique<LiteralToken>(LiteralCategory::BOOLEAN, word);
    if (isNullLiteral(word))      return std::make_unique<LiteralToken>(LiteralCategory::NULL_VALUE, word);
    if (isFloatLiteral(word))     return std::make_unique<LiteralToken>(LiteralCategory::FLOAT, word);
    if (isIntegerLiteral(word))   return std::make_unique<LiteralToken>(LiteralCategory::INTEGER, word);
    if (isCharLiteral(word))      return std::make_unique<LiteralToken>(LiteralCategory::CHAR, word);
    if (isStringLiteral(word))    return std::make_unique<LiteralToken>(LiteralCategory::STRING, word);
    return std::make_unique<LiteralToken>(LiteralCategory::UNKNOWN, word);
}

/**
 * @brief Creates an OperatorToken from a string, or UNKNOWN if not a known operator.
 */
std::unique_ptr<Token> Lexer::createOperatorToken(const std::string& word) {
    // 1. Comparison operators
    auto itComp = compMap.find(word);
    if (itComp != compMap.end())
        return std::make_unique<OperatorToken>(itComp->second, word);

    // 2. Assignment operators
    auto itAssign = assignMap.find(word);
    if (itAssign != assignMap.end())
        return std::make_unique<OperatorToken>(itAssign->second, word);

    // 3. Logical operators (AND, OR, NOT)
    auto itLog = logMap.find(word);
    if (itLog != logMap.end())
        return std::make_unique<OperatorToken>(itLog->second, word);

    // 4. Bitwise operators
    auto itBit = bitMap.find(word);
    if (itBit != bitMap.end())
        return std::make_unique<OperatorToken>(itBit->second, word);

    // 5. Concat operators
    auto itConcat = concatMap.find(word);
    if (itConcat != concatMap.end())
        return std::make_unique<OperatorToken>(itConcat->second, word);

    // 6. Arithmetic operators
    auto itArith = arithMap.find(word);
    if (itArith != arithMap.end())
        return std::make_unique<OperatorToken>(itArith->second, word);

    // Fallback: unknown operator
    return std::make_unique<OperatorToken>(ArithmeticOp::UNKNOWN, word);
}

/**
 * @brief Creates a PunctuatorToken from a string, or UNKNOWN if not a known punctuator.
 */
std::unique_ptr<Token> Lexer::createPunctuatorToken(const std::string& word) {
    if (word.empty()) return std::make_unique<PunctuatorToken>(CommonSymbol::UNKNOWN, word);

    // 2-char TSQL symbols (e.g., '::')
    auto itTsql = tsqlSymMap.find(word);
    if (itTsql != tsqlSymMap.end())
        return std::make_unique<PunctuatorToken>(itTsql->second, word);

    // 1-char common symbols
    if (word.size() == 1) {
        char c = word[0];
        auto itCommon = commonSymMap.find(c);
        if (itCommon != commonSymMap.end())
            return std::make_unique<PunctuatorToken>(itCommon->second, word);

        auto itDelim = strDelimMap.find(c);
        if (itDelim != strDelimMap.end())
            return std::make_unique<PunctuatorToken>(itDelim->second, word);
    }

    return std::make_unique<PunctuatorToken>(CommonSymbol::UNKNOWN, word);
}