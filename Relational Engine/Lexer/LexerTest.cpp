#include "Lexer.hpp"
#include "LexerUtils.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>

// Test categories
void runBasicTests(std::string username);
void runAdvancedTests(std::string username);
void runEdgeCaseTests(std::string username);
void runErrorHandlingTests(std::string username);
void runPerformanceTests(std::string username);
void runInteractiveTests(std::string username);

// Test utilities
void printTestHeader(const std::string& name);
void printTokenDetails(const Token& token);
void printLexerDiagnostics(const Lexer& lexer);
void measurePerformance(const std::string& query, int iterations);

/**
 * @brief Get a valid choice from user input within specified range
 * @param min_choice Minimum valid choice value
 * @param max_choice Maximum valid choice value
 * @return Valid user choice
 */
int get_valid_choice(int min_choice, int max_choice) {
    int choice;
    while (true) {
        std::cout << "Enter your choice: ";
        std::string input;
        std::getline(std::cin, input);

        // Check if input is an integer
        try {
            size_t idx;
            choice = std::stoi(input, &idx);
            if (idx != input.size()) throw std::invalid_argument("Extra chars");
        }
        catch (...) {
            std::cout << "Invalid input. Please enter a number between "
                << min_choice << " and " << max_choice << ".\n";
            continue;
        }

        // Validate range
        if (choice < min_choice || choice > max_choice) {
            std::cout << "Choice out of range. Please enter a number between "
                << min_choice << " and " << max_choice << ".\n";
            continue;
        }
        return choice;
    }
}

int main() {
    std::cout << "Please enter your username: ";
    std::string username;
    std::getline(std::cin, username);
    if (username.empty()) {
        username = "anonymous";
    }

    constexpr int min_choice = 0, max_choice = 6;  
    while (true) {
        std::cout << "\n=== SQL Lexer Test Suite ===\n"
            << "1. Basic Tests\n"
            << "2. Advanced Tests\n"
            << "3. Edge Case Tests\n"
            << "4. Error Handling Tests\n"
            << "5. Performance Tests\n"
            << "6. Interactive Mode\n"
            << "0. Exit\n"
            << "Choose an option: ";

        int choice = get_valid_choice(min_choice, max_choice);

        switch (choice) {
        case 0: return 0;
        case 1: runBasicTests(username); break;
        case 2: runAdvancedTests(username); break;
        case 3: runEdgeCaseTests(username); break;
        case 4: runErrorHandlingTests(username); break;
        case 5: runPerformanceTests(username); break;
        case 6: runInteractiveTests(username); break;
        default: std::cout << "Invalid choice\n";
        }
    }
}

void printTestHeader(const std::string& name) {
    std::cout << "\n" << std::string(50, '=') << "\n"
        << name << "\n"
        << std::string(50, '=') << std::endl;
}

/**
 * @brief Get current timestamp in YYYY-MM-DD HH:MM:SS format
 */
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm timeStruct;
    gmtime_s(&timeStruct, &time); 
    std::stringstream ss;
    ss << std::put_time(&timeStruct, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

/**
 * @brief Prints detailed information about a token
 * @param token The token to print
 */
void printTokenDetails(const Token& token) {
    std::cout << "Token: \"" << token.getValue() << "\""
        << " | Type: " << LexerUtils::tokenTypeToString(token.getType());

    switch (token.getType()) {
    case TokenType::KEYWORD: {
        auto kw = dynamic_cast<const KeywordToken*>(&token);
        if (kw && kw->infoPtr) {
            std::cout << " (Category: " << LexerUtils::keywordCategoryToString(kw->getCategory()) << ")";
        }
        break;
    }

    case TokenType::FUNCTION: {
        auto func = dynamic_cast<const FunctionToken*>(&token);
        if (func && func->infoPtr) {
            std::cout << " (Category: " << LexerUtils::FunctionCategoryTypeToString(func->getCategory()) << ")";
        }
        break;
    }

    case TokenType::IDENTIFIER: {
        auto id = dynamic_cast<const IdentifierToken*>(&token);
        if (id && id->infoPtr) {
            std::cout << " (Category: " << LexerUtils::identifierCategoryToString(id->getCategory());
            if (id->isQualified()) {
                std::cout << ", Schema: \"" << id->getSchema() << "\"";
            }
            std::cout << ")";
        }
        break;
    }

    case TokenType::LITERAL: {
        auto lit = dynamic_cast<const LiteralToken*>(&token);
        if (lit) {
            std::cout << " (Category: " << LexerUtils::literalCategoryToString(lit->litType);
            if (lit->hasValue()) {
                std::cout << ", Value: " << lit->valuePtr->toString();
            }
            std::cout << ")";
        }
        break;
    }

    case TokenType::LITERAL_CATEGORY: {
        auto litcat = dynamic_cast<const LiteralCategoryToken*>(&token);
        if (litcat) {
            std::cout << " (" << LexerUtils::literalCategoryToString(litcat->getCategory());
        }
        break;
    }

    case TokenType::OPERATOR: {
        auto op = dynamic_cast<const OperatorToken*>(&token);
        if (op && op->infoPtr) {
            std::cout << " (Category: " << LexerUtils::operatorCategoryToString(op->getCategory()) << ")";
        }
        break;
    }

    case TokenType::PUNCTUATOR: {
        auto punc = dynamic_cast<const PunctuatorToken*>(&token);
        if (punc && punc->infoPtr) {
            std::cout << " (";
            if (punc->isCommonSymbol()) {
                std::cout << "Common: " << LexerUtils::CommonSymbolTypeToString(punc->getCommonSymbol());
            }
            if (punc->isTSQLSymbol()) {
                if (punc->isCommonSymbol()) std::cout << ", ";
                std::cout << "TSQL: " << LexerUtils::TSQLSymbolTypeToString(punc->getTSQLSymbol());
            }
            if (punc->isStringDelimiter()) {
                if (punc->isCommonSymbol() || punc->isTSQLSymbol()) std::cout << ", ";
                std::cout << "Delim: " << LexerUtils::StringDelimiterTypeToString(punc->getDelimiterType());
            }
            std::cout << ")";
        }
        break;
    }

    case TokenType::DATETIMEPART: {
        auto dtpart = dynamic_cast<const DateTimePartToken*>(&token);
        if (dtpart) {
            std::cout << " (" << LexerUtils::DateTimePartTypeToString(dtpart->getPart())
                << (dtpart->isDatePart() ? ", Date" :
                    dtpart->isTimePart() ? ", Time" : ", Zone")
                << ")";
        }
        break;
    }

    case TokenType::COMMENT: {
        auto comment = dynamic_cast<const CommentToken*>(&token);
        if (comment) {
            std::cout << " (Type: " << LexerUtils::commentTypeToString(comment->commentType)
                << ", Lines: " << comment->getLineCount()
                << ", Length: " << comment->getText().length() << ")";
        }
        break;
    }

    case TokenType::END_OF_FILE:
        std::cout << " (EOF)";
        break;
    }

    std::cout << " | Position: " << token.getPosition();

    // Add line and column information if available
    if (token.hasValidPosition()) {
        std::cout << " (Line: " << token.getPosition() / 80 + 1  // Approximate line number
            << ", Col: " << token.getPosition() % 80 + 1     // Approximate column
            << ")";
    }

    std::cout << std::endl;
}

/**
 * @brief Prints detailed diagnostic information about the lexer's current state
 * @param lexer Reference to the lexer instance
 */
void printLexerDiagnostics(const Lexer& lexer) {
    // Get lexer diagnostics string
    std::string diagnostics = lexer.getDiagnostics();
    std::cout << diagnostics;

    // Get error information
    const auto& errorLog = lexer.getErrorLog();
    if (!errorLog.empty()) {
        std::cout << "\nDetailed Error Log:\n";
        for (const auto& error : errorLog) {
            std::cout << "Error at position " << error.position
                << " (Line " << error.line << ", Col " << error.column << "):\n"
                << "Message: " << error.message << "\n"
                << "Context: " << error.context << "\n"
                << "         " << error.pointer << "\n"
                << "Severity: " << static_cast<int>(error.severity) << "\n"
                << "Timestamp: " << error.timestamp << "\n"
                << std::string(50, '-') << "\n";
        }
    }

    // Position information
    std::cout << "\nCurrent Position Info:\n"
        << "- Position: " << lexer.getCurrentPosition() << "\n"
        << "- Line: " << lexer.getCurrentLine() << "\n"
        << "- Column: " << lexer.getCurrentColumn() << "\n"
        << "- Token Index: " << lexer.getCurrentTokenIndex() << "\n"
        << "- Total Tokens: " << lexer.getTokenCount() << "\n";

    // Context information
    std::cout << "\nContext:\n"
        << "- Error Context: " << lexer.getErrorContext() << "\n"
        << "- Has Errors: " << (lexer.hasErrors() ? "Yes" : "No") << "\n";

    std::cout << std::string(50, '=') << std::endl;
}

/**
 * @brief Measures the performance of lexical analysis for a given query
 * @param query The SQL query to analyze
 * @param iterations Number of times to run the analysis
 */
void measurePerformance(const std::string& query, int iterations) {
    std::cout << "\n=== Performance Measurement ===\n"
        << "Query:\n" << query << "\n"
        << "Iterations: " << iterations << "\n\n";

    // Warm-up run
    {
        Lexer lexer(query);
        while (lexer.nextToken()->getType() != TokenType::END_OF_FILE) {}
    }

    // Performance metrics
    size_t totalTokens = 0;
    double totalTime = 0.0;
    double minTime = std::numeric_limits<double>::max();
    double maxTime = 0.0;
    std::vector<double> times;
    times.reserve(iterations);

    // Run measurements
    for (int i = 0; i < iterations; ++i) {
        auto start = std::chrono::high_resolution_clock::now();

        // Create lexer and tokenize
        Lexer lexer(query);
        size_t tokenCount = 0;
        while (lexer.nextToken()->getType() != TokenType::END_OF_FILE) {
            ++tokenCount;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        double ms = duration.count();

        // Update statistics
        totalTokens += tokenCount;
        totalTime += ms;
        minTime = std::min(minTime, ms);
        maxTime = std::max(maxTime, ms);
        times.push_back(ms);
    }

    // Calculate statistics
    double avgTime = totalTime / iterations;
    double avgTokensPerRun = static_cast<double>(totalTokens) / iterations;

    // Calculate standard deviation
    double variance = 0.0;
    for (double time : times) {
        variance += (time - avgTime) * (time - avgTime);
    }
    double stdDev = std::sqrt(variance / iterations);

    // Sort times for percentiles
    std::sort(times.begin(), times.end());
    double median = (iterations % 2 == 0)
        ? (times[iterations / 2 - 1] + times[iterations / 2]) / 2
        : times[iterations / 2];
    double p95 = times[static_cast<size_t>(iterations * 0.95)];
    double p99 = times[static_cast<size_t>(iterations * 0.99)];

    // Print results
    std::cout << std::fixed << std::setprecision(3)
        << "Results:\n"
        << "- Average time: " << avgTime << " ms\n"
        << "- Median time: " << median << " ms\n"
        << "- Min time: " << minTime << " ms\n"
        << "- Max time: " << maxTime << " ms\n"
        << "- Std Dev: " << stdDev << " ms\n"
        << "- 95th percentile: " << p95 << " ms\n"
        << "- 99th percentile: " << p99 << " ms\n"
        << "- Tokens per run: " << avgTokensPerRun << "\n"
        << "- Tokens per millisecond: " << avgTokensPerRun / avgTime << "\n";

    std::cout << "\nDistribution:\n";
    const int buckets = 10;
    std::vector<int> histogram(buckets, 0);
    double bucketSize = (maxTime - minTime) / buckets;
    for (double time : times) {
        int bucket = static_cast<int>((time - minTime) / bucketSize);
        if (bucket >= buckets) bucket = buckets - 1;
        ++histogram[bucket];
    }

    // Print histogram
    for (int i = 0; i < buckets; ++i) {
        double start = minTime + i * bucketSize;
        double end = start + bucketSize;
        std::cout << std::fixed << std::setprecision(2)
            << "[" << start << " - " << end << " ms]: "
            << std::string(histogram[i] * 50 / iterations, '*') << " "
            << histogram[i] << "\n";
    }

    std::cout << std::string(50, '=') << std::endl;
}

/**
 * @brief Run basic lexer tests covering fundamental SQL constructs
 */
void runBasicTests(std::string username) {
    printTestHeader("Basic Lexer Tests");
    std::cout << "Current Date and Time (UTC): " << getCurrentTimestamp() << std::endl;
    std::cout << "Test Runner: " << username << "\n\n";

    // Test cases structure
    struct TestCase {
        std::string name;
        std::string query;
        std::function<void(const std::vector<std::unique_ptr<Token>>&)> validator;
    };

    std::vector<TestCase> testCases = {
        // Simple SELECT
        {
            "Simple SELECT",
            "SELECT * FROM users;",
            [](const auto& tokens) {
                if (tokens.size() != 6) { // SELECT, *, FROM, users, ;
                    throw std::runtime_error("Unexpected token count");
                }
                // Verify token types
                assert(tokens[0]->getType() == TokenType::KEYWORD);
                assert(tokens[1]->getType() == TokenType::OPERATOR);
                assert(tokens[2]->getType() == TokenType::KEYWORD);
                assert(tokens[3]->getType() == TokenType::IDENTIFIER);
                assert(tokens[4]->getType() == TokenType::PUNCTUATOR);
            }
        },

        // Basic literals
        {
            "Basic Literals",
            "SELECT 42, 3.14, 'hello', TRUE, FALSE, NULL;",
            [](const auto& tokens) {
            // Skip SELECT and validate literals
            for (size_t i = 1; i < tokens.size() - 1; i += 2) { // Skip commas
                assert(tokens[i]->getType() == TokenType::LITERAL);
            }
        }
    },

        // Basic operators
        {
            "Basic Operators",
            "SELECT col1 + col2 * col3 / col4 - col5;",
            [](const auto& tokens) {
                std::vector<std::string> expectedOps = {"+", "*", "/", "-"};
                int opIndex = 0;
                for (const auto& token : tokens) {
                    if (token->getType() == TokenType::OPERATOR) {
                        assert(token->getValue() == expectedOps[opIndex++]);
                    }
                }
            }
        },

        // Basic identifiers
        {
            "Basic Identifiers",
            "SELECT table1.column1, schema2.table2.column2 FROM schema1.table3;",
            [](const auto& tokens) {
            // Verify qualified identifiers
            assert(dynamic_cast<IdentifierToken*>(tokens[1].get()));
            assert(dynamic_cast<IdentifierToken*>(tokens[5].get()));
            assert(dynamic_cast<IdentifierToken*>(tokens[9].get()));
        }
    },

        // Basic comments
        {
            "Basic Comments",
            "SELECT col1 -- This is a comment\n/* Multi-line\ncomment */FROM table1;",
            [](const auto& tokens) {
                bool foundSingleLine = false;
                bool foundMultiLine = false;
                for (const auto& token : tokens) {
                    if (token->getType() == TokenType::COMMENT) {
                        auto comment = dynamic_cast<CommentToken*>(token.get());
                        if (comment->commentType == CommentType::SINGLE_LINE) {
                            foundSingleLine = true;
                        }
                        else if (comment->commentType == CommentType::MULTI_LINE) {
                            foundMultiLine = true;
                        }
                    }
                }
                assert(foundSingleLine && foundMultiLine);
            }
        },

        // Basic punctuation
        {
            "Basic Punctuation",
            "SELECT (col1, col2), [col3];",
            [](const auto& tokens) {
                std::vector<std::string> expectedPunct =
                    {"(", ",", ")", ",", ";"};
                int punctIndex = 0;
                for (const auto& token : tokens) {
                    if (token->getType() == TokenType::PUNCTUATOR) {
                        assert(token->getValue() == expectedPunct[punctIndex++]);
                    }
                }
            }
        },

        // Basic keywords
        {
            "Basic Keywords",
            "SELECT DISTINCT TOP 5 FROM WHERE GROUP BY HAVING ORDER BY",
            [](const auto& tokens) {
                std::vector<std::string> expectedKeywords = {
                    "SELECT", "DISTINCT", "TOP", "FROM", "WHERE",
                    "GROUP", "BY", "HAVING", "ORDER", "BY"
                };
                int keywordIndex = 0;
                for (const auto& token : tokens) {
                    if (token->getType() == TokenType::KEYWORD) {
                        assert(token->getValue() == expectedKeywords[keywordIndex++]);
                    }
                }
            }
        },

        // Basic functions
        {
            "Basic Functions",
            "SELECT COUNT(*), SUM(col1), AVG(col2), MAX(col3), MIN(col4)",
            [](const auto& tokens) {
                std::vector<std::string> expectedFuncs =
                    {"COUNT", "SUM", "AVG", "MAX", "MIN"};
                int funcIndex = 0;
                for (const auto& token : tokens) {
                    if (token->getType() == TokenType::FUNCTION) {
                        assert(token->getValue() == expectedFuncs[funcIndex++]);
                    }
                }
            }
        }
    };

    // Run all test cases
    for (const auto& test : testCases) {
        std::cout << "\nRunning test: " << test.name << std::endl;
        std::cout << "Query:\n" << test.query << std::endl;

        try {
            // Initialize lexer and collect tokens
            Lexer lexer(test.query);
            std::vector<std::unique_ptr<Token>> tokens;

            while (true) {
                auto token = lexer.nextToken();
                if (token->getType() == TokenType::END_OF_FILE) {
                    tokens.push_back(std::move(token));
                    break;
                }
                tokens.push_back(std::move(token));
            }

            // Print tokens
            std::cout << "\nTokens:" << std::endl;
            for (const auto& token : tokens) {
                printTokenDetails(*token);
            }

            // Validate tokens
            test.validator(tokens);
            std::cout << "✓ Test passed" << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << "✗ Test failed: " << e.what() << std::endl;
        }
    }

    std::cout << "\nBasic tests completed." << std::endl;
}

/**
 * @brief Run advanced lexer tests covering complex SQL constructs
 */
void runAdvancedTests(std::string username) {
    printTestHeader("Advanced Lexer Tests");
    std::cout << "Current Date and Time (UTC): " << getCurrentTimestamp() << std::endl;
    std::cout << "Test Runner: " << username << "\n\n";

    struct TestCase {
        std::string name;
        std::string query;
        std::string description;
    };

    std::vector<TestCase> testCases = {
        {
            "Complex SELECT with Subqueries",
            R"(
WITH RECURSIVE cte_name (col1, col2) AS (
    SELECT id, name
    FROM employees
    WHERE department_id IN (
        SELECT dept_id 
        FROM departments 
        WHERE budget > 1000000
    )
    UNION ALL
    SELECT e.id, e.name
    FROM employees e
    INNER JOIN cte_name c ON e.manager_id = c.col1
)
SELECT 
    e.employee_id,
    e.first_name || ' ' || e.last_name AS full_name,
    d.department_name,
    CASE 
        WHEN e.salary >= 100000 THEN 'High'
        WHEN e.salary >= 50000 THEN 'Medium'
        ELSE 'Low'
    END AS salary_band,
    (SELECT COUNT(*) 
     FROM projects p 
     WHERE p.lead_id = e.employee_id) AS projects_led,
    ARRAY[2023, 2024, 2025] AS forecast_years,
    '550e8400-e29b-41d4-a716-446655440000'::UUID AS employee_uuid,
    E'Manager\'s \nNotes:\nExcellent performer' AS notes,
    '{"skills": ["SQL", "Python"], "level": "Senior"}'::JSON AS profile,
    '<employee><rating>5</rating></employee>'::XML AS annual_review
FROM 
    cte_name c
    JOIN employees e ON c.col1 = e.employee_id
    LEFT JOIN departments d ON e.department_id = d.department_id
WHERE 
    e.hire_date BETWEEN '2024-01-01' AND CURRENT_DATE
    AND e.salary > (
        SELECT AVG(salary) * 1.1 
        FROM employees 
        WHERE department_id = e.department_id
    )
GROUP BY 
    e.employee_id, d.department_name
HAVING 
    COUNT(*) > 1
ORDER BY 
    e.salary DESC
LIMIT 10 OFFSET 0;
            )",
            "Tests recursive CTE, subqueries, joins, and complex expressions"
        },
        {
            "All Literal Types",
            R"(
SELECT
    -- String literals
    'Simple string',
    E'Escaped\nstring',
    $tag$Dollar quoted string$tag$,
    "Quoted identifier",
    [Bracketed identifier],
    
    -- Numeric literals
    42,
    -123,
    3.14159,
    1.23e-4,
    0xDEADBEEF,
    B'1010',
    
    -- Date/Time literals
    DATE '2024-06-15',
    TIME '14:30:00',
    TIMESTAMP '2024-06-15 14:30:00',
    INTERVAL '2' YEAR,
    INTERVAL '2-3' YEAR TO MONTH,
    INTERVAL '2 YEARS 3 MONTHS',
    
    -- Boolean literals
    TRUE,
    FALSE,
    NULL,
    
    -- Extended literals
    ARRAY[1, 2, 3],
    550e8400-e29b-41d4-a716-446655440000,
    {"key": "value"},
    <root>value</root>,
FROM dual;
            )",
            "Tests all supported literal types"
        },
        {
            "Complex Operators and Functions",
            R"(
SELECT
    -- Arithmetic operators
    a + b * (c - d) / e % f,
    
    -- Comparison operators
    x > y AND y <= z,
    a BETWEEN x AND y,
    str LIKE '_pattern%',
    val IN (1, 2, 3),
    
    -- Logical operators
    NOT (a OR b) AND c,
    CASE WHEN x THEN y ELSE z END,
    
    -- Bitwise operators
    a & b | c ^ d,
    
    -- String operators
    first_name || ' ' || last_name,
    
    -- Window functions
    ROW_NUMBER() OVER (PARTITION BY dept ORDER BY salary),
    RANK() OVER w,
    DENSE_RANK() OVER w,
    
    -- Aggregate functions
    COUNT(*),
    SUM(DISTINCT amount),
    AVG(NULLIF(value, 0)),
    STRING_AGG(name, ','),
    
    -- Date/time functions
    EXTRACT(YEAR FROM timestamp_col),
    DATE_TRUNC(month, date_col),
    
    -- Type casting
    CAST(x AS INTEGER),
    y::STRING,
    
    -- JSON operators
    data->>'name',
    data#>'{address,city}'
FROM 
    table_name
WINDOW w AS (PARTITION BY dept ORDER BY salary)
            )",
            "Tests operators, functions, and window clauses"
        },
        {
            "Comments and Special Syntax",
            R"(
-- Single line comment
/* Multi-line
   comment with nested /* comment */ 
   continuation */
SELECT 
    /* Inline comment */ column_name,
    another_column -- End of line comment
    
    -- Special syntax
    value::TYPE AS type_cast,     -- PostgreSQL type cast
    
    -- Special characters
    $tag$Special § characters © in $$ string$tag$,
    
    -- Nested structures
    (
        SELECT /* Nested comment */ *
        FROM (
            /* Deeply 
               nested 
               comment */
            SELECT * FROM inner_table
        ) AS sub
    )
FROM 
    table_name;
            )",
            "Tests comments and special syntax features"
        }
    };

    // Run all test cases
    for (const auto& test : testCases) {
        std::cout << "\nRunning test: " << test.name << std::endl;
        std::cout << "Description: " << test.description << std::endl;
        std::cout << "Query length: " << test.query.length() << " characters" << std::endl;

        try {
            // Create lexer with default configuration
            Lexer lexer(test.query);
            std::vector<std::unique_ptr<Token>> tokens;

            // Collect all tokens
            while (true) {
                auto token = lexer.nextToken();
                if (token->getType() == TokenType::END_OF_FILE) {
                    tokens.push_back(std::move(token));
                    break;
                }
                tokens.push_back(std::move(token));
            }

            // Print token statistics
            std::cout << "\nToken Statistics:" << std::endl;
            std::map<TokenType, int> tokenCounts;
            for (const auto& token : tokens) {
                tokenCounts[token->getType()]++;
            }

            for (const auto& [type, count] : tokenCounts) {
                std::cout << "- " << LexerUtils::tokenTypeToString(type)
                    << ": " << count << std::endl;
            }

            // Print detailed token information
            std::cout << "\nDetailed Token List:" << std::endl;
            for (const auto& token : tokens) {
                printTokenDetails(*token);
            }

            // Print lexer diagnostics
            printLexerDiagnostics(lexer);

            std::cout << "✓ Test passed" << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << "✗ Test failed: " << e.what() << std::endl;
        }

        std::cout << std::string(80, '-') << std::endl;
    }

    // Performance test with the most complex query
    std::cout << "\nRunning performance test..." << std::endl;
    measurePerformance(testCases[0].query, 100);

    std::cout << "\nAdvanced tests completed." << std::endl;
}

/**
 * @brief Run edge case tests to verify lexer behavior in unusual situations
 */
void runEdgeCaseTests(std::string username) {
    printTestHeader("Edge Case Tests");
    std::cout << "Current Date and Time (UTC): " << getCurrentTimestamp() << std::endl;
    std::cout << "Test Runner: " << username << "\n\n";

    struct TestCase {
        std::string name;
        std::string query;
        std::string description;
        bool shouldPass;
    };

    std::vector<TestCase> testCases = {
        {
            "Maximum Length Identifiers",
            std::string("SELECT ") + std::string(256, 'a') + " FROM " + std::string(256, 'b') + ";",
            "Tests identifiers at maximum length limit",
            true
        },
        {
            "Nested Comments",
            R"(
SELECT * FROM table_name
/* Outer comment
   /* Nested comment level 1
      /* Nested comment level 2 */
      Still in level 1 */
   Back to outer comment */
WHERE id > 0;
            )",
            "Tests deeply nested comments",
            true
        },
        {
            "Mixed Quote Types",
            R"(
SELECT 'Simple string', "Quoted identifier", $tag$Dollar quoted$tag$,
       $$ Dollar quoted no tag $$,
       E'Escaped \n string',
       N'Unicode string',
       U&'Unicode string',
       U&'Unicode string' UESCAPE '+'
FROM "Table.Name"."Column.Name";
            )",
            "Tests various types of string quoting",
            true
        },
        {
            "Extreme Numeric Values",
            R"(
SELECT 
    9223372036854775807,  -- Max INT64
    -9223372036854775808, -- Min INT64
    1.7976931348623157e+308, -- Max Double
    2.2250738585072014e-308, -- Min positive Double
    0x7FFFFFFFFFFFFFFF,   -- Max INT64 in hex
    B'1111111111111111'   -- 16 bits
FROM dual;
            )",
            "Tests numeric literals at their limits",
            true
        },
        {
            "Complex Nested Structures",
            R"(
WITH RECURSIVE cte AS (
    SELECT (((((1)))))), 
           ((SELECT ((SELECT (1)) FROM dual))) AS nested,
           ARRAY[ARRAY[ARRAY[1,2,3]]]
    FROM (((table_name))) AS deeply_nested
)
SELECT * FROM (((cte)));
            )",
            "Tests deeply nested parentheses and structures",
            true
        },
        {
            "Special Characters in Identifiers",
            R"(
SELECT 
    "Column#1",
    "Column@2",
    "Column$3",
    "Column%4",
    "Column&5",
    "Column*6",
    "Column.7",
    "Column:8",
    "Column;9"
FROM "Table#1";
            )",
            "Tests special characters in quoted identifiers",
            true
        },
        {
            "Empty and Whitespace Elements",
            R"(
SELECT '', "", $tag$$tag$, /* */, -- 
       '     ', "     ", 
       '
', "
"
FROM dual WHERE col = '';
            )",
            "Tests empty strings, comments, and whitespace",
            true
        },
        {
            "Unicode Characters",
            R"(
SELECT "გამარჯობა", '世界', "Привет",
       '🌍', '👋', '🚀',
       N'über', U&'über',
       '\\u0001', '\\u0000', '\\uffff'
FROM "国际" WHERE "région" = 'région';
            )",
            "Tests Unicode characters in strings and identifiers",
            true
        },
        {
            "Edge Case Operators",
            R"(
SELECT 
    col1 !!= col2,  -- PostgreSQL not equal
    col1 <=> col2,  -- MySQL null-safe equal
    col1 ::= col2,  -- Custom operator
    col1 >>> col2,  -- Bit shift
    col1 ??? col2,  -- Custom operator
    col1 @@@ col2   -- Custom operator
FROM dual;
            )",
            "Tests unusual and custom operators",
            true
        },
        {
            "Dollar Quote Variations",
            R"(
SELECT 
    $tag$nested $other_tag$content$other_tag$ here$tag$,
    $$no tag$$,
    $tag$empty$tag$,
    $a$single char tag$a$,
    $1$numeric tag$1$,
    $TAG$uppercase tag$TAG$,
    $t$g$nested dollar$t$g$
FROM dual;
            )",
            "Tests various dollar quote formats",
            true
        },
        {
            "Boundary Whitespace",
            R"(   
                
                

SELECT col FROM tab
                
                

WHERE id > 0
                
                

ORDER BY col;
                
                

            )",
            "Tests excessive whitespace and newlines",
            true
        },
        {
            "Mixed Case Identifiers",
            R"(
SELECT 
    ColumnName,
    COLUMN_NAME,
    column_name,
    "ColumnName",
    "COLUMN_NAME",
    "column_name"
FROM 
    TableName,
    TABLE_NAME,
    table_name;
            )",
            "Tests case sensitivity in identifiers",
            true
        }
    };

    // Run all test cases
    for (const auto& test : testCases) {
        std::cout << "\nRunning test: " << test.name << std::endl;
        std::cout << "Description: " << test.description << std::endl;
        std::cout << "Expected: " << (test.shouldPass ? "Pass" : "Fail") << std::endl;
        std::cout << "Query length: " << test.query.length() << " characters" << std::endl;

        try {
            // Create lexer with default configuration
            Lexer lexer(test.query);
            std::vector<std::unique_ptr<Token>> tokens;

            // Collect all tokens
            while (true) {
                auto token = lexer.nextToken();
                if (token->getType() == TokenType::END_OF_FILE) {
                    tokens.push_back(std::move(token));
                    break;
                }
                tokens.push_back(std::move(token));
            }

            // Token type statistics
            std::cout << "\nToken Statistics:" << std::endl;
            std::map<TokenType, int> tokenCounts;
            std::map<std::string, int> specialCounts;

            for (const auto& token : tokens) {
                tokenCounts[token->getType()]++;

                // Count special cases
                if (token->getValue().empty()) {
                    specialCounts["Empty tokens"]++;
                }
                if (token->getValue().length() > 100) {
                    specialCounts["Long tokens (>100 chars)"]++;
                }
                if (token->getValue().find('\n') != std::string::npos) {
                    specialCounts["Multi-line tokens"]++;
                }
            }

            // Print statistics
            for (const auto& [type, count] : tokenCounts) {
                std::cout << "- " << LexerUtils::tokenTypeToString(type)
                    << ": " << count << std::endl;
            }

            // Print special cases
            if (!specialCounts.empty()) {
                std::cout << "\nSpecial Cases:" << std::endl;
                for (const auto& [category, count] : specialCounts) {
                    std::cout << "- " << category << ": " << count << std::endl;
                }
            }

            // Print detailed token information
            std::cout << "\nDetailed Token List:" << std::endl;
            for (const auto& token : tokens) {
                printTokenDetails(*token);
            }

            // Print lexer diagnostics
            printLexerDiagnostics(lexer);

            if (test.shouldPass) {
                std::cout << "✓ Test passed as expected" << std::endl;
            }
            else {
                std::cout << "⚠ Test passed but was expected to fail" << std::endl;
            }
        }
        catch (const std::exception& e) {
            if (!test.shouldPass) {
                std::cout << "✓ Test failed as expected: " << e.what() << std::endl;
            }
            else {
                std::cout << "✗ Test failed unexpectedly: " << e.what() << std::endl;
            }
        }

        std::cout << std::string(80, '-') << std::endl;
    }

    std::cout << "\nEdge case tests completed." << std::endl;
}

/**
 * @brief Run error handling tests to verify lexer's error detection and reporting
 */
void runErrorHandlingTests(std::string username) {
    printTestHeader("Error Handling Tests");
    std::cout << "Current Date and Time (UTC): " << getCurrentTimestamp() << std::endl;
    std::cout << "Test Runner: " << username << "\n\n";

    struct TestCase {
        std::string name;
        std::string query;
        std::string expectedError;
        ErrorSeverity expectedSeverity;
    };

    std::vector<TestCase> testCases = {
        {
            "Unterminated String Literal",
            "SELECT 'unclosed string FROM table;",
            "Unterminated string literal",
            ErrorSeverity::ERROR
        },
        {
            "Invalid Identifier Characters",
            "SELECT #invalid@name FROM table;",
            "Invalid character in identifier",
            ErrorSeverity::ERROR
        },
        {
            "Unterminated Comment",
            "SELECT col /* This comment never ends FROM table;",
            "Unterminated multi-line comment",
            ErrorSeverity::ERROR
        },
        {
            "Invalid Numeric Literal",
            "SELECT 12.34.56 FROM table;",
            "Invalid numeric literal",
            ErrorSeverity::ERROR
        },
        {
            "Invalid Date Format",
            "SELECT DATE '2024-13-45' FROM table;",
            "Invalid date literal",
            ErrorSeverity::ERROR
        },
        {
            "Invalid Time Format",
            "SELECT TIME '25:61:99' FROM table;",
            "Invalid time literal",
            ErrorSeverity::ERROR
        },
        {
            "Invalid UUID Format",
            "SELECT not-a-valid-uuid FROM table;",
            "Invalid UUID literal",
            ErrorSeverity::ERROR
        },
        {
            "Invalid JSON Format",
            "SELECT {not valid json} FROM table;",
            "Invalid JSON literal",
            ErrorSeverity::ERROR
        },
        {
            "Invalid XML Format",
            "SELECT <unclosed> FROM table;",
            "Invalid XML literal",
            ErrorSeverity::ERROR
        },
        {
            "Invalid Dollar Quote",
            "SELECT $tag$unclosed dollar quote FROM table;",
            "Unterminated dollar-quoted string",
            ErrorSeverity::ERROR
        },
        {
            "Invalid Binary Literal",
            "SELECT B'1012' FROM table;",
            "Invalid binary literal",
            ErrorSeverity::ERROR
        },
        {
            "Invalid Hex Literal",
            "SELECT 0xGHIJ FROM table;",
            "Invalid hexadecimal literal",
            ErrorSeverity::ERROR
        },
        {
            "Mismatched Quotes",
            "SELECT \"mixed' quotes\" FROM table;",
            "Mismatched quote characters",
            ErrorSeverity::ERROR
        },
        {
            "Invalid Array Format",
            "SELECT ARRAY[1, 2, 3 FROM table;",
            "Unterminated array literal",
            ErrorSeverity::ERROR
        },
        {
            "Overflow in Numeric Literal",
            "SELECT 99999999999999999999999999999999 FROM table;",
            "Numeric literal overflow",
            ErrorSeverity::ERROR
        }
    };

    std::cout << "Testing error handling with " << testCases.size() << " test cases\n";
    std::cout << "Current UTC Time: " << getCurrentTimestamp() << "\n";
    std::cout << "Testing User: " << username <<"\n\n";

    int passCount = 0;
    int failCount = 0;

    // Run all test cases
    for (const auto& test : testCases) {
        std::cout << "\nRunning test: " << test.name << std::endl;
        std::cout << "Query:\n" << test.query << std::endl;
        std::cout << "Expected error: " << test.expectedError << std::endl;
        std::cout << "Expected severity: " << static_cast<int>(test.expectedSeverity) << std::endl;

        bool errorCaught = false;
        std::string actualError;

        try {
            // Create lexer with strict mode enabled
            LexerConfig config;
            config.strictMode = true;
            Lexer lexer(test.query, config);

            // Try to tokenize the entire input
            while (true) {
                auto token = lexer.nextToken();
                if (token->getType() == TokenType::END_OF_FILE) break;
            }

            // If we get here, check if there were any errors logged
            if (lexer.hasErrors(test.expectedSeverity)) {
                const auto& errorLog = lexer.getErrorLog();
                if (!errorLog.empty()) {
                    errorCaught = true;
                    actualError = errorLog.back().message;
                }
            }
        }
        catch (const LexerError& e) {
            errorCaught = true;
            actualError = e.what();
        }
        catch (const std::exception& e) {
            errorCaught = true;
            actualError = e.what();
        }

        // Verify the result
        if (errorCaught) {
            if (actualError.find(test.expectedError) != std::string::npos) {
                std::cout << "✓ Test passed: Error caught as expected" << std::endl;
                std::cout << "Actual error: " << actualError << std::endl;
                passCount++;
            }
            else {
                std::cout << "⚠ Test partially failed: Error caught but message mismatch" << std::endl;
                std::cout << "Expected: " << test.expectedError << std::endl;
                std::cout << "Actual: " << actualError << std::endl;
                failCount++;
            }
        }
        else {
            std::cout << "✗ Test failed: No error caught" << std::endl;
            failCount++;
        }

        std::cout << std::string(80, '-') << std::endl;
    }

    // Print summary
    std::cout << "\nError Handling Test Summary:" << std::endl;
    std::cout << "Total tests: " << testCases.size() << std::endl;
    std::cout << "Passed: " << passCount << std::endl;
    std::cout << "Failed: " << failCount << std::endl;
    std::cout << "Success rate: " << (passCount * 100.0 / testCases.size()) << "%" << std::endl;

    // Additional error handling verification
    std::cout << "\nTesting error recovery capabilities..." << std::endl;

    const std::string recoveryTest = R"(
SELECT 'unclosed string
FROM table;
SELECT * FROM table2;
SELECT 'another unclosed string
FROM table3;
SELECT * FROM table4;
    )";

    try {
        Lexer lexer(recoveryTest);
        int validTokens = 0;
        int errorTokens = 0;

        while (true) {
            try {
                auto token = lexer.nextToken();
                if (token->getType() == TokenType::END_OF_FILE) break;
                validTokens++;
            }
            catch (const LexerError&) {
                errorTokens++;
                // Continue after error
            }
        }

        std::cout << "Error recovery test results:" << std::endl;
        std::cout << "- Valid tokens processed: " << validTokens << std::endl;
        std::cout << "- Errors encountered: " << errorTokens << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Error recovery test failed: " << e.what() << std::endl;
    }

    std::cout << "\nError handling tests completed." << std::endl;
}

/**
 * @brief Run performance tests to measure lexer efficiency
 */
void runPerformanceTests(std::string username) {
    printTestHeader("Performance Tests");
    std::cout << "Current Date and Time (UTC): " << getCurrentTimestamp() << std::endl;
    std::cout << "Test Runner: " << username << "\n\n";

    struct TestCase {
        std::string name;
        std::string query;
        int iterations;
        std::string description;
    };

    // Test cases with increasing complexity
    std::vector<TestCase> testCases = {
        {
            "Simple Query",
            "SELECT * FROM users WHERE id = 1;",
            10000,
            "Basic SELECT query"
        },
        {
            "Medium Query",
            R"(
SELECT u.id, u.name, d.name as dept_name
FROM users u
JOIN departments d ON u.dept_id = d.id
WHERE u.salary > 50000
  AND d.location = 'NYC'
ORDER BY u.name;
            )",
            5000,
            "Query with joins and conditions"
        },
        {
            "Complex Query",
            R"(
WITH RECURSIVE emp_hierarchy AS (
    SELECT employee_id, manager_id, name, 1 as level
    FROM employees
    WHERE manager_id IS NULL
    UNION ALL
    SELECT e.employee_id, e.manager_id, e.name, eh.level + 1
    FROM employees e
    JOIN emp_hierarchy eh ON e.manager_id = eh.employee_id
)
SELECT 
    e.employee_id,
    e.name,
    d.department_name,
    p.project_name,
    (SELECT COUNT(*) FROM assignments a WHERE a.employee_id = e.employee_id) as assignment_count,
    ARRAY(SELECT skill FROM employee_skills WHERE employee_id = e.employee_id) as skills,
    JSON_BUILD_OBJECT(
        'title', e.title,
        'level', e.level,
        'certifications', (SELECT JSON_AGG(cert_name) FROM employee_certifications WHERE employee_id = e.employee_id)
    ) as employee_info
FROM emp_hierarchy e
JOIN departments d ON e.department_id = d.department_id
LEFT JOIN projects p ON e.project_id = p.project_id
WHERE e.status = 'ACTIVE'
GROUP BY e.employee_id, d.department_name, p.project_name
HAVING COUNT(*) > 0
ORDER BY e.level, e.name;
            )",
            1000,
            "Complex query with CTE, subqueries, and JSON"
        },
        {
            "Large Volume Simple",
            [] {
                std::string query = "SELECT id, name, value FROM table_name WHERE id IN (";
                for (int i = 1; i <= 1000; i++) {
                    query += std::to_string(i);
                    if (i < 1000) query += ",";
                }
                query += ");";
                return query;
            }(),
            1000,
            "Simple query with large IN clause"
        },
        {
            "Many Literals",
            [] {
                std::stringstream query;
                query << "SELECT ";
                for (int i = 1; i <= 100; i++) {
                    query << "'string" << i << "', ";
                    query << i << ", ";
                    query << i << "." << i << ", ";
                    query << "TRUE, ";
                    query << "NULL";
                    query << (i < 100 ? ", " : " ");
                }
                query << "FROM dual;";
                return query.str();
            }(),
            1000,
            "Query with many literals of different types"
        }
    };

    struct PerformanceMetrics {
        double totalTime;      // Total execution time
        double avgTimePerRun;  // Average time per run
        double tokenPerSec;    // Tokens processed per second
        size_t tokenCount;     // Total tokens processed
        double memoryUsage;    // Approximate memory usage
        std::vector<double> runTimes; // Individual run times
    };

    std::cout << "Running performance tests with " << testCases.size() << " test cases\n";
    std::vector<PerformanceMetrics> allMetrics;

    for (const auto& test : testCases) {
        std::cout << "\nRunning test: " << test.name << std::endl;
        std::cout << "Description: " << test.description << std::endl;
        std::cout << "Iterations: " << test.iterations << std::endl;
        std::cout << "Query length: " << test.query.length() << " characters\n";

        PerformanceMetrics metrics{};
        metrics.runTimes.reserve(test.iterations);

        // Warm-up run
        {
            Lexer lexer(test.query);
            while (lexer.nextToken()->getType() != TokenType::END_OF_FILE) {}
        }

        // Main performance test
        for (int i = 0; i < test.iterations; ++i) {
            auto startTime = std::chrono::high_resolution_clock::now();
            size_t tokens = 0;

            try {
                Lexer lexer(test.query);
                while (lexer.nextToken()->getType() != TokenType::END_OF_FILE) {
                    tokens++;
                }

                auto endTime = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();

                metrics.runTimes.push_back(duration);
                metrics.totalTime += duration;
                metrics.tokenCount += tokens;
            }
            catch (const std::exception& e) {
                std::cout << "Error during iteration " << i << ": " << e.what() << std::endl;
                break;
            }
        }

        // Calculate metrics
        metrics.avgTimePerRun = metrics.totalTime / test.iterations;
        metrics.tokenPerSec = (metrics.tokenCount * 1000.0) / metrics.totalTime; // Convert to per second

        // Calculate percentiles
        std::sort(metrics.runTimes.begin(), metrics.runTimes.end());
        double median = metrics.runTimes[test.iterations / 2];
        double p95 = metrics.runTimes[static_cast<size_t>(test.iterations * 0.95)];
        double p99 = metrics.runTimes[static_cast<size_t>(test.iterations * 0.99)];

        // Calculate standard deviation
        double variance = 0.0;
        for (double time : metrics.runTimes) {
            variance += (time - metrics.avgTimePerRun) * (time - metrics.avgTimePerRun);
        }
        double stdDev = std::sqrt(variance / test.iterations);

        // Print results
        std::cout << "\nPerformance Results:" << std::endl;
        std::cout << std::fixed << std::setprecision(3)
            << "- Total time: " << metrics.totalTime << " ms\n"
            << "- Average time per run: " << metrics.avgTimePerRun << " ms\n"
            << "- Median time: " << median << " ms\n"
            << "- 95th percentile: " << p95 << " ms\n"
            << "- 99th percentile: " << p99 << " ms\n"
            << "- Standard deviation: " << stdDev << " ms\n"
            << "- Tokens processed: " << metrics.tokenCount << "\n"
            << "- Tokens per second: " << metrics.tokenPerSec << "\n";

        // Print histogram
        std::cout << "\nDistribution Histogram:\n";
        const int buckets = 10;
        std::vector<int> histogram(buckets, 0);
        double minTime = metrics.runTimes.front();
        double maxTime = metrics.runTimes.back();
        double bucketSize = (maxTime - minTime) / buckets;

        for (double time : metrics.runTimes) {
            int bucket = std::min(static_cast<int>((time - minTime) / bucketSize), buckets - 1);
            histogram[bucket]++;
        }

        for (int i = 0; i < buckets; ++i) {
            double start = minTime + i * bucketSize;
            double end = start + bucketSize;
            std::cout << std::fixed << std::setprecision(2)
                << "[" << start << " - " << end << " ms]: "
                << std::string(histogram[i] * 50 / test.iterations, '*')
                << " " << histogram[i] << "\n";
        }

        allMetrics.push_back(metrics);
        std::cout << std::string(80, '-') << std::endl;
    }

    // Print comparative analysis
    std::cout << "\nComparative Analysis:\n";
    std::cout << std::setw(20) << "Test Case"
        << std::setw(15) << "Avg Time (ms)"
        << std::setw(15) << "Tokens/sec"
        << std::setw(15) << "Total Tokens" << std::endl;

    for (size_t i = 0; i < testCases.size(); ++i) {
        std::cout << std::fixed << std::setprecision(3)
            << std::setw(20) << testCases[i].name
            << std::setw(15) << allMetrics[i].avgTimePerRun
            << std::setw(15) << allMetrics[i].tokenPerSec
            << std::setw(15) << allMetrics[i].tokenCount << std::endl;
    }

    std::cout << "\nPerformance tests completed." << std::endl;
}

/**
 * @brief Run interactive tests allowing user input and real-time token analysis
 */
void runInteractiveTests(std::string username) {
    printTestHeader("Interactive Lexer Testing Environment");
    std::cout << "Current Date and Time (UTC): " << getCurrentTimestamp() << std::endl;
    std::cout << "Test Runner: " << username << "\n\n";

    std::cout << "Interactive SQL Lexer Testing Environment\n"
        << "----------------------------------------\n"
        << "Commands:\n"
        << "  :q, :quit       - Exit the interactive mode\n"
        << "  :h, :help       - Show this help message\n"
        << "  :c, :clear      - Clear the screen\n"
        << "  :d, :debug      - Toggle debug mode\n"
        << "  :s, :stats      - Show lexer statistics\n"
        << "  :p, :perf       - Show performance metrics\n"
        << "  :r, :reset      - Reset lexer state\n"
        << "  :v, :verbose    - Toggle verbose output\n"
        << "Enter SQL queries to analyze, or use commands above.\n\n";

    bool running = true;
    bool debugMode = false;
    bool verboseMode = false;
    std::vector<std::string> history;
    size_t totalTokens = 0;
    size_t totalQueries = 0;
    auto startTime = std::chrono::high_resolution_clock::now();

    while (running) {
        std::string input;
        std::string line;

        // Prompt
        std::cout << "\nsql> ";
        if (!std::getline(std::cin, line)) break;   // EOF (Ctrl+D on Unix, Ctrl+Z+Enter on Windows)
        if (line.empty()) continue;                  // empty input - re-enter

        // Command processing
        if (line[0] == ':') {
            std::string cmd = line.substr(1);
            if (cmd == "q" || cmd == "quit") {
                running = false;
                continue;
            }
            else if (cmd == "h" || cmd == "help") {
                std::cout << "\nCommands:\n"
                    << "  :q, :quit       - Exit the interactive mode\n"
                    << "  :h, :help       - Show this help message\n"
                    << "  :c, :clear      - Clear the screen\n"
                    << "  :d, :debug      - Toggle debug mode\n"
                    << "  :s, :stats      - Show lexer statistics\n"
                    << "  :p, :perf       - Show performance metrics\n"
                    << "  :r, :reset      - Reset lexer state\n"
                    << "  :v, :verbose    - Toggle verbose output\n";
                continue;
            }
            else if (cmd == "c" || cmd == "clear") {
#ifdef _WIN32
                system("cls");
#else
                system("clear");
#endif
                continue;
            }
            else if (cmd == "d" || cmd == "debug") {
                debugMode = !debugMode;
                std::cout << "Debug mode: " << (debugMode ? "ON" : "OFF") << std::endl;
                continue;
            }
            else if (cmd == "v" || cmd == "verbose") {
                verboseMode = !verboseMode;
                std::cout << "Verbose mode: " << (verboseMode ? "ON" : "OFF") << std::endl;
                continue;
            }
            else if (cmd == "s" || cmd == "stats") {
                auto currentTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);

                std::cout << "\nLexer Statistics:\n"
                    << "- Total queries processed: " << totalQueries << "\n"
                    << "- Total tokens processed: " << totalTokens << "\n"
                    << "- Average tokens per query: "
                    << (totalQueries > 0 ? totalTokens / totalQueries : 0) << "\n"
                    << "- Session duration: " << duration.count() << " seconds\n"
                    << "- Tokens per second: "
                    << (duration.count() > 0 ? totalTokens / duration.count() : 0) << "\n";
                continue;
            }
            else if (cmd == "p" || cmd == "perf") {
                if (history.empty()) {
                    std::cout << "No queries to analyze.\n";
                    continue;
                }

                // Run performance test on the last query
                std::cout << "\nRunning performance test on last query...\n";
                measurePerformance(history.back(), 1000);
                continue;
            }
            else if (cmd == "r" || cmd == "reset") {
                history.clear();
                totalTokens = 0;
                totalQueries = 0;
                startTime = std::chrono::high_resolution_clock::now();
                std::cout << "Lexer state reset.\n";
                continue;
            }
            else {
                std::cout << "Unknown command. Use :h for help.\n";
                continue;
            }
        }

        // Not a command — start multi-line SQL entry until the empty line
        input += line + "\n";
        while (true) {
            std::cout << "...> ";
            if (!std::getline(std::cin, line)) {
                running = false;  // EOF — end all REPL
                break;
            }
            if (line.empty()) {
                break;           // empty line ends the SQL input
            }
            input += line + "\n";
        }

        if (!running) break;

        // Query processing
        try {
            history.push_back(input);
            totalQueries++;

            // Create lexer with current configuration
            LexerConfig config;
            config.strictMode = debugMode;
            Lexer lexer(input, config);

            // Token collection and analysis
            std::vector<std::unique_ptr<Token>> tokens;
            std::map<TokenType, int> tokenCounts;

            auto analysisStart = std::chrono::high_resolution_clock::now();

            while (true) {
                auto token = lexer.nextToken();
                if (token->getType() == TokenType::END_OF_FILE) {
                    tokens.push_back(std::move(token));
                    break;
                }
                tokenCounts[token->getType()]++;
                totalTokens++;
                tokens.push_back(std::move(token));
            }

            auto analysisEnd = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(analysisEnd - analysisStart);

            // Output results
            std::cout << "\nAnalysis completed in " << duration.count() << " microseconds\n";

            if (verboseMode || debugMode) {
                std::cout << "\nToken distribution:\n";
                for (const auto& [type, count] : tokenCounts) {
                    std::cout << "- " << LexerUtils::tokenTypeToString(type)
                        << ": " << count << "\n";
                }
            }

            std::cout << "\nTokens:\n";
            for (const auto& token : tokens) {
                if (token->getType() == TokenType::END_OF_FILE) continue;
                printTokenDetails(*token);
            }

            if (debugMode) {
                printLexerDiagnostics(lexer);
            }

        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }

    // Print session summary
    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);

    std::cout << "\nSession Summary:\n"
        << "- Total time: " << totalDuration.count() << " seconds\n"
        << "- Queries processed: " << totalQueries << "\n"
        << "- Total tokens: " << totalTokens << "\n"
        << "- Average tokens per query: "
        << (totalQueries > 0 ? totalTokens / totalQueries : 0) << "\n";

    std::cout << "\nInteractive testing session ended.\n";
}