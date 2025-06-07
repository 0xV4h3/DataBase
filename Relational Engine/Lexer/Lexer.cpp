/**
 * @file Lexer.cpp
 * @brief Implementation of SQL Lexer with advanced token support.
 * @details
 * Implements lexical analysis for SQL, supporting:
 * - Keywords, functions, identifiers
 * - All literal types (string, numeric, date/time, JSON, XML, UUID, etc.)
 * - Operators and punctuators
 * - Comments and special tokens
 * With extensive validation and error handling.
 */

#include "Lexer.hpp"

 // Token system includes
#include "Token.hpp"
#include "TokenEnums.hpp"

// Literal value includes
#include "LiteralValue.hpp"
#include "IntegerLiteralValue.hpp"
#include "FloatLiteralValue.hpp"
#include "StringLiteralValue.hpp"
#include "CharLiteralValue.hpp"
#include "BooleanLiteralValue.hpp"
#include "NullLiteralValue.hpp"
#include "BinaryLiteralValue.hpp"
#include "HEXLiteralValue.hpp"
#include "ArrayLiteralValue.hpp"
#include "DateLiteralValue.hpp"
#include "TimeLiteralValue.hpp"
#include "DateTimeLiteralValue.hpp"
#include "IntervalLiteralValue.hpp"
#include "UUIDLiteralValue.hpp"
#include "JSONLiteralValue.hpp"
#include "XMLLiteralValue.hpp"
#include "EscapeStringLiteralValue.hpp"

// Token info includes
#include "KeywordInfo.hpp"
#include "FunctionInfo.hpp"
#include "IdentifierInfo.hpp"
#include "OperatorInfo.hpp"
#include "PunctuatorInfo.hpp"

// Standard library includes
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <regex>
#include <chrono>
#include <iomanip>
#include <unordered_set>

// === Static Maps ===

/**
 * @brief Map for Data Manipulation Language keywords
 */
const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> Lexer::dmlMap = {
    {"SELECT", std::make_shared<KeywordInfo>("SELECT", KeywordCategory::DML, static_cast<int>(DMLKeyword::SELECT))},
    {"INSERT", std::make_shared<KeywordInfo>("INSERT", KeywordCategory::DML, static_cast<int>(DMLKeyword::INSERT))},
    {"UPDATE", std::make_shared<KeywordInfo>("UPDATE", KeywordCategory::DML, static_cast<int>(DMLKeyword::UPDATE))},
    {"DELETE", std::make_shared<KeywordInfo>("DELETE", KeywordCategory::DML, static_cast<int>(DMLKeyword::DELETE))},
    {"MERGE", std::make_shared<KeywordInfo>("MERGE", KeywordCategory::DML, static_cast<int>(DMLKeyword::MERGE))},
    {"EXECUTE", std::make_shared<KeywordInfo>("EXECUTE", KeywordCategory::DML, static_cast<int>(DMLKeyword::EXECUTE))},
    {"VALUES", std::make_shared<KeywordInfo>("VALUES", KeywordCategory::DML, static_cast<int>(DMLKeyword::VALUES))},
    {"OUTPUT", std::make_shared<KeywordInfo>("OUTPUT", KeywordCategory::DML, static_cast<int>(DMLKeyword::OUTPUT))},
    {"DEFAULT", std::make_shared<KeywordInfo>("DEFAULT", KeywordCategory::DML, static_cast<int>(DMLKeyword::DEFAULT))},
    {"INTO", std::make_shared<KeywordInfo>("INTO", KeywordCategory::DML, static_cast<int>(DMLKeyword::INTO))},
    {"RETURNING", std::make_shared<KeywordInfo>("RETURNING", KeywordCategory::DML, static_cast<int>(DMLKeyword::RETURNING))},
    {"USING", std::make_shared<KeywordInfo>("USING", KeywordCategory::DML, static_cast<int>(DMLKeyword::USING))}
};

/**
 * @brief Map for Data Definition Language keywords
 */
const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> Lexer::ddlMap = {
    {"CREATE", std::make_shared<KeywordInfo>("CREATE", KeywordCategory::DDL, static_cast<int>(DDLKeyword::CREATE))},
    {"ALTER", std::make_shared<KeywordInfo>("ALTER", KeywordCategory::DDL, static_cast<int>(DDLKeyword::ALTER))},
    {"DROP", std::make_shared<KeywordInfo>("DROP", KeywordCategory::DDL, static_cast<int>(DDLKeyword::DROP))},
    {"TRUNCATE", std::make_shared<KeywordInfo>("TRUNCATE", KeywordCategory::DDL, static_cast<int>(DDLKeyword::TRUNCATE))},
    {"TABLE", std::make_shared<KeywordInfo>("TABLE", KeywordCategory::DDL, static_cast<int>(DDLKeyword::TABLE))},
    {"VIEW", std::make_shared<KeywordInfo>("VIEW", KeywordCategory::DDL, static_cast<int>(DDLKeyword::VIEW))},
    {"INDEX", std::make_shared<KeywordInfo>("INDEX", KeywordCategory::DDL, static_cast<int>(DDLKeyword::INDEX))},
    {"SEQUENCE", std::make_shared<KeywordInfo>("SEQUENCE", KeywordCategory::DDL, static_cast<int>(DDLKeyword::SEQUENCE))},
    {"CONSTRAINT", std::make_shared<KeywordInfo>("CONSTRAINT", KeywordCategory::DDL, static_cast<int>(DDLKeyword::CONSTRAINT))},
    {"TRIGGER", std::make_shared<KeywordInfo>("TRIGGER", KeywordCategory::DDL, static_cast<int>(DDLKeyword::TRIGGER))},
    {"PRIMARY", std::make_shared<KeywordInfo>("PRIMARY", KeywordCategory::DDL, static_cast<int>(DDLKeyword::PRIMARY))},
    {"FOREIGN", std::make_shared<KeywordInfo>("FOREIGN", KeywordCategory::DDL, static_cast<int>(DDLKeyword::FOREIGN))},
    {"REFERENCES", std::make_shared<KeywordInfo>("REFERENCES", KeywordCategory::DDL, static_cast<int>(DDLKeyword::REFERENCES))},
    {"UNIQUE", std::make_shared<KeywordInfo>("UNIQUE", KeywordCategory::DDL, static_cast<int>(DDLKeyword::UNIQUE))},
    {"CHECK", std::make_shared<KeywordInfo>("CHECK", KeywordCategory::DDL, static_cast<int>(DDLKeyword::CHECK))},
    {"PARTITION", std::make_shared<KeywordInfo>("PARTITION", KeywordCategory::DDL, static_cast<int>(DDLKeyword::PARTITION))},
    {"COLUMN", std::make_shared<KeywordInfo>("COLUMN", KeywordCategory::DDL, static_cast<int>(DDLKeyword::COLUMN))},
    {"DATABASE", std::make_shared<KeywordInfo>("DATABASE", KeywordCategory::DDL, static_cast<int>(DDLKeyword::DATABASE))},
    {"SCHEMA", std::make_shared<KeywordInfo>("SCHEMA", KeywordCategory::DDL, static_cast<int>(DDLKeyword::SCHEMA))},
    {"TYPE", std::make_shared<KeywordInfo>("TYPE", KeywordCategory::DDL, static_cast<int>(DDLKeyword::TYPE))}
};

/**
 * @brief Map for clause keywords
 */
const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> Lexer::clauseMap = {
    {"FROM", std::make_shared<KeywordInfo>("FROM", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::FROM))},
    {"WHERE", std::make_shared<KeywordInfo>("WHERE", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::WHERE))},
    {"GROUP", std::make_shared<KeywordInfo>("GROUP", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::GROUP))},
    {"BY", std::make_shared<KeywordInfo>("BY", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::BY))},
    {"HAVING", std::make_shared<KeywordInfo>("HAVING", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::HAVING))},
    {"ORDER", std::make_shared<KeywordInfo>("ORDER", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::ORDER))},
    {"DISTINCT", std::make_shared<KeywordInfo>("DISTINCT", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::DISTINCT))},
    {"JOIN", std::make_shared<KeywordInfo>("JOIN", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::JOIN))},
    {"INNER", std::make_shared<KeywordInfo>("INNER", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::INNER))},
    {"OUTER", std::make_shared<KeywordInfo>("OUTER", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::OUTER))},
    {"LEFT", std::make_shared<KeywordInfo>("LEFT", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::LEFT))},
    {"RIGHT", std::make_shared<KeywordInfo>("RIGHT", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::RIGHT))},
    {"FULL", std::make_shared<KeywordInfo>("FULL", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::FULL))},
    {"CROSS", std::make_shared<KeywordInfo>("CROSS", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::CROSS))},
    {"ON", std::make_shared<KeywordInfo>("ON", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::ON))},
    {"USING", std::make_shared<KeywordInfo>("USING", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::USING))},
    {"TOP", std::make_shared<KeywordInfo>("TOP", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::TOP))},
    {"LIMIT", std::make_shared<KeywordInfo>("LIMIT", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::LIMIT))},
    {"OFFSET", std::make_shared<KeywordInfo>("OFFSET", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::OFFSET))},
    {"WINDOW", std::make_shared<KeywordInfo>("WINDOW", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::WINDOW))},
    {"PARTITION", std::make_shared<KeywordInfo>("PARTITION", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::PARTITION))},
    {"OVER", std::make_shared<KeywordInfo>("OVER", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::OVER))},
    {"AS", std::make_shared<KeywordInfo>("AS", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::AS))},
    {"DO", std::make_shared<KeywordInfo>("DO", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::DO))},
    {"END", std::make_shared<KeywordInfo>("END", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::END))}
};

/**
 * @brief Map for Common Table Expression keywords
 */
const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> Lexer::cteMap = {
    {"WITH", std::make_shared<KeywordInfo>("WITH", KeywordCategory::CTE, static_cast<int>(CTEKeyword::WITH))},
    {"RECURSIVE", std::make_shared<KeywordInfo>("RECURSIVE", KeywordCategory::CTE, static_cast<int>(CTEKeyword::RECURSIVE))}
};

/**
 * @brief Map for set operation keywords
 */
const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> Lexer::setOpMap = {
    {"UNION", std::make_shared<KeywordInfo>("UNION", KeywordCategory::SETOP, static_cast<int>(SetOpKeyword::UNION))},
    {"INTERSECT", std::make_shared<KeywordInfo>("INTERSECT", KeywordCategory::SETOP, static_cast<int>(SetOpKeyword::INTERSECT))},
    {"EXCEPT", std::make_shared<KeywordInfo>("EXCEPT", KeywordCategory::SETOP, static_cast<int>(SetOpKeyword::EXCEPT))}
};

/**
 * @brief Map for predicate keywords
 */
const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> Lexer::predicateMap = {
    {"IN", std::make_shared<KeywordInfo>("IN", KeywordCategory::PREDICATE, static_cast<int>(PredicateKeyword::IN))},
    {"IS", std::make_shared<KeywordInfo>("IS", KeywordCategory::PREDICATE, static_cast<int>(PredicateKeyword::IS))},
    {"LIKE", std::make_shared<KeywordInfo>("LIKE", KeywordCategory::PREDICATE, static_cast<int>(PredicateKeyword::LIKE))},
    {"BETWEEN", std::make_shared<KeywordInfo>("BETWEEN", KeywordCategory::PREDICATE, static_cast<int>(PredicateKeyword::BETWEEN))},
    {"EXISTS", std::make_shared<KeywordInfo>("EXISTS", KeywordCategory::PREDICATE, static_cast<int>(PredicateKeyword::EXISTS))},
    {"ALL", std::make_shared<KeywordInfo>("ALL", KeywordCategory::PREDICATE, static_cast<int>(PredicateKeyword::ALL))},
    {"ANY", std::make_shared<KeywordInfo>("ANY", KeywordCategory::PREDICATE, static_cast<int>(PredicateKeyword::ANY))},
    {"SOME", std::make_shared<KeywordInfo>("SOME", KeywordCategory::PREDICATE, static_cast<int>(PredicateKeyword::SOME))},
    {"NOT", std::make_shared<KeywordInfo>("NOT", KeywordCategory::PREDICATE, static_cast<int>(PredicateKeyword::NOT))},
    {"UNIQUE", std::make_shared<KeywordInfo>("UNIQUE", KeywordCategory::PREDICATE, static_cast<int>(PredicateKeyword::UNIQUE))}
};

/**
 * @brief Map for logical constant keywords
 */
const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> Lexer::logicalConstMap = {
    {"NULL", std::make_shared<KeywordInfo>("NULL", KeywordCategory::LOGICAL_CONST, static_cast<int>(LogicalConstantKeyword::NULL_KEYWORD))},
    {"TRUE", std::make_shared<KeywordInfo>("TRUE", KeywordCategory::LOGICAL_CONST, static_cast<int>(LogicalConstantKeyword::TRUE_KEYWORD))},
    {"FALSE", std::make_shared<KeywordInfo>("FALSE", KeywordCategory::LOGICAL_CONST, static_cast<int>(LogicalConstantKeyword::FALSE_KEYWORD))}
};

/**
 * @brief Map for transaction keywords
 */
const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> Lexer::tranMap = {
    {"BEGIN", std::make_shared<KeywordInfo>("BEGIN", KeywordCategory::TRANSACTION, static_cast<int>(TransactionKeyword::BEGIN))},
    {"COMMIT", std::make_shared<KeywordInfo>("COMMIT", KeywordCategory::TRANSACTION, static_cast<int>(TransactionKeyword::COMMIT))},
    {"ROLLBACK", std::make_shared<KeywordInfo>("ROLLBACK", KeywordCategory::TRANSACTION, static_cast<int>(TransactionKeyword::ROLLBACK))},
    {"SAVEPOINT", std::make_shared<KeywordInfo>("SAVEPOINT", KeywordCategory::TRANSACTION, static_cast<int>(TransactionKeyword::SAVEPOINT))},
    {"RELEASE", std::make_shared<KeywordInfo>("RELEASE", KeywordCategory::TRANSACTION, static_cast<int>(TransactionKeyword::RELEASE))},
    {"CHAIN", std::make_shared<KeywordInfo>("CHAIN", KeywordCategory::TRANSACTION, static_cast<int>(TransactionKeyword::CHAIN))}
};

/**
 * @brief Map for security-related keywords
 */
const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> Lexer::secMap = {
    {"GRANT", std::make_shared<KeywordInfo>("GRANT", KeywordCategory::SECURITY, static_cast<int>(SecurityKeyword::GRANT))},
    {"REVOKE", std::make_shared<KeywordInfo>("REVOKE", KeywordCategory::SECURITY, static_cast<int>(SecurityKeyword::REVOKE))},
    {"DENY", std::make_shared<KeywordInfo>("DENY", KeywordCategory::SECURITY, static_cast<int>(SecurityKeyword::DENY))},
    {"ON", std::make_shared<KeywordInfo>("ON", KeywordCategory::SECURITY, static_cast<int>(SecurityKeyword::ON))},
    {"TO", std::make_shared<KeywordInfo>("TO", KeywordCategory::SECURITY, static_cast<int>(SecurityKeyword::TO))}
};

/**
 * @brief Map for programming statement keywords
 */
const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> Lexer::progMap = {
    {"DECLARE", std::make_shared<KeywordInfo>("DECLARE", KeywordCategory::PROG_STMT, static_cast<int>(ProgStmtKeyword::DECLARE))},
    {"SET", std::make_shared<KeywordInfo>("SET", KeywordCategory::PROG_STMT, static_cast<int>(ProgStmtKeyword::SET))},
    {"PRINT", std::make_shared<KeywordInfo>("PRINT", KeywordCategory::PROG_STMT, static_cast<int>(ProgStmtKeyword::PRINT))},
    {"RETURN", std::make_shared<KeywordInfo>("RETURN", KeywordCategory::PROG_STMT, static_cast<int>(ProgStmtKeyword::RETURN))},
    {"THROW", std::make_shared<KeywordInfo>("THROW", KeywordCategory::PROG_STMT, static_cast<int>(ProgStmtKeyword::THROW))},
    {"TRY", std::make_shared<KeywordInfo>("TRY", KeywordCategory::PROG_STMT, static_cast<int>(ProgStmtKeyword::TRY))},
    {"CATCH", std::make_shared<KeywordInfo>("CATCH", KeywordCategory::PROG_STMT, static_cast<int>(ProgStmtKeyword::CATCH))},
    {"IF", std::make_shared<KeywordInfo>("IF", KeywordCategory::PROG_STMT, static_cast<int>(ProgStmtKeyword::IF))},
    {"ELSE", std::make_shared<KeywordInfo>("ELSE", KeywordCategory::PROG_STMT, static_cast<int>(ProgStmtKeyword::ELSE))},
    {"LOOP", std::make_shared<KeywordInfo>("LOOP", KeywordCategory::PROG_STMT, static_cast<int>(ProgStmtKeyword::LOOP))},
    {"WHILE", std::make_shared<KeywordInfo>("WHILE", KeywordCategory::PROG_STMT, static_cast<int>(ProgStmtKeyword::WHILE))},
    {"FOR", std::make_shared<KeywordInfo>("FOR", KeywordCategory::PROG_STMT, static_cast<int>(ProgStmtKeyword::FOR))},
    {"BREAK", std::make_shared<KeywordInfo>("BREAK", KeywordCategory::PROG_STMT, static_cast<int>(ProgStmtKeyword::BREAK))},
    {"CONTINUE", std::make_shared<KeywordInfo>("CONTINUE", KeywordCategory::PROG_STMT, static_cast<int>(ProgStmtKeyword::CONTINUE))},
    {"EXEC", std::make_shared<KeywordInfo>("EXEC", KeywordCategory::PROG_STMT, static_cast<int>(ProgStmtKeyword::EXEC))},
    {"GO", std::make_shared<KeywordInfo>("GO", KeywordCategory::PROG_STMT, static_cast<int>(ProgStmtKeyword::GO))}
};

/**
 * @brief Map for miscellaneous keywords
 */
const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> Lexer::miscMap = {
    {"CASE", std::make_shared<KeywordInfo>("CASE", KeywordCategory::MISC, static_cast<int>(MiscKeyword::CASE))},
    {"WHEN", std::make_shared<KeywordInfo>("WHEN", KeywordCategory::MISC, static_cast<int>(MiscKeyword::WHEN))},
    {"THEN", std::make_shared<KeywordInfo>("THEN", KeywordCategory::MISC, static_cast<int>(MiscKeyword::THEN))},
    {"ELSE", std::make_shared<KeywordInfo>("ELSE", KeywordCategory::MISC, static_cast<int>(MiscKeyword::ELSE))},
    {"END", std::make_shared<KeywordInfo>("END", KeywordCategory::MISC, static_cast<int>(MiscKeyword::END))},
    {"ASC", std::make_shared<KeywordInfo>("ASC", KeywordCategory::MISC, static_cast<int>(MiscKeyword::ASC))},
    {"DESC", std::make_shared<KeywordInfo>("DESC", KeywordCategory::MISC, static_cast<int>(MiscKeyword::DESC))},
    {"GENERATED", std::make_shared<KeywordInfo>("GENERATED", KeywordCategory::MISC, static_cast<int>(MiscKeyword::GENERATED))},
    {"AUTOINCREMENT", std::make_shared<KeywordInfo>("AUTOINCREMENT", KeywordCategory::MISC, static_cast<int>(MiscKeyword::AUTOINCREMENT))},
    {"CASCADE", std::make_shared<KeywordInfo>("CASCADE", KeywordCategory::MISC, static_cast<int>(MiscKeyword::CASCADE))},
    {"RESTRICT", std::make_shared<KeywordInfo>("RESTRICT", KeywordCategory::MISC, static_cast<int>(MiscKeyword::RESTRICT))},
    {"DEFERRABLE", std::make_shared<KeywordInfo>("DEFERRABLE", KeywordCategory::MISC, static_cast<int>(MiscKeyword::DEFERRABLE))},
    {"EXPLAIN", std::make_shared<KeywordInfo>("EXPLAIN", KeywordCategory::MISC, static_cast<int>(MiscKeyword::EXPLAIN))},
    {"ANALYZE", std::make_shared<KeywordInfo>("ANALYZE", KeywordCategory::MISC, static_cast<int>(MiscKeyword::ANALYZE))},
    {"VACUUM", std::make_shared<KeywordInfo>("VACUUM", KeywordCategory::MISC, static_cast<int>(MiscKeyword::VACUUM))}
};

/**
 * @brief Map for aggregate functions
 */
const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> Lexer::aggFuncMap = {
    {"COUNT", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, "COUNT", "BIGINT", std::vector<std::string>{"ANY"}, 1, 1, true)},
    {"SUM", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, "SUM", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1, true)},
    {"AVG", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, "AVG", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1, true)},
    {"MIN", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, "MIN", "ANY", std::vector<std::string>{"ANY"}, 1, 1, true)},
    {"MAX", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, "MAX", "ANY", std::vector<std::string>{"ANY"}, 1, 1, true)},
    {"GROUP_CONCAT", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, "GROUP_CONCAT", "STRING", std::vector<std::string>{"ANY"}, 1, -1, true)},
    {"ARRAY_AGG", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, "ARRAY_AGG", "ARRAY", std::vector<std::string>{"ANY"}, 1, 1, true)},
    {"LISTAGG", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, "LISTAGG", "STRING", std::vector<std::string>{"ANY", "STRING"}, 1, 2, true)},
    {"STDDEV", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, "STDDEV", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1, true)},
    {"VARIANCE", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, "VARIANCE", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1, true)}
};

/**
 * @brief Map for scalar functions
 */
const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> Lexer::scalarFuncMap = {
    {"CONVERT", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, "CONVERT", "ANY", std::vector<std::string>{"ANY", "STRING"}, 2, 3)},
    {"CAST", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, "CAST", "ANY", std::vector<std::string>{"ANY", "STRING"}, 2, 2)},
    {"COALESCE", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, "COALESCE", "ANY", std::vector<std::string>{"ANY"}, 2, -1)},
    {"NULLIF", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, "NULLIF", "ANY", std::vector<std::string>{"ANY", "ANY"}, 2, 2)},
    {"IFNULL", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, "IFNULL", "ANY", std::vector<std::string>{"ANY", "ANY"}, 2, 2)},
    {"LEAST", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, "LEAST", "ANY", std::vector<std::string>{"ANY"}, 2, -1)},
    {"GREATEST", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, "GREATEST", "ANY", std::vector<std::string>{"ANY"}, 2, -1)},
    {"FORMAT", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, "FORMAT", "STRING", std::vector<std::string>{"ANY", "STRING"}, 2, 2)},
    {"LENGTH", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, "LENGTH", "INTEGER", std::vector<std::string>{"STRING"}, 1, 1)},
    {"POSITION", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, "POSITION", "INTEGER", std::vector<std::string>{"STRING", "STRING"}, 2, 2)},
    {"ABS", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, "ABS", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1)},
    {"ROUND", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, "ROUND", "NUMERIC", std::vector<std::string>{"NUMERIC", "INTEGER"}, 1, 2)},
    {"FLOOR", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, "FLOOR", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1)},
    {"CEILING", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, "CEILING", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1)},
    {"DATE_TRUNC", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, "DATE_TRUNC", "TIMESTAMP", std::vector<std::string>{"STRING", "TIMESTAMP"}, 2, 2)},
    {"DATE_ADD", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, "DATE_ADD", "TIMESTAMP", std::vector<std::string>{"TIMESTAMP", "INTERVAL"}, 2, 2)},
    {"DATE_SUB", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, "DATE_SUB", "TIMESTAMP", std::vector<std::string>{"TIMESTAMP", "INTERVAL"}, 2, 2)},
    {"EXTRACT", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, "EXTRACT", "INTEGER", std::vector<std::string>{"STRING", "TIMESTAMP"}, 2, 2)}
};

/**
 * @brief Map for string functions
 */
const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> Lexer::stringFuncMap = {
    {"UPPER", std::make_shared<FunctionInfo>(FunctionCategory::STRING, "UPPER", "STRING", std::vector<std::string>{"STRING"}, 1, 1)},
    {"LOWER", std::make_shared<FunctionInfo>(FunctionCategory::STRING, "LOWER", "STRING", std::vector<std::string>{"STRING"}, 1, 1)},
    {"SUBSTRING", std::make_shared<FunctionInfo>(FunctionCategory::STRING, "SUBSTRING", "STRING", std::vector<std::string>{"STRING", "INTEGER", "INTEGER"}, 2, 3)},
    {"TRIM", std::make_shared<FunctionInfo>(FunctionCategory::STRING, "TRIM", "STRING", std::vector<std::string>{"STRING"}, 1, 1)},
    {"LTRIM", std::make_shared<FunctionInfo>(FunctionCategory::STRING, "LTRIM", "STRING", std::vector<std::string>{"STRING"}, 1, 1)},
    {"RTRIM", std::make_shared<FunctionInfo>(FunctionCategory::STRING, "RTRIM", "STRING", std::vector<std::string>{"STRING"}, 1, 1)},
    {"CONCAT", std::make_shared<FunctionInfo>(FunctionCategory::STRING, "CONCAT", "STRING", std::vector<std::string>{"STRING"}, 2, -1)},
    {"REPLACE", std::make_shared<FunctionInfo>(FunctionCategory::STRING, "REPLACE", "STRING", std::vector<std::string>{"STRING", "STRING", "STRING"}, 3, 3)},
    {"SPLIT_PART", std::make_shared<FunctionInfo>(FunctionCategory::STRING, "SPLIT_PART", "STRING", std::vector<std::string>{"STRING", "STRING", "INTEGER"}, 3, 3)},
    {"LEFT", std::make_shared<FunctionInfo>(FunctionCategory::STRING, "LEFT", "STRING", std::vector<std::string>{"STRING", "INTEGER"}, 2, 2)},
    {"RIGHT", std::make_shared<FunctionInfo>(FunctionCategory::STRING, "RIGHT", "STRING", std::vector<std::string>{"STRING", "INTEGER"}, 2, 2)},
    {"REPEAT", std::make_shared<FunctionInfo>(FunctionCategory::STRING, "REPEAT", "STRING", std::vector<std::string>{"STRING", "INTEGER"}, 2, 2)},
    {"REVERSE", std::make_shared<FunctionInfo>(FunctionCategory::STRING, "REVERSE", "STRING", std::vector<std::string>{"STRING"}, 1, 1)},
    {"CHAR_LENGTH", std::make_shared<FunctionInfo>(FunctionCategory::STRING, "CHAR_LENGTH", "INTEGER", std::vector<std::string>{"STRING"}, 1, 1)},
    {"CHARACTER_LENGTH", std::make_shared<FunctionInfo>(FunctionCategory::STRING, "CHARACTER_LENGTH", "INTEGER", std::vector<std::string>{"STRING"}, 1, 1)},
    {"POSITION", std::make_shared<FunctionInfo>(FunctionCategory::STRING, "POSITION", "INTEGER", std::vector<std::string>{"STRING", "STRING"}, 2, 2)}
};

/**
 * @brief Map for date/time functions
 */
const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> Lexer::dtFuncMap = {
    {"DATEPART", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, "DATEPART", "INTEGER", std::vector<std::string>{"STRING", "TIMESTAMP"}, 2, 2)},
    {"GETDATE", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, "GETDATE", "TIMESTAMP", std::vector<std::string>{}, 0, 0)},
    {"NOW", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, "NOW", "TIMESTAMP", std::vector<std::string>{}, 0, 0)},
    {"CURRENT_DATE", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, "CURRENT_DATE", "DATE", std::vector<std::string>{}, 0, 0)},
    {"CURRENT_TIME", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, "CURRENT_TIME", "TIME", std::vector<std::string>{}, 0, 0)},
    {"CURRENT_TIMESTAMP", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, "CURRENT_TIMESTAMP", "TIMESTAMP", std::vector<std::string>{}, 0, 0)},
    {"LOCALTIME", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, "LOCALTIME", "TIME", std::vector<std::string>{}, 0, 0)},
    {"LOCALTIMESTAMP", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, "LOCALTIMESTAMP", "TIMESTAMP", std::vector<std::string>{}, 0, 0)},
    {"AGE", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, "AGE", "INTERVAL", std::vector<std::string>{"TIMESTAMP", "TIMESTAMP"}, 1, 2)},
    {"TO_DATE", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, "TO_DATE", "DATE", std::vector<std::string>{"STRING", "STRING"}, 1, 2)},
    {"TO_TIMESTAMP", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, "TO_TIMESTAMP", "TIMESTAMP", std::vector<std::string>{"STRING", "STRING"}, 1, 2)}
};

/**
 * @brief Map for mathematical functions
 */
const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> Lexer::mathFuncMap = {
    {"ABS", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "ABS", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1)},
    {"CEILING", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "CEILING", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1)},
    {"FLOOR", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "FLOOR", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1)},
    {"ROUND", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "ROUND", "NUMERIC", std::vector<std::string>{"NUMERIC", "INTEGER"}, 1, 2)},
    {"POWER", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "POWER", "NUMERIC", std::vector<std::string>{"NUMERIC", "NUMERIC"}, 2, 2)},
    {"SQRT", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "SQRT", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1)},
    {"EXP", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "EXP", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1)},
    {"LN", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "LN", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1)},
    {"LOG", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "LOG", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1)},
    {"MOD", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "MOD", "INTEGER", std::vector<std::string>{"INTEGER", "INTEGER"}, 2, 2)},
    {"RANDOM", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "RANDOM", "NUMERIC", std::vector<std::string>{}, 0, 0, false, false, false)},
    {"SIGN", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "SIGN", "INTEGER", std::vector<std::string>{"NUMERIC"}, 1, 1)},
    {"TRUNC", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "TRUNC", "NUMERIC", std::vector<std::string>{"NUMERIC", "INTEGER"}, 1, 2)},
    {"PI", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "PI", "NUMERIC", std::vector<std::string>{}, 0, 0)},
    {"SIN", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "SIN", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1)},
    {"COS", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "COS", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1)},
    {"TAN", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "TAN", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1)},
    {"ASIN", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "ASIN", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1)},
    {"ACOS", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "ACOS", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1)},
    {"ATAN", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "ATAN", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1)},
    {"DEGREES", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "DEGREES", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1)},
    {"RADIANS", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, "RADIANS", "NUMERIC", std::vector<std::string>{"NUMERIC"}, 1, 1)}
};

/**
 * @brief Map for system functions
 */
const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> Lexer::sysFuncMap = {
    {"SUSER_SNAME", std::make_shared<FunctionInfo>(FunctionCategory::SYSTEM, "SUSER_SNAME", "STRING", std::vector<std::string>{}, 0, 0)},
    {"CURRENT_USER", std::make_shared<FunctionInfo>(FunctionCategory::SYSTEM, "CURRENT_USER", "STRING", std::vector<std::string>{}, 0, 0)},
    {"SESSION_USER", std::make_shared<FunctionInfo>(FunctionCategory::SYSTEM, "SESSION_USER", "STRING", std::vector<std::string>{}, 0, 0)},
    {"USER", std::make_shared<FunctionInfo>(FunctionCategory::SYSTEM, "USER", "STRING", std::vector<std::string>{}, 0, 0)},
    {"SYSTEM_USER", std::make_shared<FunctionInfo>(FunctionCategory::SYSTEM, "SYSTEM_USER", "STRING", std::vector<std::string>{}, 0, 0)},
    {"TRANCOUNT", std::make_shared<FunctionInfo>(FunctionCategory::SYSTEM, "TRANCOUNT", "INTEGER", std::vector<std::string>{}, 0, 0)},
    {"VERSION", std::make_shared<FunctionInfo>(FunctionCategory::SYSTEM, "VERSION", "STRING", std::vector<std::string>{}, 0, 0)},
    {"DATABASE", std::make_shared<FunctionInfo>(FunctionCategory::SYSTEM, "DATABASE", "STRING", std::vector<std::string>{}, 0, 0)},
    {"SCHEMA_NAME", std::make_shared<FunctionInfo>(FunctionCategory::SYSTEM, "SCHEMA_NAME", "STRING", std::vector<std::string>{"INTEGER"}, 0, 1)}
};

/**
 * @brief Map for window functions
 */
const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> Lexer::windowFuncMap = {
    {"ROW_NUMBER", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, "ROW_NUMBER", "INTEGER", std::vector<std::string>{}, 0, 0, false, true)},
    {"RANK", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, "RANK", "INTEGER", std::vector<std::string>{}, 0, 0, false, true)},
    {"DENSE_RANK", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, "DENSE_RANK", "INTEGER", std::vector<std::string>{}, 0, 0, false, true)},
    {"NTILE", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, "NTILE", "INTEGER", std::vector<std::string>{"INTEGER"}, 1, 1, false, true)},
    {"LEAD", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, "LEAD", "ANY", std::vector<std::string>{"ANY", "INTEGER", "ANY"}, 1, 3, false, true)},
    {"LAG", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, "LAG", "ANY", std::vector<std::string>{"ANY", "INTEGER", "ANY"}, 1, 3, false, true)},
    {"FIRST_VALUE", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, "FIRST_VALUE", "ANY", std::vector<std::string>{"ANY"}, 1, 1, false, true)},
    {"LAST_VALUE", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, "LAST_VALUE", "ANY", std::vector<std::string>{"ANY"}, 1, 1, false, true)},
    {"NTH_VALUE", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, "NTH_VALUE", "ANY", std::vector<std::string>{"ANY", "INTEGER"}, 2, 2, false, true)},
    {"PERCENT_RANK", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, "PERCENT_RANK", "NUMERIC", std::vector<std::string>{}, 0, 0, false, true)},
    {"CUME_DIST", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, "CUME_DIST", "NUMERIC", std::vector<std::string>{}, 0, 0, false, true)}
};

/**
 * @brief Map for arithmetic operators
 */
const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> Lexer::arithMap = {
    {"+", std::make_shared<OperatorInfo>(OperatorCategory::ARITHMETIC, "+", SQLOperatorPrecedence::ADDITIVE, true, false, "NUMERIC", std::vector<std::string>{"NUMERIC", "NUMERIC"})},
    {"-", std::make_shared<OperatorInfo>(OperatorCategory::ARITHMETIC, "-", SQLOperatorPrecedence::ADDITIVE, true, false, "NUMERIC", std::vector<std::string>{"NUMERIC", "NUMERIC"})},
    {"*", std::make_shared<OperatorInfo>(OperatorCategory::ARITHMETIC, "*", SQLOperatorPrecedence::MULTIPLICATIVE, true, false, "NUMERIC", std::vector<std::string>{"NUMERIC", "NUMERIC"})},
    {"/", std::make_shared<OperatorInfo>(OperatorCategory::ARITHMETIC, "/", SQLOperatorPrecedence::MULTIPLICATIVE, true, false, "NUMERIC", std::vector<std::string>{"NUMERIC", "NUMERIC"})},
    {"%", std::make_shared<OperatorInfo>(OperatorCategory::ARITHMETIC, "%", SQLOperatorPrecedence::MULTIPLICATIVE, true, false, "INTEGER", std::vector<std::string>{"INTEGER", "INTEGER"})}
};

/**
 * @brief Map for assignment operators
 */
const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> Lexer::assignMap = {
    {"=", std::make_shared<OperatorInfo>(OperatorCategory::ASSIGN, "=", SQLOperatorPrecedence::ASSIGNMENT, true, false, "ANY", std::vector<std::string>{"ANY", "ANY"})},
    {":=", std::make_shared<OperatorInfo>(OperatorCategory::ASSIGN, ":=", SQLOperatorPrecedence::ASSIGNMENT, true, false, "ANY", std::vector<std::string>{"ANY", "ANY"})}
};

/**
 * @brief Map for comparison operators
 */
const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> Lexer::compMap = {
    {"<", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, "<", SQLOperatorPrecedence::COMPARISON, true, false, "BOOLEAN", std::vector<std::string>{"ANY", "ANY"})},
    {">", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, ">", SQLOperatorPrecedence::COMPARISON, true, false, "BOOLEAN", std::vector<std::string>{"ANY", "ANY"})},
    {"<=", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, "<=", SQLOperatorPrecedence::COMPARISON, true, false, "BOOLEAN", std::vector<std::string>{"ANY", "ANY"})},
    {">=", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, ">=", SQLOperatorPrecedence::COMPARISON, true, false, "BOOLEAN", std::vector<std::string>{"ANY", "ANY"})},
    {"<>", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, "<>", SQLOperatorPrecedence::COMPARISON, true, false, "BOOLEAN", std::vector<std::string>{"ANY", "ANY"})},
    {"!=", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, "!=", SQLOperatorPrecedence::COMPARISON, true, false, "BOOLEAN", std::vector<std::string>{"ANY", "ANY"})},
    {"=", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, "=", SQLOperatorPrecedence::COMPARISON, true, false, "BOOLEAN", std::vector<std::string>{"ANY", "ANY"})},
    {"LIKE", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, "LIKE", SQLOperatorPrecedence::PATTERN, true, false, "BOOLEAN", std::vector<std::string>{"STRING", "STRING"})},
    {"NOT LIKE", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, "NOT LIKE", SQLOperatorPrecedence::PATTERN, true, false, "BOOLEAN", std::vector<std::string>{"STRING", "STRING"})},
    {"ILIKE", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, "ILIKE", SQLOperatorPrecedence::PATTERN, true, false, "BOOLEAN", std::vector<std::string>{"STRING", "STRING"})},
    {"NOT ILIKE", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, "NOT ILIKE", SQLOperatorPrecedence::PATTERN, true, false, "BOOLEAN", std::vector<std::string>{"STRING", "STRING"})},
    {"SIMILAR TO", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, "SIMILAR TO", SQLOperatorPrecedence::PATTERN, true, false, "BOOLEAN", std::vector<std::string>{"STRING", "STRING"})},
    {"NOT SIMILAR TO", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, "NOT SIMILAR TO", SQLOperatorPrecedence::PATTERN, true, false, "BOOLEAN", std::vector<std::string>{"STRING", "STRING"})},
    {"IS DISTINCT FROM", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, "IS DISTINCT FROM", SQLOperatorPrecedence::COMPARISON, true, false, "BOOLEAN", std::vector<std::string>{"ANY", "ANY"})},
    {"IS NOT DISTINCT FROM", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, "IS NOT DISTINCT FROM", SQLOperatorPrecedence::COMPARISON, true, false, "BOOLEAN", std::vector<std::string>{"ANY", "ANY"})}
};

/**
 * @brief Map for logical operators
 */
const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> Lexer::logMap = {
    {"AND", std::make_shared<OperatorInfo>(OperatorCategory::LOGICAL, "AND", SQLOperatorPrecedence::AND, true, false, "BOOLEAN", std::vector<std::string>{"BOOLEAN", "BOOLEAN"})},
    {"OR", std::make_shared<OperatorInfo>(OperatorCategory::LOGICAL, "OR", SQLOperatorPrecedence::OR, true, false, "BOOLEAN", std::vector<std::string>{"BOOLEAN", "BOOLEAN"})},
    {"NOT", std::make_shared<OperatorInfo>(OperatorCategory::LOGICAL, "NOT", SQLOperatorPrecedence::UNARY, true, true, "BOOLEAN", std::vector<std::string>{"BOOLEAN"})},
    {"XOR", std::make_shared<OperatorInfo>(OperatorCategory::LOGICAL, "XOR", SQLOperatorPrecedence::OR, true, false, "BOOLEAN", std::vector<std::string>{"BOOLEAN", "BOOLEAN"})},
    {"IMPLIES", std::make_shared<OperatorInfo>(OperatorCategory::LOGICAL, "IMPLIES", SQLOperatorPrecedence::OR, true, false, "BOOLEAN", std::vector<std::string>{"BOOLEAN", "BOOLEAN"})}
};

/**
 * @brief Map for bitwise operators
 */
const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> Lexer::bitMap = {
    {"&", std::make_shared<OperatorInfo>(OperatorCategory::BITWISE, "&", SQLOperatorPrecedence::BITWISE_AND, true, false, "INTEGER", std::vector<std::string>{"INTEGER", "INTEGER"})},
    {"|", std::make_shared<OperatorInfo>(OperatorCategory::BITWISE, "|", SQLOperatorPrecedence::BITWISE_OR, true, false, "INTEGER", std::vector<std::string>{"INTEGER", "INTEGER"})},
    {"^", std::make_shared<OperatorInfo>(OperatorCategory::BITWISE, "^", SQLOperatorPrecedence::BITWISE_XOR, true, false, "INTEGER", std::vector<std::string>{"INTEGER", "INTEGER"})},
    {"~", std::make_shared<OperatorInfo>(OperatorCategory::BITWISE, "~", SQLOperatorPrecedence::UNARY, true, true, "INTEGER", std::vector<std::string>{"INTEGER"})},
    {"<<", std::make_shared<OperatorInfo>(OperatorCategory::BITWISE, "<<", SQLOperatorPrecedence::SHIFT, true, false, "INTEGER", std::vector<std::string>{"INTEGER", "INTEGER"})},
    {">>", std::make_shared<OperatorInfo>(OperatorCategory::BITWISE, ">>", SQLOperatorPrecedence::SHIFT, true, false, "INTEGER", std::vector<std::string>{"INTEGER", "INTEGER"})}
};

/**
 * @brief Map for concatenation operators
 */
const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> Lexer::concatMap = {
    {"||", std::make_shared<OperatorInfo>(OperatorCategory::CONCAT, "||", SQLOperatorPrecedence::ADDITIVE, true, false, "STRING", std::vector<std::string>{"STRING", "STRING"})}
};

// Add after the existing operator maps in Lexer\Lexer.cpp

/**
 * @brief Map for JSON operators (PostgreSQL-style)
 */
const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> Lexer::jsonOpMap = {
    {"->", std::make_shared<OperatorInfo>(OperatorCategory::JSON, "->", SQLOperatorPrecedence::MEMBER, true, false, "JSON", std::vector<std::string>{"JSON", "TEXT"})},
    {"->>", std::make_shared<OperatorInfo>(OperatorCategory::JSON, "->>", SQLOperatorPrecedence::MEMBER, true, false, "TEXT", std::vector<std::string>{"JSON", "TEXT"})},
    {"#>", std::make_shared<OperatorInfo>(OperatorCategory::JSON, "#>", SQLOperatorPrecedence::MEMBER, true, false, "JSON", std::vector<std::string>{"JSON", "TEXT[]"})},
    {"#>>", std::make_shared<OperatorInfo>(OperatorCategory::JSON, "#>>", SQLOperatorPrecedence::MEMBER, true, false, "TEXT", std::vector<std::string>{"JSON", "TEXT[]"})},
    {"?", std::make_shared<OperatorInfo>(OperatorCategory::JSON, "?", SQLOperatorPrecedence::COMPARISON, true, false, "BOOLEAN", std::vector<std::string>{"JSON", "TEXT"})},
    {"?|", std::make_shared<OperatorInfo>(OperatorCategory::JSON, "?|", SQLOperatorPrecedence::COMPARISON, true, false, "BOOLEAN", std::vector<std::string>{"JSON", "TEXT[]"})},
    {"?&", std::make_shared<OperatorInfo>(OperatorCategory::JSON, "?&", SQLOperatorPrecedence::COMPARISON, true, false, "BOOLEAN", std::vector<std::string>{"JSON", "TEXT[]"})},
    {"#-", std::make_shared<OperatorInfo>(OperatorCategory::JSON, "#-", SQLOperatorPrecedence::MEMBER, true, false, "JSON", std::vector<std::string>{"JSON", "TEXT"})},
    {"@", std::make_shared<OperatorInfo>(OperatorCategory::JSON, "@", SQLOperatorPrecedence::MEMBER, true, false, "JSON", std::vector<std::string>{"JSON", "ANY"})}
};

/**
 * @brief Map for Regex operators (PostgreSQL-style)
 */
const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> Lexer::regexOpMap = {
    {"~", std::make_shared<OperatorInfo>(OperatorCategory::REGEX, "~", SQLOperatorPrecedence::PATTERN, true, false, "BOOLEAN", std::vector<std::string>{"TEXT", "TEXT"})},
    {"!~", std::make_shared<OperatorInfo>(OperatorCategory::REGEX, "!~", SQLOperatorPrecedence::PATTERN, true, false, "BOOLEAN", std::vector<std::string>{"TEXT", "TEXT"})},
    {"~*", std::make_shared<OperatorInfo>(OperatorCategory::REGEX, "~*", SQLOperatorPrecedence::PATTERN, true, false, "BOOLEAN", std::vector<std::string>{"TEXT", "TEXT"})},
    {"!~*", std::make_shared<OperatorInfo>(OperatorCategory::REGEX, "!~*", SQLOperatorPrecedence::PATTERN, true, false, "BOOLEAN", std::vector<std::string>{"TEXT", "TEXT"})}
};

/**
 * @brief Map for typecast operator (::)
 */
const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> Lexer::typecastOpMap = {
    {"::", std::make_shared<OperatorInfo>(OperatorCategory::TYPECAST, "::", SQLOperatorPrecedence::TYPECAST, true, false, "ANY", std::vector<std::string>{"ANY", "STRING"}, false)}
};

/**
 * @brief Map for punctuation symbols
 */
const std::unordered_map<std::string, std::shared_ptr<PunctuatorInfo>> Lexer::punctuatorMap = {
    {",", std::make_shared<PunctuatorInfo>(CommonSymbol::COMMA, ",")},
    {";", std::make_shared<PunctuatorInfo>(CommonSymbol::SEMICOLON, ";")},
    {"(", std::make_shared<PunctuatorInfo>(CommonSymbol::LPAREN, "(")},
    {")", std::make_shared<PunctuatorInfo>(CommonSymbol::RPAREN, ")")},
    {"{", std::make_shared<PunctuatorInfo>(CommonSymbol::LBRACE, "{")},
    {"}", std::make_shared<PunctuatorInfo>(CommonSymbol::RBRACE, "}")},
    {"[", std::make_shared<PunctuatorInfo>(CommonSymbol::LBRACKET, "[")},
    {"]", std::make_shared<PunctuatorInfo>(CommonSymbol::RBRACKET, "]")},
    {".", std::make_shared<PunctuatorInfo>(CommonSymbol::DOT, ".")},
    {":", std::make_shared<PunctuatorInfo>(CommonSymbol::COLON, ":")},
    {"?", std::make_shared<PunctuatorInfo>(CommonSymbol::PARAM_MARKER, "?")}
};

/**
 * @brief Map for TSQL-specific symbols
 */
const std::unordered_map<std::string, std::shared_ptr<PunctuatorInfo>> Lexer::tsqlSymMap = {
    {".", std::make_shared<PunctuatorInfo>(TSQLSymbol::DOT, ".")},
    {":", std::make_shared<PunctuatorInfo>(TSQLSymbol::COLON, ":")}
};

/**
 * @brief Map for string delimiters
 */
const std::unordered_map<char, std::shared_ptr<PunctuatorInfo>> Lexer::strDelimMap = {
    {'\'', std::make_shared<PunctuatorInfo>(StringDelimiter::SINGLE_QUOTE, "'")},
    {'"', std::make_shared<PunctuatorInfo>(StringDelimiter::DOUBLE_QUOTE, "\"")},
    {'`', std::make_shared<PunctuatorInfo>(StringDelimiter::BACKTICK, "`")}
};

/**
 * @brief Map for date/time parts
 */
const std::unordered_map<std::string, DateTimePart> Lexer::dateTimePartsMap = {
    {"YEAR", DateTimePart::YEAR},
    {"QUARTER", DateTimePart::QUARTER},
    {"MONTH", DateTimePart::MONTH},
    {"DAYOFYEAR", DateTimePart::DAY_OF_YEAR},
    {"DAY", DateTimePart::DAY},
    {"WEEK", DateTimePart::WEEK},
    {"ISOWEEK", DateTimePart::ISO_WEEK},
    {"WEEKDAY", DateTimePart::WEEKDAY},
    {"HOUR", DateTimePart::HOUR},
    {"MINUTE", DateTimePart::MINUTE},
    {"SECOND", DateTimePart::SECOND},
    {"MILLISECOND", DateTimePart::MILLISECOND},
    {"MICROSECOND", DateTimePart::MICROSECOND},
    {"NANOSECOND", DateTimePart::NANOSECOND},
    {"TIMEZONE_OFFSET", DateTimePart::TIMEZONE_OFFSET}
};

/**
 * @brief Map for literal categories
 */
const std::unordered_map<std::string, LiteralCategory> Lexer::literalCategoryMap = {
    {"STRING",      LiteralCategory::STRING},
    {"ESCAPE_STRING", LiteralCategory::ESCAPE_STRING},
    {"CHAR",        LiteralCategory::CHAR},
    {"INTEGER",     LiteralCategory::INTEGER},
    {"FLOAT",       LiteralCategory::FLOAT},
    {"BINARY",      LiteralCategory::BINARY},
    {"HEX",         LiteralCategory::HEX},
    {"DATE",        LiteralCategory::DATE},
    {"TIME",        LiteralCategory::TIME},
    {"DATETIME",    LiteralCategory::DATETIME},
    {"INTERVAL",    LiteralCategory::INTERVAL},
    {"UUID",        LiteralCategory::UUID},
    {"ARRAY",       LiteralCategory::ARRAY},
    {"JSON",        LiteralCategory::JSON},
    {"XML",         LiteralCategory::XML},
    {"BOOLEAN",     LiteralCategory::BOOLEAN},
    {"NULL_VALUE",  LiteralCategory::NULL_VALUE}
};

// === Constructor ===

/**
 * @brief Constructs a new Lexer instance.
 * @param query The SQL query string to tokenize
 * @param config The lexer configuration (optional)
 */
Lexer::Lexer(const std::string& query, const LexerConfig& config)
    : input(query)
    , pos(0)
    , line(1)
    , column(1)
    , tokenIndex(0)
    , config(config)
{
    tokens.clear();
}

// === Core Token Access Methods ===

/**
 * @brief Gets the next token from the input stream.
 * @return Unique pointer to the next Token
 * @throws LexerError if tokenization fails
 */
std::unique_ptr<Token> Lexer::nextToken() {
    // Return cached token if available
    if (tokenIndex < tokens.size()) {
        return std::make_unique<Token>(*tokens[tokenIndex++]);
    }

    try {
        // Skip whitespace and track position
        skipWhitespaceAndNewlines();

        // Check for end of input
        if (pos >= input.size()) {
            auto tok = std::make_unique<Token>(TokenType::END_OF_FILE, "", static_cast<int>(pos));
            tokens.push_back(std::make_unique<Token>(*tok));
            ++tokenIndex;
            return tok;
        }

        // Record start position for error reporting
        size_t startPos = pos;
        size_t startLine = line;
        size_t startColumn = column;

        // Get next token based on current character
        std::unique_ptr<Token> tok = lexNextToken();

        // Validate token
        if (!tok) {
            throwError("Failed to create token", input.substr(startPos, 1));
        }

        // Set token position
        tok->setPosition(static_cast<int>(startPos));

        // Cache token
        tokens.push_back(std::make_unique<Token>(*tok));
        ++tokenIndex;

        return tok;
    }
    catch (const std::exception& e) {
        // Convert any internal exceptions to LexerError with position info
        throwError(e.what());
    }
}

/**
 * @brief Peeks at the next token without consuming it.
 * @return Unique pointer to the next token
 * @throws LexerError if tokenization fails
 */
std::unique_ptr<Token> Lexer::peekToken() {
    // Save current state
    size_t savedPos = pos;
    size_t savedLine = line;
    size_t savedColumn = column;
    size_t savedIndex = tokenIndex;

    // Get next token
    auto tok = nextToken();

    // Restore state
    pos = savedPos;
    line = savedLine;
    column = savedColumn;
    tokenIndex = savedIndex;

    return tok;
}

/**
 * @brief Jumps to a specific token index in the stream.
 * @param offset The token index to jump to
 * @return Unique pointer to the token at the specified index
 * @throws LexerError if offset is invalid or tokenization fails
 */
std::unique_ptr<Token> Lexer::jumpToken(int offset) {
    if (offset < 0) {
        throwError("Cannot jump to negative token index");
    }

    // Generate tokens up to the requested offset
    while (tokens.size() <= static_cast<size_t>(offset)) {
        if (pos >= input.size()) {
            throwError("Offset exceeds input length");
        }
        nextToken();
    }

    // Set current position to requested token
    tokenIndex = offset;
    return std::make_unique<Token>(*tokens[tokenIndex]);
}

/**
 * @brief Attempts to identify and lex the next token from current position.
 * @return Unique pointer to the identified token
 * @throws LexerError if tokenization fails
 */
std::unique_ptr<Token> Lexer::lexNextToken() {
    char c = input[pos];

    // 1. Comments
    if ((pos + 1 < input.size() && input[pos] == '-' && input[pos + 1] == '-') ||
        (pos + 1 < input.size() && input[pos] == '/' && input[pos + 1] == '*')) {
        auto tok = lexComment();
        if (tok) return tok;
    }

    // 2. Dollar-quoted strings (PostgreSQL-style)
    if (isDollarQuoteStart()) {
        auto tok = lexDollarQuotedString();
        if (tok) return tok;
    }

    // 3. Escape string literals (E'...')
    if (c == 'E' && pos + 1 < input.size() && input[pos + 1] == '\'') {
        auto tok = lexEscapeString();
        if (tok) return tok;
    }

    // 4. Binary string literals (B'...')
    if ((c == 'B' || c == 'b') && pos + 1 < input.size() && input[pos + 1] == '\'') {
        auto tok = lexBinary();
        if (tok) return tok;
    }

    // 5. Hex string literals (X'...')
    if ((c == 'X' || c == 'x') && pos + 1 < input.size() && input[pos + 1] == '\'') {
        auto tok = lexHex();
        if (tok) return tok;
    }

    // 6. C-style hex literal: 0xDEADBEEF
    if (c == '0' && pos + 1 < input.size() && (input[pos + 1] == 'x' || input[pos + 1] == 'X')) {
        auto tok = lexCStyleHex();
        if (tok) return tok;
    }

    // 7. C-style binary literal: 0b1010
    if (c == '0' && pos + 1 < input.size() && (input[pos + 1] == 'b' || input[pos + 1] == 'B')) {
        auto tok = lexCStyleBinary();
        if (tok) return tok;
    }

    // 8. ARRAY[ ... ] literal support
    if ((c == 'A' || c == 'a') && pos + 6 < input.size()) {
        std::string maybeArray = input.substr(pos, 5);
        if ((maybeArray == "ARRAY" || maybeArray == "array") && input[pos + 5] == '[') {
            pos += 5; // Skip "ARRAY"
            auto tok = lexArray();
            if (tok) return tok;
        }
    }

    // 9. JSON literals
    if (c == '{') {
        auto tok = lexJSON();
        if (tok) return tok;
    }

    // 10. XML literals
    if (c == '<' && pos + 1 < input.size() &&
        (std::isalpha(input[pos + 1]) || input[pos + 1] == '?' || input[pos + 1] == '!')) {
        auto tok = lexXML();
        if (tok) return tok;
    }

    // 11. UUID literals (look for quoted UUID pattern)
    if (pos + 36 <= input.size() && isUUIDLiteral(input.substr(pos, 36))) {
        auto tok = lexUUID();
        if (tok) return tok;
    }

    // 12. Quoted identifiers
    if (c == '[' || c == '"') {
        auto tok = lexDelimitedIdentifier();
        if (tok) return tok;
    }

    // 13. String/date/time literals (all start with a single quote)
    if (c == '\'') {
        std::string lit = peekQuotedLiteralContent(pos); // does not include quotes
        if (!lit.empty()) {
            if (validateDateTime(lit)) {
                auto dtTok = lexDateTime();
                if (dtTok) return dtTok;
            }
            if (validateDate(lit)) {
                auto dTok = lexDate();
                if (dTok) return dTok;
            }
            if (validateTime(lit)) {
                auto tTok = lexTime();
                if (tTok) return tTok;
            }
        }
        auto sTok = lexString();
        if (sTok) return sTok;
    }

    // 14. Character literals (C'...')
    if (c == 'C' && pos + 1 < input.size() && input[pos + 1] == '\'') {
        auto tok = lexCharacter();
        if (tok) return tok;
    }

    // 15. Numbers and negative numbers
    if (std::isdigit(static_cast<unsigned char>(c)) ||
        (c == '-' && pos + 1 < input.size() && std::isdigit(static_cast<unsigned char>(input[pos + 1])))) {
        auto tok = lexNumber();
        if (tok) return tok;
    }

    // 16. Word-like tokens and literal categories
    if (isWordStart(c)) {
        // 16.1 SQL literal categories (INTEGER, STRING, JSON, etc.)
        auto litCatTok = lexLiteralCategory();
        if (litCatTok) return litCatTok;

        // 16.2 NULL literal
        if (isNullLiteral(input.substr(pos, 4))) {
            auto tok = lexNull();
            if (tok) return tok;
        }

        // 16.3 Boolean literals
        if ((input.size() - pos >= 4 &&
            (input.substr(pos, 4) == "TRUE" || input.substr(pos, 4) == "true") &&
            !isIdentifierPart(input[pos + 4])) ||
            (input.size() - pos >= 5 &&
                (input.substr(pos, 5) == "FALSE" || input.substr(pos, 5) == "false") &&
                !isIdentifierPart(input[pos + 5]))) {
            auto tok = lexBoolean();
            if (tok) return tok;
        }

        // 16.4 INTERVAL keyword
        if (pos + 8 <= input.size() && input.substr(pos, 8) == "INTERVAL") {
            auto tok = lexInterval();
            if (tok) return tok;
        }

        // 16.5 Functions
        auto funcTok = lexFunction();
        if (funcTok) return funcTok;

        // 16.6 Word operators
        auto wordOpTok = lexOperator(true);
        if (wordOpTok) return wordOpTok;

        // 16.7 Keywords
        auto keyTok = lexKeyword();
        if (keyTok) return keyTok;

        // 16.8 DateTime parts
        auto dtPartTok = lexDateTimePart();
        if (dtPartTok) return dtPartTok;
    }

    // 17. Default: identifiers
    bool canIdentifier = true;
    // For '@' or '#', ensure next char is part of identifier or same prefix for @@/##
    if ((c == '@' || c == '#')) {
        if (!(pos + 1 < input.size() &&
            (isIdentifierPart(input[pos + 1]) || input[pos + 1] == c))) {
            canIdentifier = false;
        }
    }
    if (canIdentifier && isIdentifierStart(c)) {
        auto idTok = lexIdentifier();
        if (idTok) return idTok;
    }

    // 19. Symbolic Operators (+, !=, ||, etc.)
    auto symOpTok = lexOperator(false);
    if (symOpTok) return symOpTok;

    // 20. Punctuators
    auto punctTok = lexPunctuator();
    if (punctTok) return punctTok;

    // Unknown character
    throwError("Invalid character encountered", std::string(1, c));
    return nullptr; // Never reached due to throwError
}

/**
 * @brief Gets the current token index.
 * @return Current token index
 */
size_t Lexer::getCurrentTokenIndex() const {
    return tokenIndex;
}

/**
 * @brief Gets the total number of tokens processed so far.
 * @return Number of tokens in cache
 */
size_t Lexer::getTokenCount() const {
    return tokens.size();
}

/**
 * @brief Resets the lexer to start tokenizing from the beginning.
 */
void Lexer::reset() {
    pos = 0;
    line = 1;
    column = 1;
    tokenIndex = 0;
    tokens.clear();
    errorContext.clear();
}

// === Position & Whitespace Handling ===

/**
 * @brief Advances the position by a specified count.
 * @param count Number of positions to advance (default: 1)
 * @details Updates both position and column count.
 */
void Lexer::advance(size_t count) {
    for (size_t i = 0; i < count && pos < input.size(); ++i) {
        if (input[pos] == '\n') {
            newline();
        }
        else {
            ++column;
        }
        ++pos;
    }
}

/**
 * @brief Handles a newline character.
 * @details Increments line count and resets column count.
 */
void Lexer::newline() {
    ++line;
    column = 1;
}

/**
 * @brief Skips all whitespace and newline characters.
 * @details Handles both Windows (\r\n) and Unix (\n) line endings.
 */
void Lexer::skipWhitespaceAndNewlines() {
    while (pos < input.size()) {
        char c = input[pos];

        // Handle different types of whitespace
        if (c == ' ' || c == '\t') {
            ++column;
            ++pos;
            continue;
        }

        // Handle different line endings
        if (c == '\n') {
            newline();
            ++pos;
            continue;
        }

        if (c == '\r') {
            // Check for Windows-style line ending (\r\n)
            if (pos + 1 < input.size() && input[pos + 1] == '\n') {
                ++pos; // Skip \r
            }
            newline();
            ++pos;
            continue;
        }

        // Not a whitespace character
        break;
    }
}

/**
 * @brief Checks if current position is at end of input.
 * @return true if at end of input
 */
bool Lexer::isAtEnd() const {
    return pos >= input.size();
}

/**
 * @brief Checks if character at current position matches expected.
 * @param expected The expected character
 * @return true if character matches
 */
bool Lexer::match(char expected) {
    if (isAtEnd() || input[pos] != expected) {
        return false;
    }
    advance();
    return true;
}

/**
 * @brief Looks ahead at the next character without advancing position.
 * @return Next character or '\0' if at end
 */
char Lexer::peek() const {
    return isAtEnd() ? '\0' : input[pos];
}

/**
 * @brief Looks ahead at the character after next without advancing position.
 * @return Character after next or '\0' if at end
 */
char Lexer::peekNext() const {
    return (pos + 1 >= input.size()) ? '\0' : input[pos + 1];
}

/**
 * @brief Peek the quoted content without advancing pos.
 * @return empty string if unterminated
 */
std::string Lexer::peekQuotedLiteralContent(size_t start) const {
    if (input[start] != '\'') return "";
    size_t end = start + 1;
    std::string content;
    while (end < input.size()) {
        if (input[end] == '\'') break; // First closing quote
        content += input[end++];
    }
    if (end >= input.size() || input[end] != '\'') return ""; // Unterminated
    return content;
}

/**
 * @brief Saves current position state.
 * @return Struct containing current position information
 */
Lexer::Position Lexer::savePosition() const {
    return { pos, line, column };
}

/**
 * @brief Restores position state.
 * @param position Previously saved position state
 */
void Lexer::restorePosition(const Position& position) {
    pos = position.pos;
    line = position.line;
    column = position.column;
}

// === Core Lexing Methods ===

/**
 * @brief Lex a SQL keyword from the input.
 * @return Unique pointer to KeywordToken, or nullptr if not a keyword.
 */
std::unique_ptr<Token> Lexer::lexKeyword() {
    size_t start = pos;

    // 1. Collect identifier characters (letters, digits, underscores)
    while (pos < input.size() && isIdentifierPart(input[pos])) {
        advance();
    }

    // 2. Extract the word and normalize to uppercase
    std::string word = input.substr(start, pos - start);
    if (word.empty()) {
        pos = start;
        return nullptr;
    }
    std::string upperWord = word;
    std::transform(upperWord.begin(), upperWord.end(), upperWord.begin(), ::toupper);

    // 3. Check for special literals (TRUE, FALSE, NULL) and return nullptr if found
    if (upperWord == "TRUE" || upperWord == "FALSE" || upperWord == "NULL") {
        pos = start;
        return nullptr;
    }

    // 4. Check keyword maps in order of precedence
    auto keyword = createKeywordToken(upperWord);
    if (keyword) {
        return keyword;
    }

    // 5. Not a keyword, reset position and return nullptr
    pos = start;
    return nullptr;
}

/**
 * @brief Lex an identifier
 * @return Unique pointer to IdentifierToken
 */
std::unique_ptr<Token> Lexer::lexIdentifier() {
    size_t start = pos;
    IdentifierCategory category = IdentifierCategory::UNKNOWN;

    // 1. Handle special identifier prefixes (@@, ##)
    if (match('@')) {
        if (match('@')) {
            category = IdentifierCategory::SYSTEM_VARIABLE;
        }
        else {
            category = IdentifierCategory::USER_VARIABLE;
        }
    }
    else if (match('#')) {
        if (match('#')) {
            category = IdentifierCategory::GLOBAL_TEMP_TABLE;
        }
        else {
            category = IdentifierCategory::TEMP_TABLE;
        }
    }

    // 2. Must start with valid character after any prefixes
    if (pos == start && !isIdentifierStart(peek())) {
        throwError("Invalid identifier start");
    }

    // 3. Collect identifier characters
    while (pos < input.size() && isIdentifierPart(peek())) {
        advance();
    }

    // 4. Extract identifier
    std::string identifier = input.substr(start, pos - start);

    // 5. Validate format and length
    if (!validateIdentifier(identifier)) {
        throwError("Invalid identifier", identifier);
    }
    if (identifier.length() > config.maxIdentLength) {
        throwError("Identifier exceeds maximum length", identifier);
    }

    // 6. Create and return token
    return createIdentifierToken(identifier, category);
}

/**
 * @brief Lex a delimited identifier (quoted or bracketed), with full SQL support including spaces and special prefixes
 * @return Unique pointer to IdentifierToken
 */
std::unique_ptr<Token> Lexer::lexDelimitedIdentifier() {
    size_t start = pos;
    char delim = peek();

    // 1. Check for valid delimiter ([ or ")
    if (delim != '[' && delim != '"') {
        return nullptr;
    }
    advance(); // Skip opening delimiter

    // 2. Handle special identifier prefixes (@, @@, #, ##)
    IdentifierCategory category = IdentifierCategory::UNKNOWN;
    if (match('@')) {
        if (match('@')) {
            category = IdentifierCategory::SYSTEM_VARIABLE;
        }
        else {
            category = IdentifierCategory::USER_VARIABLE;
        }
    }
    else if (match('#')) {
        if (match('#')) {
            category = IdentifierCategory::GLOBAL_TEMP_TABLE;
        }
        else {
            category = IdentifierCategory::TEMP_TABLE;
        }
    }

    std::string identifier;
    bool closed = false;

    // 3. Parse until matching closing delimiter, allowing escapes
    while (pos < input.size()) {
        char c = peek();
        if (delim == '[' && c == ']') {
            if (peekNext() == ']') {
                identifier.push_back(']');
                advance(2); // Skip "]]"
                continue;
            }
            else {
                closed = true;
                advance(); // Skip closing ]
                break;
            }
        }
        if (delim == '"' && c == '"') {
            if (peekNext() == '"') {
                identifier.push_back('"');
                advance(2); // Skip ""
                continue;
            }
            else {
                closed = true;
                advance(); // Skip closing "
                break;
            }
        }
        if (c == '\n' || c == '\r' || isAtEnd()) {
            throwError("Unterminated delimited identifier");
        }
        // Allow spaces and SQL identifier characters
        identifier.push_back(c);
        advance();
    }

    // 4. Validate closing and length
    if (!closed) {
        throwError("Unterminated delimited identifier");
    }
    if (identifier.empty()) {
        throwError("Empty delimited identifier");
    }
    if (identifier.length() > config.maxIdentLength) {
        throwError("Delimited identifier exceeds maximum length", identifier);
    }

    // 5. Validate content with SQL rules
    if (!validateDelimitedIdentifier(identifier)) {
        throwError("Invalid characters in delimited identifier", identifier);
    }

    // 6. Create and return delimited identifier token
    return createDelimitedIdentifierToken(identifier, category);
}

/**
 * @brief Lex a function call.
 * @return Unique pointer to FunctionToken or nullptr if not a function
 */
std::unique_ptr<Token> Lexer::lexFunction() {
    Position savedPos = savePosition();

    // Get potential function name
    size_t start = pos;
    while (pos < input.size() && isIdentifierPart(input[pos])) {
        advance();
    }

    std::string name = input.substr(start, pos - start);
    std::string upperName = name;
    std::transform(upperName.begin(), upperName.end(), upperName.begin(), ::toupper);

    // Skip whitespace
    skipWhitespaceAndNewlines();

    // Check if followed by '('
    if (peek() != '(') {
        restorePosition(savedPos);
        return nullptr;
    }

    // Check if it's a known function
    auto funcToken = createFunctionToken(upperName);
    if (funcToken) {
        return funcToken;
    }

    // Not a recognized function
    restorePosition(savedPos);
    return nullptr;
}

/**
 * @brief Lex a SQL date/time part token from the input.
 * @return Unique pointer to DateTimePartToken, or nullptr if not a date/time part.
 */
std::unique_ptr<Token> Lexer::lexDateTimePart() {
    size_t start = pos;
    // 1. Collect identifier characters (letters, digits, underscores)
    while (pos < input.size() && isIdentifierPart(input[pos])) {
        advance();
    }
    // 2. Extract the word and normalize to uppercase
    std::string word = input.substr(start, pos - start);
    if (word.empty()) {
        pos = start;
        return nullptr;
    }
    std::string upperWord = word;
    std::transform(upperWord.begin(), upperWord.end(), upperWord.begin(), ::toupper);

    // 3. Attempt to create a DateTimePartToken if recognized
    auto dtTok = createDateTimePartToken(upperWord);
    if (dtTok) {
        return dtTok;
    }
    // 4. Not a date/time part, reset position and return nullptr
    pos = start;
    return nullptr;
}

/**
 * @brief Lex a literal category (e.g., INTEGER, STRING, JSON)
 * @return Unique pointer to LiteralCategoryToken, or nullptr if not a literal category
 */
std::unique_ptr<Token> Lexer::lexLiteralCategory() {
    size_t start = pos;
    // 1. Collect letters
    while (pos < input.size() && std::isalpha(static_cast<unsigned char>(input[pos]))) {
        advance();
    }
    // 2. Extract and uppercase
    std::string word = input.substr(start, pos - start);
    if (word.empty()) {
        pos = start;
        return nullptr;
    }
    std::string upperWord = word;
    std::transform(upperWord.begin(), upperWord.end(), upperWord.begin(), ::toupper);

    // 3. Create token if matches
    auto tok = createLiteralCategoryToken(upperWord);
    if (tok) {
        return tok;
    }

    // 4. Not a literal category
    pos = start;
    return nullptr;
}

/**
 * @brief Lexes a SQL comment (single-line or multi-line).
 * @return Unique pointer to CommentToken, or nullptr if not a comment.
 * @throws LexerError if unterminated multi-line comment or invalid syntax.
 */
std::unique_ptr<Token> Lexer::lexComment() {
    size_t start = pos;

    // 1. Check for valid start: -- (single-line) or /* (multi-line)
    if (pos + 1 >= input.size()) {
        return nullptr;
    }

    // Single-line comment: --
    if (input[pos] == '-' && input[pos + 1] == '-') {
        pos += 2; // Skip --
        CommentType type = CommentType::SINGLE_LINE;

        // 2. Parse content until end of line or input
        while (pos < input.size() && input[pos] != '\n' && input[pos] != '\r') {
            pos++;
        }
        // 3. Extract content (including --)
        std::string content = input.substr(start, pos - start);

        // 4. Create and return token
        return createCommentToken(content, type);
    }

    // Multi-line comment: /*
    if (input[pos] == '/' && input[pos + 1] == '*') {
        pos += 2; // Skip /*
        CommentType type = CommentType::MULTI_LINE;
        bool terminated = false;

        // 2. Parse content until closing */
        while (pos + 1 < input.size()) {
            if (input[pos] == '*' && input[pos + 1] == '/') {
                pos += 2; // Skip */
                terminated = true;
                break;
            }
            pos++;
        }

        // 3. Check for valid end
        if (!terminated) {
            throwError("Unterminated multi-line comment");
        }

        // 4. Extract content (including /* ... */)
        std::string content = input.substr(start, pos - start);

        // 5. Create and return token
        return createCommentToken(content, type);
    }

    return nullptr;
}

/**
 * @brief Lexes a SQL operator from the input.
 * @param isWordOp If true, attempts to lex word operators (e.g., AND, LIKE, IS NOT DISTINCT FROM).
 *                 If false, attempts to lex symbolic operators (e.g., +, !=, ||, ->>).
 * @return Unique pointer to OperatorToken, or nullptr if not an operator.
 */
std::unique_ptr<Token> Lexer::lexOperator(bool isWordOp) {
    size_t start = pos;

    if (isWordOp) {
        // Try to match longest word-operator, up to 4 words
        constexpr int maxWords = 4;
        size_t lookahead = pos;
        for (int wordCount = maxWords; wordCount > 0; --wordCount) {
            size_t la = lookahead;
            std::vector<std::string> words;
            for (int i = 0; i < wordCount; ++i) {
                while (la < input.size() && std::isspace(input[la])) ++la;
                size_t wordStart = la;
                while (la < input.size() && isIdentifierPart(input[la])) ++la;
                if (la == wordStart) break;
                words.push_back(input.substr(wordStart, la - wordStart));
            }
            if (words.size() == wordCount) {
                std::string joined;
                for (size_t i = 0; i < words.size(); ++i) {
                    if (i > 0) joined += " ";
                    joined += words[i];
                }
                std::string upperJoined = joined;
                std::transform(upperJoined.begin(), upperJoined.end(), upperJoined.begin(), ::toupper);

                if (compMap.count(upperJoined)) {
                    pos += (la - start);
                    return std::make_unique<OperatorToken>(compMap.at(upperJoined), joined);
                }
                if (logMap.count(upperJoined)) {
                    pos += (la - start);
                    return std::make_unique<OperatorToken>(logMap.at(upperJoined), joined);
                }
            }
        }
        return nullptr;
    }
    else {
        constexpr size_t maxOpLen = 3;
        for (size_t len = maxOpLen; len > 0; --len) {
            if (pos + len <= input.size()) {
                std::string op = input.substr(pos, len);
                if (!std::all_of(op.begin(), op.end(), [this](char c) { return isOperatorChar(c); })) {
                    continue;
                }
                auto opToken = createOperatorToken(op);
                if (opToken && opToken->getType() == TokenType::OPERATOR) {
                    pos += len;
                    return opToken;
                }
            }
        }
        return nullptr;
    }
}

/**
 * @brief Lexes a punctuator (symbols, delimiters, etc.).
 * @return Unique pointer to PunctuatorToken, or nullptr if not a punctuator.
 */
std::unique_ptr<Token> Lexer::lexPunctuator() {
    size_t start = pos;

    // 1. Try two-character punctuators first (e.g., "::")
    if (pos + 1 < input.size()) {
        std::string twoChar = input.substr(pos, 2);

        // Check all known punctuator maps for two-char match
        if (punctuatorMap.count(twoChar) || tsqlSymMap.count(twoChar)) {
            auto punctToken = createPunctuatorToken(twoChar);
            if (punctToken) {
                advance(2);
                return punctToken;
            }
        }
    }

    // 2. Try single-character punctuators
    if (isAtEnd()) {
        return nullptr;
    }
    char c = peek();

    // Check string delimiters as possible punctuators (if desired)
    if (strDelimMap.count(c)) {
        auto punctToken = createPunctuatorToken(std::string(1, c));
        if (punctToken) {
            advance();
            return punctToken;
        }
    }

    std::string oneChar(1, c);
    if (punctuatorMap.count(oneChar) || tsqlSymMap.count(oneChar)) {
        auto punctToken = createPunctuatorToken(oneChar);
        if (punctToken) {
            advance();
            return punctToken;
        }
    }

    // 3. Not a punctuator, return nullptr
    return nullptr;
}

/**
 * @brief Lex a dot operator or member access.
 * @return Unique pointer to PunctuatorToken
 */
std::unique_ptr<Token> Lexer::lexDotOperator() {
    if (!match('.')) {
        return nullptr;
    }

    // Look ahead to see if this is part of a numeric literal
    if (pos < input.size() && std::isdigit(peek())) {
        pos--; // Move back to include the dot in number lexing
        return nullptr;
    }

    return createPunctuatorToken(".");
}

// === Literal Lexing Methods ===

/**
 * @brief Lex a string literal value (handles escapes and doubled single quotes).
 * @param allowEscape Whether to handle escape sequences (E'' strings)
 * @return String content without delimiters
 */
std::string Lexer::lexStringContent(bool allowEscape) {
    // 1. Check for valid start (single quote)
    char quote = input[pos++];
    if (quote != '\'') {
        throwError("Expected opening single quote for string literal");
    }

    std::string content;

    // 2. Parse content, handling escapes and doubled single quotes
    while (pos < input.size()) {
        // Handle end-of-string: if current is quote
        if (input[pos] == '\'') {
            // If next char is also quote, it's a literal quote
            if (pos + 1 < input.size() && input[pos + 1] == '\'') {
                content += '\''; // Add a single quote
                pos += 2;        // Skip both quotes
                continue;
            }
            else {
                // Single quote, end of string
                pos++; // Skip closing quote
                break;
            }
        }

        // 3. Handle escapes (only for E'' strings)
        if (allowEscape && input[pos] == '\\' && pos + 1 < input.size()) {
            pos++;
            switch (input[pos]) {
            case 'n': content += '\n'; break;
            case 'r': content += '\r'; break;
            case 't': content += '\t'; break;
            case 'b': content += '\b'; break;
            case 'f': content += '\f'; break;
            case '\\': content += '\\'; break;
            case '\'': content += '\''; break;
            case '"': content += '"'; break;
            default:
                throwError("Invalid escape sequence", std::string("\\") + input[pos]);
            }
            pos++;    // move past the escaped character
            continue;
        }
        else {
            // 4. Add normal character
            content += input[pos];
        }
        pos++;
    }

    // 5. Validate end of string
    if (pos > input.size()) {
        throwError("Unterminated string literal");
    }

    return content;
}

/**
 * @brief Lex a string literal (single-quoted, SQL style).
 * @return LiteralToken
 */
std::unique_ptr<Token> Lexer::lexString() {
    size_t start = pos;

    // 1. Check for valid start (single quote)
    if (input[pos] != '\'') {
        return nullptr;
    }
    pos++; // Skip opening quote

    // 2. Parse content, handling doubled single quotes as escapes
    std::string content;
    while (pos < input.size()) {
        if (input[pos] == '\'') {
            // Check for doubled single quote (escaped quote)
            if (pos + 1 < input.size() && input[pos + 1] == '\'') {
                content += '\'';
                pos += 2;
                continue;
            }
            else {
                // End of string literal
                pos++;
                break;
            }
        }
        content += input[pos++];
    }

    // 3. Check for valid end (unterminated string)
    if (content.empty() || (pos > input.size() && input[input.size() - 1] != '\'')) {
        throwError("Unterminated string literal");
    }

    // 4. Validate content length
    if (content.length() > config.maxStringLength) {
        throwError("String literal exceeds maximum length", content);
    }

    // 5. Create and return token
    return createStringLiteral("'" + content + "'");
}

/**
 * @brief Lex an escape string literal (E'...').
 * @return LiteralToken
 */
std::unique_ptr<Token> Lexer::lexEscapeString() {
    size_t start = pos;

    // 1. Check for valid start (E' or e')
    if (!((pos + 1 < input.size()) && (input[pos] == 'E' || input[pos] == 'e') && input[pos + 1] == '\'')) {
        return nullptr;
    }
    pos++; // Skip E

    // 2. Parse content with escape handling
    std::string content = lexStringContent(true);

    // 3. Validate content length
    if (content.length() > config.maxStringLength) {
        throwError("Escape string literal exceeds maximum length", content);
    }

    // 4. Create and return token
    return createEscapeStringLiteral(content);
}

/**
 * @brief Lexes a character literal (C'A' or c'A')
 * @return Token representing the character literal
 * @throws LexerError if invalid character literal
 */
std::unique_ptr<Token> Lexer::lexCharacter() {
    size_t start = pos;

    // 1. Check for valid start (C' or c')
    if (!((pos + 1 < input.size()) && (input[pos] == 'C' || input[pos] == 'c') && input[pos + 1] == '\'')) {
        return nullptr;
    }
    pos += 2; // Skip C'

    // 2. Parse content (handle escape)
    char value = '\0';
    if (pos >= input.size()) {
        throwError("Unterminated character literal");
    }

    if (input[pos] == '\\') {
        // Handle escaped character
        pos++;
        if (pos >= input.size()) {
            throwError("Unterminated character literal - escape sequence");
        }
        std::string escaped = getUnescapedChar("\\" + std::string(1, input[pos]));
        if (escaped.length() != 1) {
            throwError("Invalid escape sequence in character literal");
        }
        value = escaped[0];
        pos++;
    }
    else {
        value = input[pos++];
    }

    // 3. Must end with single quote
    if (pos >= input.size() || input[pos] != '\'') {
        throwError("Unterminated character literal");
    }
    pos++; // Skip closing quote

    // 4. Create and return token
    return createCharLiteral(value);
}

/**
 * @brief Process optional sign at current position
 * @param start Reference to start position for literal
 * @param isNegative Flag set if literal is negative
 */
void Lexer::processSign(size_t& start, bool& isNegative) {
    start = pos;
    isNegative = false;

    // 1. Optional sign
    if (peek() == '-') {
        isNegative = true;
        advance();
    }
    else if (peek() == '+') {
        advance();
    }
}

/**
 * @brief Lex a numeric literal (integer or float)
 * @return LiteralToken
 */
std::unique_ptr<Token> Lexer::lexNumber() {
    size_t start;
    bool isNegative;
    processSign(start, isNegative);

    // 4. Standard integer/float parsing
    size_t numberStart = pos;
    bool hasDigits = false;
    bool hasDecimal = false;
    bool hasExponent = false;

    // Integer part
    while (pos < input.size() && std::isdigit(input[pos])) {
        hasDigits = true;
        advance();
    }

    // Decimal part
    if (peek() == '.') {
        hasDecimal = true;
        advance();
        if (pos >= input.size() || !std::isdigit(input[pos])) {
            throwError("Invalid decimal point in number");
        }
        while (pos < input.size() && std::isdigit(input[pos])) {
            hasDigits = true;
            advance();
        }
    }

    // Exponent part
    if (peek() == 'e' || peek() == 'E') {
        hasExponent = true;
        advance();
        if (peek() == '+' || peek() == '-') {
            advance();
        }
        if (pos >= input.size() || !std::isdigit(input[pos])) {
            throwError("Invalid exponent in number");
        }
        while (pos < input.size() && std::isdigit(input[pos])) {
            advance();
        }
    }

    // No digits found
    if (!hasDigits) {
        throwError("Invalid numeric literal");
    }

    std::string number = input.substr(start, pos - start);

    // 5. Validate number string
    if (!validateNumber(number)) {
        throwError("Invalid numeric literal", number);
    }

    // 6. Create and return token
    try {
        if (hasDecimal || hasExponent) {
            double value = std::stod(number);
            return createFloatLiteral(value);
        }
        else {
            int64_t value = std::stoll(number);
            return createIntegerLiteral(value);
        }
    }
    catch (const std::exception&) {
        throwError("Numeric literal out of range", number);
    }

    return nullptr;
}

/**
 * @brief Lex a C-style binary literal (0b...)
 * @return LiteralToken
 */
std::unique_ptr<Token> Lexer::lexCStyleBinary() {
    size_t start = pos;

    // 3. C-style binary literal: 0b101010
    if (peek() == '0' && (peekNext() == 'b' || peekNext() == 'B')) {
        advance(); // Skip '0'
        advance(); // Skip 'b' or 'B'
        size_t binStart = pos;
        while (pos < input.size() && (input[pos] == '0' || input[pos] == '1')) {
            advance();
        }
        if (binStart == pos) {
            throwError("Invalid binary literal: missing digits after 0b");
        }
        std::string binDigits = input.substr(binStart, pos - binStart);
        std::string fullLiteral = "0b" + binDigits;

        // Validate binary literal
        if (!validateCStyleBinary(fullLiteral)) {
            throwError("Invalid binary literal", fullLiteral);
        }

        return createBinaryLiteral(binDigits);
    }

    return nullptr;
}

/**
 * @brief Lex a C-style hexadecimal literal (0x...)
 * @return LiteralToken
 */
std::unique_ptr<Token> Lexer::lexCStyleHex() {
    size_t start = pos;

    // 4. C-style hex literal: 0xDEADBEEF
    if (peek() == '0' && (peekNext() == 'x' || peekNext() == 'X')) {
        advance(); // Skip '0'
        advance(); // Skip 'x' or 'X'
        size_t hexStart = pos;
        while (pos < input.size() && std::isxdigit(input[pos])) {
            advance();
        }
        if (hexStart == pos) {
            throwError("Invalid hexadecimal literal: missing digits after 0x");
        }
        std::string hexDigits = input.substr(hexStart, pos - hexStart);
        std::string fullLiteral = "0x" + hexDigits;

        // Validate hex literal
        if (!validateCStyleHex(fullLiteral)) {
            throwError("Invalid hexadecimal literal", fullLiteral);
        }

        return createHexLiteral(hexDigits);
    }

    return nullptr;
}

/**
 * @brief Lex a SQL-style binary literal (B'0101')
 * @return LiteralToken
 */
std::unique_ptr<Token> Lexer::lexBinary() {
    size_t start = pos;

    // 1. SQL-style binary literal: B'1010'
    if (peek() == 'B' || peek() == 'b') {
        if (peekNext() != '\'') return nullptr;
        advance(); // Skip 'B'
        advance(); // Skip opening quote
        size_t binStart = pos;
        while (pos < input.size() && (input[pos] == '0' || input[pos] == '1')) {
            advance();
        }
        if (binStart == pos || pos >= input.size() || input[pos] != '\'') {
            throwError("Invalid or unterminated binary literal");
        }
        std::string binDigits = input.substr(binStart, pos - binStart);
        std::string fullLiteral = "B'" + binDigits + "'";
        advance(); // Skip closing quote

        // Validate binary literal
        if (!validateBinary(fullLiteral)) {
            throwError("Invalid binary literal", fullLiteral);
        }

        return createBinaryLiteral(binDigits);
    }

    return nullptr;
}

/**
 * @brief Lex a SQL-style hexadecimal literal (X'1A2B')
 * @return LiteralToken
 */
std::unique_ptr<Token> Lexer::lexHex() {
    size_t start = pos;

    // 2. SQL-style hex literal: X'1A2B'
    if (peek() == 'X' || peek() == 'x') {
        if (peekNext() != '\'') return nullptr;
        advance(); // Skip 'X'
        advance(); // Skip opening quote
        size_t hexStart = pos;
        while (pos < input.size() && std::isxdigit(static_cast<unsigned char>(input[pos]))) {
            advance();
        }
        if (hexStart == pos || pos >= input.size() || input[pos] != '\'') {
            throwError("Invalid or unterminated hexadecimal literal");
        }
        std::string hexDigits = input.substr(hexStart, pos - hexStart);
        std::string fullLiteral = "X'" + hexDigits + "'";
        advance(); // Skip closing quote

        // Validate hex literal
        if (!validateHex(fullLiteral)) {
            throwError("Invalid hexadecimal literal", fullLiteral);
        }

        return createHexLiteral(hexDigits);
    }

    return nullptr;
}

/**
 * @brief Lex a boolean literal (TRUE or FALSE).
 * @return LiteralToken
 */
std::unique_ptr<Token> Lexer::lexBoolean() {
    size_t start = pos;

    // 1. Check for valid start (identifier part)
    if (!isIdentifierStart(input[pos])) {
        return nullptr;
    }

    // 2. Parse the word
    std::string word;
    while (pos < input.size() && isIdentifierPart(input[pos])) {
        word += input[pos];
        advance();
    }

    // 3. Check for valid boolean literal (case-insensitive)
    std::string upperWord = word;
    std::transform(upperWord.begin(), upperWord.end(), upperWord.begin(), ::toupper);

    if (upperWord == "TRUE") {
        // 4. Create and return token
        return createBooleanLiteral(true);
    }
    else if (upperWord == "FALSE") {
        // 4. Create and return token
        return createBooleanLiteral(false);
    }

    // 5. Not a valid boolean literal, reset position and return nullptr
    pos = start;
    return nullptr;
}

/**
 * @brief Lex a NULL literal.
 * @return LiteralToken
 */
std::unique_ptr<Token> Lexer::lexNull() {
    size_t start = pos;

    // 1. Check for valid start (identifier part)
    if (!isIdentifierStart(input[pos])) {
        return nullptr;
    }

    // 2. Parse the word
    std::string word;
    while (pos < input.size() && isIdentifierPart(input[pos])) {
        word += input[pos];
        advance();
    }

    // 3. Check for valid NULL literal (case-insensitive)
    std::string upperWord = word;
    std::transform(upperWord.begin(), upperWord.end(), upperWord.begin(), ::toupper);

    if (upperWord == "NULL") {
        // 4. Create and return token
        return createNullLiteral();
    }

    // 5. Not a valid NULL literal, reset position and return nullptr
    pos = start;
    return nullptr;
}

/**
 * @brief Lex a date literal ('YYYY-MM-DD').
 * @return LiteralToken
 */
std::unique_ptr<Token> Lexer::lexDate() {
    size_t start = pos;

    // 1. Check for valid start (single quote)
    if (input[pos] != '\'') {
        return nullptr;
    }
    pos++; // Skip opening quote

    // 2. Parse date content (expecting exactly 10 characters: YYYY-MM-DD)
    std::string date;
    for (int i = 0; i < 10; i++) {
        if (pos >= input.size()) {
            throwError("Unterminated date literal");
        }
        if ((i == 4 || i == 7) && input[pos] != '-') {
            throwError("Invalid date format, expected '-'");
        }
        if ((i != 4 && i != 7) && !std::isdigit(static_cast<unsigned char>(input[pos]))) {
            throwError("Invalid date format, expected digit");
        }
        date += input[pos++];
    }

    // 3. Check for valid end (closing quote)
    if (pos >= input.size() || input[pos] != '\'') {
        throwError("Unterminated date literal");
    }
    pos++; // Skip closing quote

    // 4. Validate date string
    if (!validateDate(date)) {
        throwError("Invalid date format", date);
    }

    // 5. Create and return token
    return createDateLiteral(date);
}

/**
 * @brief Lex a time literal ('HH:MM:SS').
 * @return LiteralToken
 */
std::unique_ptr<Token> Lexer::lexTime() {
    size_t start = pos;

    // 1. Check for valid start (single quote)
    if (input[pos] != '\'') {
        return nullptr;
    }
    pos++; // Skip opening quote

    // 2. Parse time content (expecting exactly 8 characters: HH:MM:SS)
    std::string time;
    for (int i = 0; i < 8; i++) {
        if (pos >= input.size()) {
            throwError("Unterminated time literal");
        }
        if ((i == 2 || i == 5) && input[pos] != ':') {
            throwError("Invalid time format, expected ':'");
        }
        if ((i != 2 && i != 5) && !std::isdigit(static_cast<unsigned char>(input[pos]))) {
            throwError("Invalid time format, expected digit");
        }
        time += input[pos++];
    }

    // 3. Check for valid end (closing quote)
    if (pos >= input.size() || input[pos] != '\'') {
        throwError("Unterminated time literal");
    }
    pos++; // Skip closing quote

    // 4. Validate time string
    if (!validateTime(time)) {
        throwError("Invalid time format", time);
    }

    // 5. Create and return token
    return createTimeLiteral(time);
}

/**
 * @brief Lex a datetime literal ('YYYY-MM-DD HH:MM:SS').
 * @return LiteralToken
 */
std::unique_ptr<Token> Lexer::lexDateTime() {
    size_t start = pos;

    // 1. Check for valid start (single quote)
    if (input[pos] != '\'') {
        return nullptr;
    }
    pos++; // Skip opening quote

    // 2. Parse date part (YYYY-MM-DD)
    std::string datetime;
    for (int i = 0; i < 10; i++) {
        if (pos >= input.size()) {
            throwError("Unterminated datetime literal");
        }
        if ((i == 4 || i == 7) && input[pos] != '-') {
            throwError("Invalid datetime format, expected '-'");
        }
        if ((i != 4 && i != 7) && !std::isdigit(static_cast<unsigned char>(input[pos]))) {
            throwError("Invalid datetime format, expected digit");
        }
        datetime += input[pos++];
    }

    // 3. Check for space separator
    if (pos >= input.size() || input[pos] != ' ') {
        throwError("Invalid datetime format, expected space after date");
    }
    datetime += input[pos++];

    // 4. Parse time part (HH:MM:SS)
    for (int i = 0; i < 8; i++) {
        if (pos >= input.size()) {
            throwError("Unterminated datetime literal");
        }
        if ((i == 2 || i == 5) && input[pos] != ':') {
            throwError("Invalid datetime format, expected ':'");
        }
        if ((i != 2 && i != 5) && !std::isdigit(static_cast<unsigned char>(input[pos]))) {
            throwError("Invalid datetime format, expected digit");
        }
        datetime += input[pos++];
    }

    // 5. Check for valid end (closing quote)
    if (pos >= input.size() || input[pos] != '\'') {
        throwError("Unterminated datetime literal");
    }
    pos++; // Skip closing quote

    // 6. Validate datetime string
    if (!validateDateTime(datetime)) {
        throwError("Invalid datetime format", datetime);
    }

    // 7. Create and return token
    return createDateTimeLiteral(datetime);
}

/**
 * @brief Lex an INTERVAL literal supporting both standard SQL and PostgreSQL-style intervals.
 * @return Unique pointer to LiteralToken representing the interval
 */
std::unique_ptr<Token> Lexer::lexInterval() {
    size_t start = pos;

    // 1. Parse "INTERVAL" keyword
    std::string word;
    size_t kwStart = pos;
    while (pos < input.size() && isIdentifierPart(input[pos])) {
        word += input[pos];
        advance();
    }
    std::string upperWord = word;
    std::transform(upperWord.begin(), upperWord.end(), upperWord.begin(), ::toupper);
    if (upperWord != "INTERVAL") {
        pos = start;
        return nullptr;
    }

    skipWhitespaceAndNewlines();

    // 2. Parse quoted string value
    std::string value;
    if (peek() == '\'') {
        advance();
        while (!isAtEnd() && peek() != '\'') {
            value += peek();
            advance();
        }
        if (!match('\'')) {
            throwError("Unterminated interval string");
        }
    }
    else {
        throwError("Expected quoted interval string");
    }
    if (value.empty()) {
        throwError("Empty interval value");
    }

    skipWhitespaceAndNewlines();

    // 3. Try to parse optional unit
    std::string fromUnit;
    while (!isAtEnd() && isIdentifierPart(peek())) {
        fromUnit += peek();
        advance();
    }
    skipWhitespaceAndNewlines();

    // 4. Try to parse optional "TO <unit>"
    std::string toUnit;
    size_t savedPos = pos;
    std::string maybeTo;
    while (!isAtEnd() && isIdentifierPart(peek())) {
        maybeTo += peek();
        advance();
    }
    std::string upperMaybeTo = maybeTo;
    std::transform(upperMaybeTo.begin(), upperMaybeTo.end(), upperMaybeTo.begin(), ::toupper);
    if (upperMaybeTo == "TO") {
        skipWhitespaceAndNewlines();
        while (!isAtEnd() && isIdentifierPart(peek())) {
            toUnit += peek();
            advance();
        }
    }
    else {
        pos = savedPos;
    }

    // 5. Create and return token
    if (fromUnit.empty()) {
        // PostgreSQL style
        return createPostgresIntervalLiteral(value);
    }
    else {
        // Standard SQL
        return createStandardIntervalLiteral(value, fromUnit, toUnit);
    }
}

 /**
  * @brief Lex a JSON literal ({...}).
  * @return LiteralToken
  */
std::unique_ptr<Token> Lexer::lexJSON() {
    size_t start = pos;

    // 1. Check for valid start (opening brace)
    if (input[pos] != '{') {
        return nullptr;
    }
    int braceCount = 1;
    pos++; // Skip opening brace

    // 2. Parse the JSON object, handling nested braces and quoted strings
    while (pos < input.size() && braceCount > 0) {
        if (input[pos] == '{') {
            braceCount++;
            pos++;
        }
        else if (input[pos] == '}') {
            braceCount--;
            pos++;
            if (braceCount == 0) break;
        }
        else if (input[pos] == '"') {
            // Skip string content, handling escapes
            pos++;
            while (pos < input.size()) {
                if (input[pos] == '\\' && pos + 1 < input.size()) {
                    pos += 2;
                }
                else if (input[pos] == '"') {
                    pos++;
                    break;
                }
                else {
                    pos++;
                }
            }
            if (pos > input.size()) {
                throwError("Unterminated string in JSON literal");
            }
        }
        else {
            pos++;
        }
    }

    // 3. Check for valid end (unterminated object)
    if (braceCount != 0) {
        throwError("Unterminated JSON literal");
    }

    // 4. Extract and validate the JSON string
    std::string json = input.substr(start, pos - start);
    if (!validateJSON(json)) {
        throwError("Invalid JSON format", json);
    }

    // 5. Create and return token
    return createJSONLiteral(json);
}

/**
 * @brief Lex an XML literal (<...>...</...>) with full XML support
 * @return LiteralToken
 */
std::unique_ptr<Token> Lexer::lexXML() {
    size_t start = pos;

    // 1. Check for valid start (opening angle bracket)
    if (peek() != '<') {
        return nullptr;
    }

    int depth = 0;
    while (pos < input.size()) {
        // 2. XML comment: <!-- ... -->
        if (input.compare(pos, 4, "<!--") == 0) {
            pos += 4;
            size_t end = input.find("-->", pos);
            if (end == std::string::npos) throwError("Unterminated XML comment");
            pos = end + 3;
            continue;
        }
        // 3. CDATA section: <![CDATA[ ... ]]>
        if (input.compare(pos, 9, "<![CDATA[") == 0) {
            pos += 9;
            size_t end = input.find("]]>", pos);
            if (end == std::string::npos) throwError("Unterminated CDATA section");
            pos = end + 3;
            continue;
        }
        // 4. Processing instruction: <? ... ?>
        if (input.compare(pos, 2, "<?") == 0) {
            pos += 2;
            size_t end = input.find("?>", pos);
            if (end == std::string::npos) throwError("Unterminated processing instruction");
            pos = end + 2;
            continue;
        }
        // 5. Tag handling: opening, closing, self-closing
        if (peek() == '<') {
            bool closing = peekNext() == '/';
            bool selfClosing = false;
            depth += closing ? -1 : 1;
            advance(); // Skip '<'
            if (closing) advance(); // Skip '/'
            // Skip tag name and attributes
            while (pos < input.size() && peek() != '>') {
                if (peek() == '"' || peek() == '\'') {
                    char q = peek(); advance();
                    while (pos < input.size() && peek() != q) advance();
                    if (peek() == q) advance();
                }
                else {
                    advance();
                }
            }
            if (peek() == '/') {
                // Self-closing tag
                selfClosing = true;
                depth--;
            }
            if (peek() != '>') throwError("Unterminated XML tag");
            advance(); // Skip '>'
            if (depth <= 0) break;
            continue;
        }
        // 6. Text content: skip until next markup
        advance();
    }

    // 7. Check for unterminated XML
    if (depth != 0) {
        throwError("Unterminated XML literal");
    }

    // 8. Extract and validate XML string
    std::string xml = input.substr(start, pos - start);
    if (!validateXML(xml)) {
        throwError("Invalid XML format", xml);
    }

    // 9. Create and return XML token
    return createXMLLiteral(xml);
}

/**
 * @brief Lex a UUID literal.
 * @return LiteralToken
 */
std::unique_ptr<Token> Lexer::lexUUID() {
    size_t start = pos;

    // 1. Check for valid start (36-character UUID format)
    if (pos + 36 > input.size()) {
        return nullptr;
    }
    std::string uuid = input.substr(pos, 36);

    // 2. Validate UUID format
    if (!validateUUID(uuid)) {
        return nullptr;
    }

    // 3. Advance position by 36 characters
    pos += 36;

    // 4. Create and return token
    return createUUIDLiteral(uuid);
}

/**
 * @brief Lex an array literal ([...]).
 * @return LiteralToken
 */
std::unique_ptr<Token> Lexer::lexArray() {
    size_t start = pos;

    // 1. Check for valid start ('[')
    if (input[pos] != '[') {
        return nullptr;
    }
    size_t arrayStart = pos;
    int bracketCount = 1;
    pos++; // Skip opening '['

    // 2. Find the matching closing bracket, handling nested arrays and strings
    bool inString = false;
    char stringQuote = 0;
    while (pos < input.size() && bracketCount > 0) {
        char c = input[pos];
        if (inString) {
            if (c == stringQuote && input[pos - 1] != '\\') {
                inString = false;
            }
        }
        else {
            if (c == '\'' || c == '"') {
                inString = true;
                stringQuote = c;
            }
            else if (c == '[') {
                bracketCount++;
            }
            else if (c == ']') {
                bracketCount--;
                if (bracketCount == 0) {
                    pos++; // Include closing bracket
                    break;
                }
            }
        }
        pos++;
    }

    // 3. Check for valid end (unterminated array)
    if (bracketCount != 0) {
        throwError("Unterminated array literal");
    }

    // 4. Parse and validate elements
    std::string arrayStr = input.substr(arrayStart, pos - arrayStart);
    std::vector<std::shared_ptr<LiteralValue>> elems = parseArrayElements(arrayStr);

    // 5. Create and return token
    return createArrayLiteral(elems);
}

/**
 * @brief Parses the elements of an array literal ([...]) into literal values.
 * @param arrayStr The array literal string, including brackets.
 * @return Vector of shared pointers to parsed LiteralValue objects for each element.
 * @throws LexerError if the array syntax is invalid or any element is not a valid literal.
 *
 * @details
 * Handles nested arrays, string literals, and all supported literal types.
 * Splits elements at top-level commas, ignoring commas inside nested arrays or strings.
 * Recursively parses nested array elements.
 */
std::vector<std::shared_ptr<LiteralValue>> Lexer::parseArrayElements(const std::string& arrayStr) {
    std::vector<std::shared_ptr<LiteralValue>> elems;
    if (arrayStr.length() < 2 || arrayStr.front() != '[' || arrayStr.back() != ']')
        throwError("Invalid array literal", arrayStr);

    std::string inner = arrayStr.substr(1, arrayStr.length() - 2);
    size_t pos = 0, start = 0;
    int bracketDepth = 0;
    bool inString = false;
    char stringQuote = 0;

    while (pos <= inner.size()) {
        char c = (pos < inner.size()) ? inner[pos] : ','; // Sentinel comma at end
        if (inString) {
            if (c == stringQuote && (pos == 0 || inner[pos - 1] != '\\')) {
                inString = false;
            }
        }
        else {
            if (c == '\'' || c == '"') {
                inString = true;
                stringQuote = c;
            }
            else if (c == '[') {
                bracketDepth++;
            }
            else if (c == ']') {
                bracketDepth--;
            }
            else if ((c == ',' || pos == inner.size()) && bracketDepth == 0) {
                std::string elem = inner.substr(start, pos - start);
                // Trim whitespace
                elem.erase(0, elem.find_first_not_of(" \t\r\n"));
                elem.erase(elem.find_last_not_of(" \t\r\n") + 1);
                if (!elem.empty()) {
                    auto tok = createLiteralToken(elem);
                    auto litTok = dynamic_cast<LiteralToken*>(tok.get());
                    if (litTok) {
                        elems.push_back(litTok->getValue());
                    }
                    else {
                        throwError("Invalid array element", elem);
                    }
                }
                start = pos + 1;
            }
        }
        ++pos;
    }
    return elems;
}

// === Dollar Quote Support ===

/**
 * @brief Checks if current position starts a dollar-quoted string.
 * @return true if a valid dollar quote tag is found
 */
bool Lexer::isDollarQuoteStart() const {
    if (pos >= input.size() || input[pos] != '$') {
        return false;
    }

    // Find next '$'
    size_t tagEnd = input.find('$', pos + 1);
    if (tagEnd == std::string::npos) {
        return false;
    }

    // Extract tag content (could be empty)
    std::string tagContent = input.substr(pos + 1, tagEnd - pos - 1);

    // Empty tag is always valid ($$)
    if (tagContent.empty()) return true;

    // Otherwise, must be a valid tag: [A-Za-z_][A-Za-z0-9_]*
    if (!std::isalpha(tagContent[0]) && tagContent[0] != '_') return false;
    for (char c : tagContent) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_') return false;
    }
    return true;
}

/**
 * @brief Extracts the dollar quote tag from the current position.
 * @param[out] tag The extracted tag including dollar signs
 * @return true if a valid tag was extracted
 */
bool Lexer::extractDollarQuoteTag(std::string& tag) {
    if (pos >= input.size() || input[pos] != '$') {
        return false;
    }

    size_t tagEnd = input.find('$', pos + 1);
    if (tagEnd == std::string::npos) {
        return false;
    }

    std::string tagContent = input.substr(pos + 1, tagEnd - pos - 1);

    // Empty tag ($$) is valid
    if (tagContent.empty()) {
        tag = "$$";
        return true;
    }

    // Tag must be a valid identifier: [A-Za-z_][A-Za-z0-9_]*
    if (!std::isalpha(tagContent[0]) && tagContent[0] != '_') return false;
    for (char c : tagContent) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_') return false;
    }

    tag = input.substr(pos, tagEnd - pos + 1); // includes both $
    return true;
}

/**
 * @brief Validates a dollar quote tag content.
 * @param tagContent The tag content without dollar signs
 * @return true if the tag content is valid
 */
bool Lexer::validateDollarQuoteTag(const std::string& tagContent) const {
    // Empty tag is valid
    if (tagContent.empty()) return true;
    if (!std::isalpha(tagContent[0]) && tagContent[0] != '_') return false;
    for (char c : tagContent) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_') return false;
    }
    return true;
}

/**
 * @brief Creates a string literal token from a dollar-quoted string.
 * @param original The full string, including dollar tags.
 * @return Token pointer for the string literal.
 * @details
 * Validates the dollar-quoted format and extracts the inner content.
 * Throws on invalid format.
 */
std::unique_ptr<Token> Lexer::createDollarQuotedStringLiteral(const std::string& original) {
    if (!isDollarQuotedString(original)) {
        throwError("Invalid dollar-quoted string literal", original);
    }
    size_t tagEnd = original.find('$', 1);
    std::string tag = original.substr(0, tagEnd + 1);
    std::string value = original.substr(tag.length(), original.length() - 2 * tag.length());
    if (value.length() > config.maxStringLength) {
        throwError("String literal exceeds maximum length", value);
    }
    return std::make_unique<LiteralToken>(
        LiteralCategory::STRING,
        original,
        std::make_shared<StringLiteralValue>(value)
    );
}

/**
 * @brief Lexes a dollar-quoted string literal (supports $$...$$ and $tag$...$tag$).
 * @return LiteralToken for the dollar-quoted string, or nullptr if not a match.
 * @throws LexerError if unterminated or invalid dollar-quoted string.
 */
std::unique_ptr<Token> Lexer::lexDollarQuotedString() {
    if (!config.allowDollarQuotes) {
        throwError("Dollar-quoted strings are not enabled");
    }

    size_t start = pos;

    // 1. Check for valid start: $...$ (could be $$)
    std::string tag;
    if (!extractDollarQuoteTag(tag)) {
        return nullptr;
    }

    // 2. Advance past opening tag
    pos += tag.length();

    // 3. Find matching closing tag
    size_t contentStart = pos;
    size_t contentEnd = input.find(tag, contentStart);

    if (contentEnd == std::string::npos) {
        throwError("Unterminated dollar-quoted string", tag);
    }

    // 4. Advance past closing tag
    pos = contentEnd + tag.length();

    // 5. The original text includes the opening and closing tag
    std::string original = input.substr(start, pos - start);
    return createDollarQuotedStringLiteral(original);
}

/**
 * @brief Checks if a string is a valid dollar-quoted string.
 * @param text The string to check
 * @return true if the string is a valid dollar-quoted string
 */
bool Lexer::isDollarQuotedString(const std::string& text) const {
    if (text.length() < 4) { // Minimum is $$...$$
        return false;
    }
    if (text.front() != '$') {
        return false;
    }

    // Find end of opening tag
    size_t tagEnd = text.find('$', 1);
    if (tagEnd == std::string::npos) {
        return false;
    }

    std::string tag = text.substr(0, tagEnd + 1); // includes both $ delimiters
    std::string tagContent = tag.substr(1, tag.length() - 2);

    // Validate tag content
    if (!validateDollarQuoteTag(tagContent)) {
        return false;
    }

    // Check for matching closing tag at the end
    if (text.length() < tag.length() * 2) {
        return false; // Not enough room for open/close tags and content
    }
    if (text.substr(text.length() - tag.length()) != tag) {
        return false;
    }

    return true;
}

/**
 * @brief Processes nested dollar-quoted strings.
 * @param outerTag The enclosing dollar quote tag
 * @return Content of the nested dollar-quoted string
 */
std::string Lexer::processNestedDollarQuotes(const std::string& outerTag) {
    std::string result;

    while (pos < input.size()) {
        // Check for closing tag
        if (pos + outerTag.length() <= input.size() &&
            input.substr(pos, outerTag.length()) == outerTag) {
            return result;
        }

        // Check for nested dollar quote
        if (input[pos] == '$') {
            std::string innerTag;
            size_t savedPos = pos;

            if (extractDollarQuoteTag(innerTag) && innerTag != outerTag) {
                // Process nested dollar quote
                pos += innerTag.length();
                result += innerTag;
                result += processNestedDollarQuotes(innerTag);
                result += innerTag;
                continue;
            }

            pos = savedPos;
        }

        // Add current character to result
        result += input[pos++];
    }

    throwError("Unterminated dollar-quoted string", outerTag);
    return result; // Never reached
}

/**
 * @brief Handles special characters in dollar-quoted strings.
 * @param text The string content to process
 * @return Processed string with special characters handled
 */
std::string Lexer::processDollarQuoteSpecials(const std::string& text) {
    std::string result;
    result.reserve(text.length());

    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];

        // Handle line endings
        if (c == '\r') {
            if (i + 1 < text.length() && text[i + 1] == '\n') {
                result += '\n';
                ++i; // Skip \n
            }
            else {
                result += '\n';
            }
        }
        // Handle other special characters
        else if (c == '\n' || c == '\t') {
            result += c;
        }
        // Regular characters
        else {
            result += c;
        }
    }

    return result;
}

/**
 * @brief Gets the line number within a dollar-quoted string.
 * @param startPos Starting position in input
 * @param targetPos Target position in input
 * @return Line number at target position
 */
size_t Lexer::getDollarQuoteLineNumber(size_t startPos, size_t targetPos) const {
    size_t lineNum = 1;

    for (size_t i = startPos; i < targetPos && i < input.size(); ++i) {
        if (input[i] == '\n' || (input[i] == '\r' &&
            (i + 1 >= input.size() || input[i + 1] != '\n'))) {
            ++lineNum;
        }
    }

    return lineNum;
}

/**
 * @brief Gets detailed error context for dollar-quoted strings.
 * @param position Error position in input
 * @param tag Dollar quote tag
 * @return Formatted error context string
 */
std::string Lexer::getDollarQuoteErrorContext(size_t position, const std::string& tag) const {
    std::ostringstream context;

    // Find start of line
    size_t lineStart = position;
    while (lineStart > 0 && input[lineStart - 1] != '\n' && input[lineStart - 1] != '\r') {
        --lineStart;
    }

    // Find end of line
    size_t lineEnd = position;
    while (lineEnd < input.size() && input[lineEnd] != '\n' && input[lineEnd] != '\r') {
        ++lineEnd;
    }

    // Build context string
    context << "Error in dollar-quoted string " << tag << " at line "
        << getDollarQuoteLineNumber(pos, position) << ":\n"
        << input.substr(lineStart, lineEnd - lineStart) << '\n'
        << std::string(position - lineStart, ' ') << '^';

    return context.str();
}

// === Token Creation Methods ===

/**
 * @brief Creates a keyword token based on the input word.
 * @param word The input word to check against keyword maps
 * @return Token pointer if word is a keyword, nullptr otherwise
 */
std::unique_ptr<Token> Lexer::createKeywordToken(const std::string& word) {
    std::string upper = word;
    if (!config.caseSensitive) {
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    }

    // Check keyword maps in priority order
    const std::vector<const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>>*> keywordMaps = {
        &dmlMap, &ddlMap, &clauseMap, &cteMap, &setOpMap, &predicateMap,
        &logicalConstMap, &tranMap, &secMap, &progMap, &miscMap
    };

    for (const auto* mapPtr : keywordMaps) {
        auto it = mapPtr->find(upper);
        if (it != mapPtr->end()) {
            return std::make_unique<KeywordToken>(it->second, word);
        }
    }

    return nullptr;
}

/**
 * @brief Creates a function token based on the input word.
 * @param word The input word to check against function maps
 * @return Token pointer if word is a function, nullptr otherwise
 */
std::unique_ptr<Token> Lexer::createFunctionToken(const std::string& word) {
    std::string upper = word;
    if (!config.caseSensitive) {
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    }

    const std::vector<const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>>*> functionMaps = {
        &aggFuncMap, &scalarFuncMap, &stringFuncMap, &dtFuncMap,
        &mathFuncMap, &sysFuncMap, &windowFuncMap
    };

    for (const auto* mapPtr : functionMaps) {
        auto it = mapPtr->find(upper);
        if (it != mapPtr->end()) {
            return std::make_unique<FunctionToken>(it->second, word);
        }
    }

    return nullptr;
}

/**
 * @brief Creates a date/time part token based on the input word.
 * @param word The input word to check against dateTimePartsMap
 * @return Token pointer if word is a recognized date/time part, nullptr otherwise
 */
std::unique_ptr<Token> Lexer::createDateTimePartToken(const std::string& word) {
    auto it = dateTimePartsMap.find(word);
    if (it != dateTimePartsMap.end()) {
        return std::make_unique<DateTimePartToken>(it->second, word);
    }
    return nullptr;
}

/**
 * @brief Creates an operator token based on the input operator string.
 * @param op The operator string to check
 * @return Token pointer for the operator
 */
std::unique_ptr<Token> Lexer::createOperatorToken(const std::string& op) {
    const std::vector<const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>>*> opMaps = {
        &compMap, &assignMap, &logMap, &bitMap, &concatMap, &arithMap, &jsonOpMap, &regexOpMap, &typecastOpMap
    };

    for (const auto* mapPtr : opMaps) {
        auto it = mapPtr->find(op);
        if (it != mapPtr->end()) {
            return std::make_unique<OperatorToken>(it->second, op);
        }
    }

    return nullptr;
}

/**
 * @brief Creates a punctuator token based on the input punctuator string.
 * @param punc The punctuator string to check
 * @return Token pointer for the punctuator
 */
std::unique_ptr<Token> Lexer::createPunctuatorToken(const std::string& punc) {
    // Check TSQL-specific multi-char symbols first
    auto itTsql = tsqlSymMap.find(punc);
    if (itTsql != tsqlSymMap.end()) {
        return std::make_unique<PunctuatorToken>(itTsql->second);
    }

    // Check punctuator map for all symbols
    auto itPunct = punctuatorMap.find(punc);
    if (itPunct != punctuatorMap.end()) {
        return std::make_unique<PunctuatorToken>(itPunct->second);
    }

    // Check string delimiters
    if (punc.length() == 1) {
        char c = punc[0];
        auto itDelim = strDelimMap.find(c);
        if (itDelim != strDelimMap.end()) {
            return std::make_unique<PunctuatorToken>(itDelim->second);
        }
    }

    return nullptr;
}

/**
 * @brief Creates an identifier token with the given category.
 * @param identifier The identifier string
 * @param category The identifier category
 * @return Token pointer for the identifier
 */
std::unique_ptr<Token> Lexer::createIdentifierToken(const std::string& identifier, IdentifierCategory category) {
    auto info = std::make_shared<IdentifierInfo>();
    info->category = category;
    return std::make_unique<IdentifierToken>(identifier, info);
}

/**
 * @brief Create a token for a delimited identifier
 * @param identifier The validated identifier content
 * @param category The identifier category
 * @return Token pointer for the identifier
 */
std::unique_ptr<Token> Lexer::createDelimitedIdentifierToken(const std::string& identifier, IdentifierCategory category) {
    auto info = std::make_shared<IdentifierInfo>();
    info->category = category;
    return std::make_unique<IdentifierToken>(identifier, info);
}

/**
 * @brief Creates a comment token.
 * @param content The comment text
 * @param type The type of comment
 * @return Token pointer for the comment
 */
std::unique_ptr<Token> Lexer::createCommentToken(const std::string& content,
    CommentType type) {
    return std::make_unique<CommentToken>(type, content);
}

/**
 * @brief Creates a literal token with appropriate type detection.
 * @param word The literal string
 * @return Token pointer for the literal
 */
std::unique_ptr<Token> Lexer::createLiteralToken(const std::string& word) {
    // Order matters: more specific types first
    if (isEscapeStringLiteral(word)) {
        return createEscapeStringLiteral(word.substr(2, word.length() - 3));
    }
    if (isBinaryLiteral(word)) {
        return createBinaryLiteral(word.substr(2, word.length() - 3));
    }
    if (isHexLiteral(word)) {
        return createHexLiteral(word.substr(2, word.length() - 3));
    }
    if (isDateTimeLiteral(word)) {
        return createDateTimeLiteral(word.substr(1, word.length() - 2));
    }
    if (isDateLiteral(word)) {
        return createDateLiteral(word.substr(1, word.length() - 2));
    }
    if (isTimeLiteral(word)) {
        return createTimeLiteral(word.substr(1, word.length() - 2));
    }
    if (isJSONLiteral(word)) {
        return createJSONLiteral(word);
    }
    if (isBooleanLiteral(word)) {
        return createBooleanLiteral(word == "TRUE" || word == "true");
    }
    if (isNullLiteral(word)) {
        return createNullLiteral();
    }
    if (isFloatLiteral(word)) {
        return createFloatLiteral(std::stod(word));
    }
    if (isIntegerLiteral(word)) {
        return createIntegerLiteral(std::stoll(word));
    }
    if (isCharLiteral(word)) {
        return createCharLiteral(word[1]);
    }
    if (isStringLiteral(word)) {
        return createStringLiteral(word.substr(1, word.length() - 2));
    }
    if (isArrayLiteral(word)) {
        std::string section = (word.rfind("ARRAY", 0) == 0 || word.rfind("array", 0) == 0)
            ? word.substr(5)
            : word;
        auto elems = parseArrayElements(section);
        return createArrayLiteral(elems);
    }
    if (isUUIDLiteral(word)) {
        return createUUIDLiteral(word);
    }
    if (isXMLLiteral(word)) {
        return createXMLLiteral(word);
    }

    return nullptr;
}

/**
 * @brief Creates a literal category token based on the input word.
 * @param word The input word to check against literalCategoryMap
 * @return Token pointer if word is a recognized literal category, nullptr otherwise
 */
std::unique_ptr<Token> Lexer::createLiteralCategoryToken(const std::string& word) {
    auto it = literalCategoryMap.find(word);
    if (it != literalCategoryMap.end()) {
        return std::make_unique<LiteralCategoryToken>(it->second, word, static_cast<int>(pos));
    }
    return nullptr;
}

// === Literal Creation Helpers ===

/**
 * @brief Creates a string literal token from a quoted or dollar-quoted string.
 * @param word The full string literal, including delimiters (quotes or dollar tags).
 * @return Token pointer for the string literal.
 * @details
 * Handles both standard quoted strings ('...' or "...") and PostgreSQL-style dollar-quoted strings ($tag$...$tag$).
 * Extracts the inner content and validates length. Throws on invalid format.
 */
std::unique_ptr<Token> Lexer::createStringLiteral(const std::string& word) {
    std::string value;
    if (word.length() >= 2 && ((word[0] == '\'' && word.back() == '\'') || (word[0] == '"' && word.back() == '"'))) {
        value = word.substr(1, word.length() - 2);
    }
    else if (config.allowDollarQuotes && isDollarQuotedString(word)) {
        size_t tagEnd = word.find('$', 1);
        std::string tag = word.substr(0, tagEnd + 1);
        value = word.substr(tag.length(), word.length() - 2 * tag.length());
    }
    else {
        throwError("Invalid string literal", word);
    }
    if (value.length() > config.maxStringLength) {
        throwError("String literal exceeds maximum length", value);
    }
    return std::make_unique<LiteralToken>(
        LiteralCategory::STRING,
        word,
        std::make_shared<StringLiteralValue>(value)
    );
}

/**
 * @brief Creates an escape string literal token
 * @param value The string content without E'' quotes
 * @return Token pointer for the escape string literal
 */
std::unique_ptr<Token> Lexer::createEscapeStringLiteral(const std::string& value) {
    if (value.length() > config.maxStringLength) {
        throwError("Escape string literal exceeds maximum length", value);
    }
    return std::make_unique<LiteralToken>(
        LiteralCategory::ESCAPE_STRING,
        "E'" + value + "'",
        std::make_shared<EscapeStringLiteralValue>(value)
    );
}

/**
 * @brief Creates a character literal token
 * @param value The character value
 * @return Token pointer for the character literal
 */
std::unique_ptr<Token> Lexer::createCharLiteral(char value) {
    return std::make_unique<LiteralToken>(
        LiteralCategory::CHAR,
        std::string("'") + value + "'",
        std::make_shared<CharLiteralValue>(value)
    );
}

/**
 * @brief Creates an integer literal token
 * @param value The integer value
 * @return Token pointer for the integer literal
 */
std::unique_ptr<Token> Lexer::createIntegerLiteral(int64_t value) {
    return std::make_unique<LiteralToken>(
        LiteralCategory::INTEGER,
        std::to_string(value),
        std::make_shared<IntegerLiteralValue>(value)
    );
}

/**
 * @brief Creates a float literal token
 * @param value The float value
 * @return Token pointer for the float literal
 */
std::unique_ptr<Token> Lexer::createFloatLiteral(double value) {
    return std::make_unique<LiteralToken>(
        LiteralCategory::FLOAT,
        std::to_string(value),
        std::make_shared<FloatLiteralValue>(value)
    );
}

/**
 * @brief Creates a binary literal token
 * @param value The binary string without B'' quotes
 * @return Token pointer for the binary literal
 */
std::unique_ptr<Token> Lexer::createBinaryLiteral(const std::string& value) {
    if (!std::all_of(value.begin(), value.end(), [](char c) { return c == '0' || c == '1'; })) {
        throwError("Invalid binary literal", value);
    }
    return std::make_unique<LiteralToken>(
        LiteralCategory::BINARY,
        "B'" + value + "'",
        std::make_shared<BinaryLiteralValue>(value)
    );
}

/**
 * @brief Creates a hexadecimal literal token
 * @param value The hex string without X'' quotes
 * @return Token pointer for the hex literal
 */
std::unique_ptr<Token> Lexer::createHexLiteral(const std::string& value) {
    if (!std::all_of(value.begin(), value.end(), [](char c) {
        return std::isxdigit(static_cast<unsigned char>(c));
        })) {
        throwError("Invalid hexadecimal literal", value);
    }
    return std::make_unique<LiteralToken>(
        LiteralCategory::HEX,
        "X'" + value + "'",
        std::make_shared<HexLiteralValue>(value)
    );
}

/**
 * @brief Creates a boolean literal token
 * @param value The boolean value
 * @return Token pointer for the boolean literal
 */
std::unique_ptr<Token> Lexer::createBooleanLiteral(bool value) {
    return std::make_unique<LiteralToken>(
        LiteralCategory::BOOLEAN,
        value ? "TRUE" : "FALSE",
        std::make_shared<BooleanLiteralValue>(value)
    );
}

/**
 * @brief Creates a NULL literal token
 * @return Token pointer for the NULL literal
 */
std::unique_ptr<Token> Lexer::createNullLiteral() {
    return std::make_unique<LiteralToken>(
        LiteralCategory::NULL_VALUE,
        "NULL",
        std::make_shared<NullLiteralValue>()
    );
}

/**
 * @brief Creates a date literal token
 * @param value The date string in YYYY-MM-DD format
 * @return Token pointer for the date literal
 */
std::unique_ptr<Token> Lexer::createDateLiteral(const std::string& value) {
    if (!validateDate(value)) {
        throwError("Invalid date format", value);
    }
    return std::make_unique<LiteralToken>(
        LiteralCategory::DATE,
        "'" + value + "'",
        std::make_shared<DateLiteralValue>(value)
    );
}

/**
 * @brief Creates a time literal token
 * @param value The time string in HH:MM:SS format
 * @return Token pointer for the time literal
 */
std::unique_ptr<Token> Lexer::createTimeLiteral(const std::string& value) {
    if (!validateTime(value)) {
        throwError("Invalid time format", value);
    }
    return std::make_unique<LiteralToken>(
        LiteralCategory::TIME,
        "'" + value + "'",
        std::make_shared<TimeLiteralValue>(value)
    );
}

/**
 * @brief Creates a datetime literal token
 * @param value The datetime string in YYYY-MM-DD HH:MM:SS format
 * @return Token pointer for the datetime literal
 */
std::unique_ptr<Token> Lexer::createDateTimeLiteral(const std::string& value) {
    if (!validateDateTime(value)) {
        throwError("Invalid datetime format", value);
    }
    return std::make_unique<LiteralToken>(
        LiteralCategory::DATETIME,
        "'" + value + "'",
        std::make_shared<DateTimeLiteralValue>(value)
    );
}

/**
 * @brief Creates a standard SQL interval literal token.
 * @param value Interval value string (e.g., "2" or "2-3").
 * @param fromUnitStr Starting unit as string (e.g., "YEAR").
 * @param toUnitStr Ending unit as string (optional, e.g., "MONTH").
 * @return Unique pointer to LiteralToken for the interval.
 */
std::unique_ptr<Token> Lexer::createStandardIntervalLiteral(
    const std::string& value,
    const std::string& fromUnitStr,
    const std::string& toUnitStr
) {
    using IntervalLiteralValue = ::IntervalLiteralValue;
    IntervalLiteralValue::Unit fromUnit = IntervalLiteralValue::unitFromString(fromUnitStr);
    IntervalLiteralValue::Unit toUnit = fromUnit;
    if (!toUnitStr.empty()) {
        toUnit = IntervalLiteralValue::unitFromString(toUnitStr);
    }
    return std::make_unique<LiteralToken>(
        LiteralCategory::INTERVAL,
        "INTERVAL '" + value + "' " + fromUnitStr + (toUnitStr.empty() ? "" : " TO " + toUnitStr),
        std::make_shared<IntervalLiteralValue>(value, fromUnit, toUnit)
    );
}

/**
 * @brief Creates a PostgreSQL-style interval literal token.
 * @param value The interval string (e.g., "2 years 3 months").
 * @return Unique pointer to LiteralToken for the interval.
 */
std::unique_ptr<Token> Lexer::createPostgresIntervalLiteral(
    const std::string& value
) {
    return std::make_unique<LiteralToken>(
        LiteralCategory::INTERVAL,
        "INTERVAL '" + value + "'",
        std::make_shared<IntervalLiteralValue>(value)
    );
}

/**
 * @brief Creates an array literal token
 * @param value The array string including brackets
 * @return Token pointer for the array literal
 */
std::unique_ptr<Token> Lexer::createArrayLiteral(const std::vector<std::shared_ptr<LiteralValue>>& elems) {
    if (!config.allowArrayLiterals) {
        throwError("Array literals are not enabled");
    }
    return std::make_unique<LiteralToken>(
        LiteralCategory::ARRAY,
        ArrayLiteralValue::elementsToString(elems),
        std::make_shared<ArrayLiteralValue>(elems)
    );
}

/**
 * @brief Creates a UUID literal token
 * @param value The UUID string
 * @return Token pointer for the UUID literal
 */
std::unique_ptr<Token> Lexer::createUUIDLiteral(const std::string& value) {
    if (!config.allowUUIDLiterals) {
        throwError("UUID literals are not enabled");
    }
    if (!validateUUID(value)) {
        throwError("Invalid UUID format", value);
    }
    return std::make_unique<LiteralToken>(
        LiteralCategory::UUID,
        value,
        std::make_shared<UUIDLiteralValue>(value)
    );
}

/**
 * @brief Creates a JSON literal token
 * @param value The JSON string including braces
 * @return Token pointer for the JSON literal
 */
std::unique_ptr<Token> Lexer::createJSONLiteral(const std::string& value) {
    if (!config.allowJSONLiterals) {
        throwError("JSON literals are not enabled");
    }
    if (!validateJSON(value)) {
        throwError("Invalid JSON format", value);
    }
    return std::make_unique<LiteralToken>(
        LiteralCategory::JSON,
        value,
        std::make_shared<JSONLiteralValue>(value)
    );
}

/**
 * @brief Creates an XML literal token
 * @param value The XML string including tags
 * @return Token pointer for the XML literal
 */
std::unique_ptr<Token> Lexer::createXMLLiteral(const std::string& value) {
    if (!config.allowXMLLiterals) {
        throwError("XML literals are not enabled");
    }
    if (!validateXML(value)) {
        throwError("Invalid XML format", value);
    }
    return std::make_unique<LiteralToken>(
        LiteralCategory::XML,
        value,
        std::make_shared<XMLLiteralValue>(value)
    );
}

// === Validation Methods ===

/**
 * @brief Validates a regular identifier string.
 * @param ident The identifier to validate
 * @return true if the identifier is valid, supports @, @@, #, ## prefixes
 */
bool Lexer::validateIdentifier(const std::string& ident) const {
    if (ident.empty()) {
        return false;
    }

    // Determine prefix length: '@@' or '##' take two, single '@' or '#' take one
    size_t prefixLen = 0;
    if (ident.size() >= 2 && ident[0] == '@' && ident[1] == '@') {
        prefixLen = 2;
    }
    else if (ident.size() >= 2 && ident[0] == '#' && ident[1] == '#') {
        prefixLen = 2;
    }
    else if (ident[0] == '@' || ident[0] == '#') {
        prefixLen = 1;
    }

    // Must have at least one character of actual name
    if (ident.size() <= prefixLen) {
        return false;
    }

    // First character of the name must be a letter or underscore
    char first = ident[prefixLen];
    if (!(std::isalpha(static_cast<unsigned char>(first)) || first == '_')) {
        return false;
    }

    // Remaining characters must be letters, digits, underscore or dollar
    for (size_t i = prefixLen + 1; i < ident.size(); ++i) {
        if (!isIdentifierPart(ident[i])) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Validate the inner content of a delimited identifier (quoted/bracketed).
 * @param ident The identifier content (no delimiters)
 * @return true if valid (supports @, @@, #, ## prefixes; spaces allowed inside name)
 */
bool Lexer::validateDelimitedIdentifier(const std::string& ident) const {
    if (ident.empty()) {
        return false;
    }

    // Skip leading spaces
    size_t i = 0;
    while (i < ident.size() && ident[i] == ' ') {
        ++i;
    }

    // Determine prefix length inside the quotes/brackets
    size_t prefixLen = 0;
    if (i + 1 < ident.size() && ident[i] == '@' && ident[i + 1] == '@') {
        prefixLen = 2;
    }
    else if (i + 1 < ident.size() && ident[i] == '#' && ident[i + 1] == '#') {
        prefixLen = 2;
    }
    else if (ident[i] == '@' || ident[i] == '#') {
        prefixLen = 1;
    }

    // Must have at least one name character
    if (ident.size() - i <= prefixLen) {
        return false;
    }

    // First character of the name must be a letter or underscore
    i += prefixLen;
    if (!(std::isalpha(static_cast<unsigned char>(ident[i])) || ident[i] == '_')) {
        return false;
    }

    // Remaining may be spaces or identifier parts
    for (++i; i < ident.size(); ++i) {
        char c = ident[i];
        if (c == ' ') continue;
        if (!isIdentifierPart(c)) return false;
    }
    return true;
}

/**
 * @brief Validates a string literal
 * @param str The string to validate
 * @return true if the string is valid
 */
bool Lexer::validateString(const std::string& str) const {
    if (str.length() > config.maxStringLength) {
        return false;
    }

    // Check for unescaped control characters
    for (size_t i = 0; i < str.length(); ++i) {
        if (static_cast<unsigned char>(str[i]) < 32) {
            if (i == 0 || str[i - 1] != '\\') {
                return false;
            }
        }
    }

    return true;
}

/**
 * @brief Validates a number string (integer or float)
 * @param num The number string to validate
 * @return true if the number is valid
 */
bool Lexer::validateNumber(const std::string& num) const {
    if (num.empty()) {
        return false;
    }

    size_t pos = 0;
    bool hasSign = (num[0] == '-' || num[0] == '+');
    if (hasSign) pos++;

    bool hasDecimal = false;
    bool hasExponent = false;
    bool hasDigits = false;

    for (; pos < num.length(); ++pos) {
        char c = num[pos];

        if (std::isdigit(c)) {
            hasDigits = true;
            continue;
        }

        if (c == '.') {
            if (hasDecimal || hasExponent) return false;
            hasDecimal = true;
            continue;
        }

        if (c == 'e' || c == 'E') {
            if (hasExponent || !hasDigits) return false;
            hasExponent = true;

            // Handle exponent sign
            if (pos + 1 < num.length() && (num[pos + 1] == '+' || num[pos + 1] == '-')) {
                pos++;
            }
            continue;
        }

        return false;
    }

    return hasDigits;
}

/**
 * @brief Validates a C-style binary literal string
 * @param binLiteral The binary literal including 0b prefix
 * @return true if the binary literal is valid
 */
bool Lexer::validateCStyleBinary(const std::string& binLiteral) const {
    // must start with "0b" or "0B" and have at least one digit
    if (binLiteral.size() <= 2 || !(binLiteral[0] == '0' && (binLiteral[1] == 'b' || binLiteral[1] == 'B'))) {
        return false;
    }
    for (size_t i = 2; i < binLiteral.size(); ++i) {
        if (binLiteral[i] != '0' && binLiteral[i] != '1') {
            return false;
        }
    }
    return true;
}

/**
 * @brief Validates a C-style hexadecimal literal string
 * @param hexLiteral The hexadecimal literal including 0x prefix
 * @return true if the hex literal is valid
 */
bool Lexer::validateCStyleHex(const std::string& hexLiteral) const {
    // must start with "0x" or "0X" and have at least one digit
    if (hexLiteral.size() <= 2 || !(hexLiteral[0] == '0' && (hexLiteral[1] == 'x' || hexLiteral[1] == 'X'))) {
        return false;
    }
    for (size_t i = 2; i < hexLiteral.size(); ++i) {
        if (!std::isxdigit(static_cast<unsigned char>(hexLiteral[i]))) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Validates a SQL-style binary literal string (B'...')
 * @param binLiteral The binary literal including B' prefix and trailing '
 * @return true if the binary literal is valid
 */
bool Lexer::validateBinary(const std::string& binLiteral) const {
    // must start with B' or b' and end with '
    if (binLiteral.size() <= 3 || !(binLiteral[0] == 'B' || binLiteral[0] == 'b') || binLiteral[1] != '\'' || binLiteral.back() != '\'') return false;
    for (size_t i = 2; i + 1 < binLiteral.size(); ++i) if (binLiteral[i] != '0' && binLiteral[i] != '1') return false;
    return (binLiteral.size() > 3);
}

/**
 * @brief Validates a SQL-style hexadecimal literal string (X'...')
 * @param hexLiteral The hexadecimal literal including X' prefix and trailing '
 * @return true if the hex literal is valid
 */
bool Lexer::validateHex(const std::string& hexLiteral) const {
    // must start with X' or x' and end with '
    if (hexLiteral.size() <= 3 || !(hexLiteral[0] == 'X' || hexLiteral[0] == 'x') || hexLiteral[1] != '\'' || hexLiteral.back() != '\'') return false;
    for (size_t i = 2; i + 1 < hexLiteral.size(); ++i) if (!std::isxdigit(static_cast<unsigned char>(hexLiteral[i]))) return false;
    return (hexLiteral.size() > 3);
}

/**
 * @brief Validates a date string (YYYY-MM-DD)
 * @param date The date string to validate
 * @return true if the date is valid
 */
bool Lexer::validateDate(const std::string& date) const {
    if (date.length() != 10) {
        return false;
    }

    // Check format YYYY-MM-DD
    if (date[4] != '-' || date[7] != '-') {
        return false;
    }

    try {
        int year = std::stoi(date.substr(0, 4));
        int month = std::stoi(date.substr(5, 2));
        int day = std::stoi(date.substr(8, 2));

        // Basic validation
        if (year < 1 || month < 1 || month > 12 || day < 1) {
            return false;
        }

        // Days in month validation
        static const int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        int maxDays = daysInMonth[month - 1];

        // Handle leap year
        if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)) {
            maxDays = 29;
        }

        return day <= maxDays;
    }
    catch (...) {
        return false;
    }
}

/**
 * @brief Validates a time string (HH:MM:SS)
 * @param time The time string to validate
 * @return true if the time is valid
 */
bool Lexer::validateTime(const std::string& time) const {
    if (time.length() != 8) {
        return false;
    }

    // Check format HH:MM:SS
    if (time[2] != ':' || time[5] != ':') {
        return false;
    }

    try {
        int hour = std::stoi(time.substr(0, 2));
        int minute = std::stoi(time.substr(3, 2));
        int second = std::stoi(time.substr(6, 2));

        return hour >= 0 && hour < 24 &&
            minute >= 0 && minute < 60 &&
            second >= 0 && second < 60;
    }
    catch (...) {
        return false;
    }
}

/**
 * @brief Validates a datetime string (YYYY-MM-DD HH:MM:SS)
 * @param datetime The datetime string to validate
 * @return true if the datetime is valid
 */
bool Lexer::validateDateTime(const std::string& datetime) const {
    if (datetime.length() != 19) {
        return false;
    }

    // Split into date and time components
    if (datetime[10] != ' ') {
        return false;
    }

    return validateDate(datetime.substr(0, 10)) &&
        validateTime(datetime.substr(11));
}

/**
 * @brief Validates a full SQL interval literal string (supports both standard and PostgreSQL-style).
 * @param interval The full interval literal string (e.g. "INTERVAL '2-3' YEAR TO MONTH" or "INTERVAL '2 years 3 months'")
 * @return true if the interval is valid, false otherwise.
 */
bool Lexer::validateInterval(const std::string& interval) const {
    // Extract quoted value and trailing units
    std::smatch m;
    std::regex rx(R"(^\s*INTERVAL\s+'([^']+)'\s*([A-Za-z]*)\s*(TO\s+([A-Za-z]+))?\s*$)", std::regex::icase);
    if (std::regex_match(interval, m, rx)) {
        std::string value = m[1];
        std::string fromUnitStr = m[2];
        std::string toUnitStr = m[4];

        if (fromUnitStr.empty()) {
            // PostgreSQL-style: "INTERVAL '2 years 3 months'"
            return validatePostgresInterval(value);
        }
        else {
            // Standard SQL: "INTERVAL '2' YEAR" or "INTERVAL '2-3' YEAR TO MONTH"
            return validateStandardInterval(value, fromUnitStr, toUnitStr);
        }
    }
    // If doesn't match pattern, not valid
    return false;
}

/**
 * @brief Validates a standard SQL interval.
 * @param value The interval value string (e.g. "2" or "2-3").
 * @param fromUnitStr The starting unit as string (e.g. "YEAR").
 * @param toUnitStr The ending unit as string (optional).
 * @return true if the interval is valid, false otherwise.
 */
bool Lexer::validateStandardInterval(
    const std::string& value,
    const std::string& fromUnitStr,
    const std::string& toUnitStr
) const {
    using IntervalLiteralValue = ::IntervalLiteralValue;
    // Validate units
    IntervalLiteralValue::Unit fromUnit, toUnit;
    try {
        fromUnit = IntervalLiteralValue::unitFromString(fromUnitStr);
        toUnit = fromUnit;
        if (!toUnitStr.empty()) {
            toUnit = IntervalLiteralValue::unitFromString(toUnitStr);
        }
        if (fromUnit == IntervalLiteralValue::Unit::UNKNOWN ||
            (!toUnitStr.empty() && toUnit == IntervalLiteralValue::Unit::UNKNOWN)) {
            return false;
        }
    }
    catch (...) {
        return false;
    }
    // Try constructing IntervalLiteralValue to trigger parsing/validation
    try {
        IntervalLiteralValue test(value, fromUnit, toUnit);
        return test.isValid();
    }
    catch (...) {
        return false;
    }
}

/**
 * @brief Validates a PostgreSQL-style interval string (e.g. "2 years 3 months").
 * @param value The interval value string.
 * @return true if the interval is valid, false otherwise.
 */
bool Lexer::validatePostgresInterval(const std::string& value) const {
    using IntervalLiteralValue = ::IntervalLiteralValue;
    try {
        IntervalLiteralValue test(value);
        return test.isValid();
    }
    catch (...) {
        return false;
    }
}

/**
 * @brief Validates a UUID string
 * @param uuid The UUID string to validate
 * @return true if the UUID is valid
 */
bool Lexer::validateUUID(const std::string& uuid) const {
    if (!config.allowUUIDLiterals) {
        return false;
    }

    if (uuid.length() != 36) {
        return false;
    }

    // Check hyphens at correct positions
    if (uuid[8] != '-' || uuid[13] != '-' || uuid[18] != '-' || uuid[23] != '-') {
        return false;
    }

    // Check that all other characters are hexadecimal
    for (size_t i = 0; i < uuid.length(); ++i) {
        if (i == 8 || i == 13 || i == 18 || i == 23) continue;
        if (!std::isxdigit(static_cast<unsigned char>(uuid[i]))) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Validates a JSON string
 * @param json The JSON string to validate
 * @return true if the JSON is valid
 */
bool Lexer::validateJSON(const std::string& json) const {
    if (!config.allowJSONLiterals) {
        return false;
    }

    try {
        nlohmann::json::parse(json);
        return true;
    }
    catch (...) {
        return false;
    }
}

/**
 * @brief Validates an XML string
 * @param xml The XML string to validate
 * @return true if the XML is valid
 */
bool Lexer::validateXML(const std::string& xml) const {
    if (!config.allowXMLLiterals) {
        return false;
    }

    try {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_string(xml.c_str());
        return result;
    }
    catch (...) {
        return false;
    }
}

/**
 * @brief Validates an array literal string
 * @param array The array string to validate
 * @return true if the array is valid
 */
bool Lexer::validateArray(const std::string& array) const {
    if (!config.allowArrayLiterals) {
        return false;
    }

    if (array.empty() || array[0] != '[' || array.back() != ']') {
        return false;
    }

    int bracketCount = 0;
    bool inString = false;
    char stringChar = 0;

    for (size_t i = 0; i < array.length(); ++i) {
        char c = array[i];

        if (inString) {
            if (c == stringChar && array[i - 1] != '\\') {
                inString = false;
            }
            continue;
        }

        switch (c) {
        case '[':
            bracketCount++;
            break;
        case ']':
            bracketCount--;
            if (bracketCount < 0) return false;
            break;
        case '\'':
        case '"':
            inString = true;
            stringChar = c;
            break;
        case ',':
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            break;
        default:
            if (!std::isalnum(c) && c != '.' && c != '-' && c != '+') {
                return false;
            }
        }
    }

    return bracketCount == 0 && !inString;
}

// === Character Classification Methods ===

/**
 * @brief Checks if a character can start a word-like token (keyword, function, operator)
 * @param c Character to check
 * @return true if letter
 */
bool Lexer::isWordStart(char c) const {
    return std::isalpha(static_cast<unsigned char>(c));
}

/**
 * @brief Checks if a character can be part of a word-like token
 * @param c Character to check
 * @return true if letter, digit, or underscore
 */
bool Lexer::isWordPart(char c) const {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

/**
 * @brief Checks if a character can start a regular (non-delimited) identifier
 * @param c Character to check
 * @return true if it can start (letter, underscore, @, #)
 */
bool Lexer::isIdentifierStart(char c) const {
    return std::isalpha(static_cast<unsigned char>(c))
        || c == '_' || c == '@' || c == '#';
}

/**
 * @brief Checks if a character can be part of a regular identifier
 * @param c Character to check
 * @return true if allowed (letter, digit, underscore, $)
 */
bool Lexer::isIdentifierPart(char c) const {
    return std::isalnum(static_cast<unsigned char>(c))
        || c == '_' || c == '$';
}

/**
 * @brief Checks if a character is a string delimiter
 * @param c Character to check
 * @return true if character is a string delimiter
 */
bool Lexer::isStringDelimiter(char c) const {
    return c == '\'' || c == '"' || c == '`';
}

/**
 * @brief Checks if a character can start a number
 * @param c Character to check
 * @return true if character can start a number
 */
bool Lexer::isNumberStart(char c) const {
    return std::isdigit(static_cast<unsigned char>(c)) || c == '-' || c == '+' || c == '.';
}

/**
 * @brief Checks if a character is a valid operator symbol
 * @param c Character to check
 * @return true if character is an operator symbol
 */
bool Lexer::isOperatorChar(char c) const {
    return std::string("=!<>+-*/%^&|~#?@:").find(c) != std::string::npos;
}

/**
 * @brief Checks if a character is a valid hex digit
 * @param c Character to check
 * @return true if character is a hex digit
 */
bool Lexer::isHexDigit(char c) const {
    return std::isxdigit(static_cast<unsigned char>(c));
}

/**
 * @brief Checks if a character is a valid binary digit
 * @param c Character to check
 * @return true if character is a binary digit
 */
bool Lexer::isBinaryDigit(char c) const {
    return c == '0' || c == '1';
}

/**
 * @brief Checks if a character is a decimal point
 * @param c Character to check
 * @return true if character is a decimal point
 */
bool Lexer::isDecimalPoint(char c) const {
    return c == '.';
}

/**
 * @brief Checks if a character is a valid exponent marker
 * @param c Character to check
 * @return true if character is an exponent marker
 */
bool Lexer::isExponentMarker(char c) const {
    return c == 'e' || c == 'E';
}

/**
 * @brief Checks if a character is a sign (+ or -)
 * @param c Character to check
 * @return true if character is a sign
 */
bool Lexer::isSign(char c) const {
    return c == '+' || c == '-';
}

/**
 * @brief Checks if a character is a whitespace character
 * @param c Character to check
 * @return true if character is whitespace
 */
bool Lexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f' || c == '\v';
}

/**
 * @brief Checks if a character is a newline character
 * @param c Character to check
 * @return true if character is a newline
 */
bool Lexer::isNewline(char c) const {
    return c == '\n' || c == '\r';
}

/**
 * @brief Checks if a character can be part of a special name
 * @param c Character to check
 * @return true if character can be part of a special name
 */
bool Lexer::isSpecialNameChar(char c) const {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '$' || c == '#';
}

/**
 * @brief Checks if a character is valid in a quoted identifier
 * @param c Character to check
 * @return true if character is valid in a quoted identifier
 */
bool Lexer::isQuotedIdentifierChar(char c) const {
    // All characters are valid in quoted identifiers except the delimiter itself
    // and control characters
    return c >= 32 && c <= 126;
}

/**
 * @brief Checks if a character needs to be escaped in strings
 * @param c Character to check
 * @return true if character needs escaping
 */
bool Lexer::needsEscaping(char c) const {
    return c == '\'' || c == '"' || c == '\\' || c == '\n' || c == '\r' ||
        c == '\t' || c == '\b' || c == '\f';
}

/**
 * @brief Gets the escaped version of a character
 * @param c Character to escape
 * @return Escaped string representation of the character
 */
std::string Lexer::getEscapedChar(char c) const {
    switch (c) {
    case '\'': return "\\'";
    case '"': return "\\\"";
    case '\\': return "\\\\";
    case '\n': return "\\n";
    case '\r': return "\\r";
    case '\t': return "\\t";
    case '\b': return "\\b";
    case '\f': return "\\f";
    default: return std::string(1, c);
    }
}

/**
 * @brief Gets the unescaped version of an escape sequence
 * @param sequence The escape sequence (including backslash)
 * @return Unescaped character or empty string if invalid
 */
std::string Lexer::getUnescapedChar(const std::string& sequence) const {
    if (sequence.length() < 2 || sequence[0] != '\\') {
        return "";
    }

    char c = sequence[1];
    switch (c) {
    case 'n': return "\n";
    case 'r': return "\r";
    case 't': return "\t";
    case 'b': return "\b";
    case 'f': return "\f";
    case '\\': return "\\";
    case '\'': return "'";
    case '"': return "\"";
    case 'u': // Unicode escape
        if (sequence.length() >= 6) {
            try {
                int codepoint = std::stoi(sequence.substr(2, 4), nullptr, 16);
                // Convert codepoint to UTF-8
                return convertCodepointToUTF8(codepoint);
            }
            catch (...) {
                return "";
            }
        }
        return "";
    default:
        return "";
    }
}

/**
 * @brief Converts a Unicode codepoint to UTF-8 string
 * @param codepoint Unicode codepoint
 * @return UTF-8 encoded string
 */
std::string Lexer::convertCodepointToUTF8(int codepoint) const {
    std::string result;

    if (codepoint <= 0x7F) {
        result.push_back(static_cast<char>(codepoint));
    }
    else if (codepoint <= 0x7FF) {
        result.push_back(static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F)));
        result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    }
    else if (codepoint <= 0xFFFF) {
        result.push_back(static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F)));
        result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    }
    else if (codepoint <= 0x10FFFF) {
        result.push_back(static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07)));
        result.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    }

    return result;
}

// === Literal Classification Methods ===

/**
 * @brief Checks if string is an escape string literal (E'...')
 * @param word String to check
 * @return true if string is an escape string literal
 */
bool Lexer::isEscapeStringLiteral(const std::string& word) const {
    return word.length() >= 4 &&
        (word[0] == 'E' || word[0] == 'e') &&
        word[1] == '\'' &&
        word.back() == '\'';
}

/**
 * @brief Checks if string is a binary literal (B'0101')
 * @param word String to check
 * @return true if string is a binary literal
 */
bool Lexer::isBinaryLiteral(const std::string& word) const {
    if (word.length() < 4 || (word[0] != 'B' && word[0] != 'b') ||
        word[1] != '\'' || word.back() != '\'') {
        return false;
    }

    return std::all_of(word.begin() + 2, word.end() - 1,
        [](char c) { return c == '0' || c == '1'; });
}

/**
 * @brief Checks if string is a hex literal (X'1A2B')
 * @param word String to check
 * @return true if string is a hex literal
 */
bool Lexer::isHexLiteral(const std::string& word) const {
    if (word.length() < 4 || (word[0] != 'X' && word[0] != 'x') ||
        word[1] != '\'' || word.back() != '\'') {
        return false;
    }

    return std::all_of(word.begin() + 2, word.end() - 1,
        [](char c) { return std::isxdigit(static_cast<unsigned char>(c)); });
}

/**
 * @brief Checks if string is a datetime literal ('YYYY-MM-DD HH:MM:SS')
 * @param word String to check
 * @return true if string is a datetime literal
 */
bool Lexer::isDateTimeLiteral(const std::string& word) const {
    if (word.length() != 21 || word[0] != '\'' || word.back() != '\'') {
        return false;
    }

    std::string content = word.substr(1, 19);
    return validateDateTime(content);
}

/**
 * @brief Checks if string is a date literal ('YYYY-MM-DD')
 * @param word String to check
 * @return true if string is a date literal
 */
bool Lexer::isDateLiteral(const std::string& word) const {
    if (word.length() != 12 || word[0] != '\'' || word.back() != '\'') {
        return false;
    }

    std::string content = word.substr(1, 10);
    return validateDate(content);
}

/**
 * @brief Checks if string is a time literal ('HH:MM:SS')
 * @param word String to check
 * @return true if string is a time literal
 */
bool Lexer::isTimeLiteral(const std::string& word) const {
    if (word.length() != 10 || word[0] != '\'' || word.back() != '\'') {
        return false;
    }

    std::string content = word.substr(1, 8);
    return validateTime(content);
}

/**
 * @brief Checks if string is a JSON literal ({...})
 * @param word String to check
 * @return true if string is a JSON literal
 */
bool Lexer::isJSONLiteral(const std::string& word) const {
    return word.length() >= 2 && word[0] == '{' && word.back() == '}' &&
        validateJSON(word);
}

/**
 * @brief Checks if string is a boolean literal (TRUE or FALSE)
 * @param word String to check
 * @return true if string is a boolean literal
 */
bool Lexer::isBooleanLiteral(const std::string& word) const {
    std::string upper = word;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    return upper == "TRUE" || upper == "FALSE";
}

/**
 * @brief Checks if string is a NULL literal
 * @param word String to check
 * @return true if string is a NULL literal
 */
bool Lexer::isNullLiteral(const std::string& word) const {
    std::string upper = word;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    return upper == "NULL";
}

/**
 * @brief Checks if string is a float literal
 * @param word String to check
 * @return true if string is a float literal
 */
bool Lexer::isFloatLiteral(const std::string& word) const {
    if (word.empty()) return false;

    bool hasDecimal = false;
    bool hasDigit = false;
    size_t i = 0;

    // Handle sign
    if (word[0] == '-' || word[0] == '+') i++;

    for (; i < word.length(); i++) {
        if (std::isdigit(word[i])) {
            hasDigit = true;
        }
        else if (word[i] == '.') {
            if (hasDecimal) return false;
            hasDecimal = true;
        }
        else {
            return false;
        }
    }

    return hasDigit && hasDecimal;
}

/**
 * @brief Checks if string is an integer literal
 * @param word String to check
 * @return true if string is an integer literal
 */
bool Lexer::isIntegerLiteral(const std::string& word) const {
    if (word.empty()) return false;

    size_t i = 0;
    if (word[0] == '-' || word[0] == '+') i++;
    if (i == word.length()) return false;

    return std::all_of(word.begin() + i, word.end(),
        [](char c) { return std::isdigit(static_cast<unsigned char>(c)); });
}

/**
 * @brief Checks if string is a character literal ('A')
 * @param word String to check
 * @return true if string is a character literal
 */
bool Lexer::isCharLiteral(const std::string& word) const {
    return word.length() == 3 && word[0] == '\'' && word[2] == '\'';
}

/**
 * @brief Checks if string is a string literal ("...", '...', or dollar-quoted)
 * @param word String to check
 * @return true if string is a string literal
 */
bool Lexer::isStringLiteral(const std::string& word) const {
    if (word.length() < 2) return false;

    char first = word[0];
    char last = word.back();

    // Standard single or double quoted string
    if ((first == '\'' && last == '\'') ||
        (first == '"' && last == '"')) {
        return validateString(word.substr(1, word.length() - 2));
    }

    // Dollar-quoted string (PostgreSQL-style)
    if (config.allowDollarQuotes && isDollarQuotedString(word)) {
        return true;
    }

    return false;
}

/**
 * @brief Checks if string is an array literal (either [... ] or ARRAY[...] ).
 * @param word String to check
 * @return true if string matches array literal syntax
 */
bool Lexer::isArrayLiteral(const std::string& word) const {
    // Must end with ']'
    if (word.size() < 2 || word.back() != ']') {
        return false;
    }

    // Bare bracket syntax: "[...]"
    if (word.front() == '[') {
        return validateArray(word);
    }

    // ARRAY prefix syntax: "ARRAY[...]" (case-insensitive)
    const std::string upref = "ARRAY";
    if (word.size() > upref.size() &&
        (word.rfind(upref, 0) == 0 || word.rfind("array", 0) == 0) &&
        word[upref.size()] == '[') {
        // Validate only the bracketed part
        return validateArray(word.substr(upref.size()));
    }

    return false;
}

/**
 * @brief Checks if string is a UUID literal
 * @param word String to check
 * @return true if string is a UUID literal
 */
bool Lexer::isUUIDLiteral(const std::string& word) const {
    if (word.length() != 36) return false;

    // Check format: 8-4-4-4-12 hexadecimal digits
    const std::vector<size_t> segments = { 8, 4, 4, 4, 12 };
    size_t pos = 0;

    for (size_t i = 0; i < segments.size(); i++) {
        if (i > 0) {
            if (word[pos] != '-') return false;
            pos++;
        }

        for (size_t j = 0; j < segments[i]; j++) {
            if (!std::isxdigit(static_cast<unsigned char>(word[pos + j]))) {
                return false;
            }
        }
        pos += segments[i];
    }

    return true;
}

/**
 * @brief Checks if string is an XML literal (<...>)
 * @param word String to check
 * @return true if string is an XML literal
 */
bool Lexer::isXMLLiteral(const std::string& word) const {
    return word.length() >= 3 && word[0] == '<' && word.back() == '>' &&
        validateXML(word);
}

// === Error Handling Methods ===

/**
 * @brief Gets current UTC timestamp in YYYY-MM-DD HH:MM:SS format
 * @return Formatted timestamp string
 */
std::string Lexer::getCurrentTimestamp() const{
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_now;
#ifdef _WIN32
    localtime_s(&tm_now, &time_t_now); // Windows-specific
#else
    localtime_r(&time_t_now, &tm_now); // POSIX-specific
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm_now, "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}

/**
 * @brief Throws a LexerError with detailed context information
 * @param message The error message
 * @param token Optional problematic token
 * @throws LexerError
 */
[[noreturn]] void Lexer::throwError(const std::string& message, const std::string& token) const {
    std::ostringstream contextBuilder;

    // Basic error information
    contextBuilder << "SQL Lexer Error at line " << line << ", column " << column << "\n";
    contextBuilder << "Error: " << message << "\n";

    // Add token information if provided
    if (!token.empty()) {
        contextBuilder << "Token: '" << token << "'\n";
    }

    // Add source line with position indicator
    contextBuilder << "\nContext:\n" << getCurrentLine() << "\n"
        << getPositionPointer() << "\n";

    // Add metadata
    contextBuilder << "\nMetadata:\n"
        << "Timestamp: " << getCurrentTimestamp() << "\n"
        << "Position: " << pos << "\n"
        << "Token Index: " << tokenIndex << "\n";

    // Add configuration state if relevant
    if (message.find("disabled") != std::string::npos) {
        contextBuilder << "\nConfiguration:\n"
            << "allowDollarQuotes: " << (config.allowDollarQuotes ? "true" : "false") << "\n"
            << "allowXMLLiterals: " << (config.allowXMLLiterals ? "true" : "false") << "\n"
            << "allowJSONLiterals: " << (config.allowJSONLiterals ? "true" : "false") << "\n"
            << "allowUUIDLiterals: " << (config.allowUUIDLiterals ? "true" : "false") << "\n"
            << "allowArrayLiterals: " << (config.allowArrayLiterals ? "true" : "false") << "\n"
            << "strictMode: " << (config.strictMode ? "true" : "false") << "\n";
    }

    throw LexerError(contextBuilder.str(), pos, token);
}

/**
 * @brief Gets the line content at current position.
 * @return Current line content for error reporting
 */
std::string Lexer::getSourceLine() const {
    // Find start of line
    size_t lineStart = pos;
    while (lineStart > 0 && input[lineStart - 1] != '\n') {
        --lineStart;
    }

    // Find end of line
    size_t lineEnd = pos;
    while (lineEnd < input.size() && input[lineEnd] != '\n') {
        ++lineEnd;
    }

    return input.substr(lineStart, lineEnd - lineStart);
}

/**
 * @brief Creates a pointer to the error position in the line
 * @return String with spaces and a caret (^) pointing to the error
 */
std::string Lexer::getPositionPointer() const {
    // Calculate column in current line
    size_t currentColumn = 0;
    size_t lineStart = pos;
    while (lineStart > 0 && input[lineStart - 1] != '\n') {
        lineStart--;
        currentColumn++;
    }

    return std::string(currentColumn, ' ') + "^";
}

/**
 * @brief Records an error without throwing an exception
 * @param message The error message
 * @param severity The severity level of the error
 */
void Lexer::recordError(const std::string& message, ErrorSeverity severity) {
    ErrorInfo error;
    error.message = message;
    error.position = pos;
    error.line = line;
    error.column = column;
    error.severity = severity;
    error.timestamp = getCurrentTimestamp();
    error.context = getSourceLine();
    error.pointer = getPositionPointer();

    errorLog.push_back(error);
}

/**
 * @brief Gets detailed diagnostic information about the current state
 * @return Diagnostic information string
 */
std::string Lexer::getDiagnostics() const {
    std::ostringstream diag;

    diag << "\n=== Lexer Diagnostics ===\n"
        << "Current Position: " << pos << "\n"
        << "Line: " << line << "\n"
        << "Column: " << column << "\n"
        << "Token Index: " << tokenIndex << "\n"
        << "Tokens Processed: " << tokens.size() << "\n"
        << "Input Length: " << input.size() << "\n"
        << "Configuration:\n"
        << "  - Case Sensitive: " << (config.caseSensitive ? "yes" : "no") << "\n"
        << "  - Strict Mode: " << (config.strictMode ? "yes" : "no") << "\n"
        << "  - Max String Length: " << config.maxStringLength << "\n"
        << "  - Max Identifier Length: " << config.maxIdentLength << "\n"
        << "Error Log: " << errorLog.size() << " entries\n";

    if (!errorLog.empty()) {
        diag << "\nRecent Errors:\n";
        size_t count = 0;
        for (auto it = errorLog.rbegin(); it != errorLog.rend() && count < 5; ++it, ++count) {
            diag << "- " << it->message << " (Line " << it->line
                << ", Col " << it->column << ")\n";
        }
    }

    return diag.str();
}

/**
 * @brief Checks if there are any errors in the error log
 * @param minSeverity Minimum severity level to consider
 * @return true if there are errors at or above the specified severity
 */
bool Lexer::hasErrors(ErrorSeverity minSeverity) const {
    return std::any_of(errorLog.begin(), errorLog.end(),
        [minSeverity](const ErrorInfo& error) {
            return error.severity >= minSeverity;
        });
}

/**
 * @brief Gets the error log
 * @return Vector of error information
 */
const std::vector<ErrorInfo>& Lexer::getErrorLog() const {
    return errorLog;
}

/**
 * @brief Clears the error log
 */
void Lexer::clearErrorLog() {
    errorLog.clear();
}
