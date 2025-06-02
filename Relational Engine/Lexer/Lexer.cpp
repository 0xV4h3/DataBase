/**
 * @file Lexer.cpp
 * @brief Implementation of the SQL Lexer class.
 * @details
 * This file contains the implementation of the Lexer class, which tokenizes
 * a SQL input string into categorized tokens: keywords, functions, identifiers,
 * literals, operators, punctuators, comments, and more. The Lexer is aware of
 * advanced literal types (including EscapeString, Array, UUID, XML, and others)
 * and is extensible for future token categories.
 */

#include "Lexer.hpp"
#include "KeywordInfo.hpp"
#include "FunctionInfo.hpp"
#include "OperatorInfo.hpp"
#include "PunctuatorInfo.hpp"
#include "IdentifierInfo.hpp"
#include <memory>
#include <algorithm>
#include <cctype>
#include <stdexcept>

 /**
  * @section Static Info Maps
  * Static maps for keyword, function, operator, symbol, and DateTimePart recognition.
  */

  /**
   * @var Lexer::dmlMap
   * @brief Map for Data Manipulation Language keywords.
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
 * @var Lexer::ddlMap
 * @brief Map for Data Definition Language keywords.
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
 * @var Lexer::clauseMap
 * @brief Map for clause keywords.
 */
const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> Lexer::clauseMap = {
    {"FROM", std::make_shared<KeywordInfo>("FROM", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::FROM))},
    {"WHERE", std::make_shared<KeywordInfo>("WHERE", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::WHERE))},
    {"JOIN", std::make_shared<KeywordInfo>("JOIN", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::JOIN))},
    {"ON", std::make_shared<KeywordInfo>("ON", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::ON))},
    {"GROUP", std::make_shared<KeywordInfo>("GROUP", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::GROUP))},
    {"BY", std::make_shared<KeywordInfo>("BY", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::BY))},
    {"HAVING", std::make_shared<KeywordInfo>("HAVING", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::HAVING))},
    {"ORDER", std::make_shared<KeywordInfo>("ORDER", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::ORDER))},
    {"UNION", std::make_shared<KeywordInfo>("UNION", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::UNION))},
    {"DISTINCT", std::make_shared<KeywordInfo>("DISTINCT", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::DISTINCT))},
    {"TOP", std::make_shared<KeywordInfo>("TOP", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::TOP))},
    {"LIMIT", std::make_shared<KeywordInfo>("LIMIT", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::LIMIT))},
    {"OFFSET", std::make_shared<KeywordInfo>("OFFSET", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::OFFSET))},
    {"LEFT", std::make_shared<KeywordInfo>("LEFT", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::LEFT))},
    {"RIGHT", std::make_shared<KeywordInfo>("RIGHT", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::RIGHT))},
    {"FULL", std::make_shared<KeywordInfo>("FULL", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::FULL))},
    {"OUTER", std::make_shared<KeywordInfo>("OUTER", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::OUTER))},
    {"INNER", std::make_shared<KeywordInfo>("INNER", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::INNER))},
    {"CROSS", std::make_shared<KeywordInfo>("CROSS", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::CROSS))},
    {"APPLY", std::make_shared<KeywordInfo>("APPLY", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::APPLY))},
    {"WINDOW", std::make_shared<KeywordInfo>("WINDOW", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::WINDOW))},
    {"PARTITION", std::make_shared<KeywordInfo>("PARTITION", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::PARTITION))},
    {"OVER", std::make_shared<KeywordInfo>("OVER", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::OVER))},
    {"AS", std::make_shared<KeywordInfo>("AS", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::AS))},
    {"USING", std::make_shared<KeywordInfo>("USING", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::USING))},
    {"DO", std::make_shared<KeywordInfo>("DO", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::DO))},
    {"END", std::make_shared<KeywordInfo>("END", KeywordCategory::CLAUSE, static_cast<int>(ClauseKeyword::END))}
};

/**
 * @var Lexer::cteMap
 * @brief Map for Common Table Expression keywords.
 */
const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> Lexer::cteMap = {
    {"WITH", std::make_shared<KeywordInfo>("WITH", KeywordCategory::CTE, static_cast<int>(CTEKeyword::WITH))},
    {"RECURSIVE", std::make_shared<KeywordInfo>("RECURSIVE", KeywordCategory::CTE, static_cast<int>(CTEKeyword::RECURSIVE))}
};

/**
 * @var Lexer::setOpMap
 * @brief Map for set operation keywords.
 */
const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> Lexer::setOpMap = {
    {"EXCEPT", std::make_shared<KeywordInfo>("EXCEPT", KeywordCategory::SETOP, static_cast<int>(SetOpKeyword::EXCEPT))},
    {"INTERSECT", std::make_shared<KeywordInfo>("INTERSECT", KeywordCategory::SETOP, static_cast<int>(SetOpKeyword::INTERSECT))},
    {"UNION", std::make_shared<KeywordInfo>("UNION", KeywordCategory::SETOP, static_cast<int>(SetOpKeyword::UNION))}
};

/**
 * @var Lexer::wordOpMap
 * @brief Map for word operator keywords.
 */
const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> Lexer::wordOpMap = {
    {"IN", std::make_shared<KeywordInfo>("IN", KeywordCategory::WORD_OP, static_cast<int>(WordOperatorKeyword::IN))},
    {"IS", std::make_shared<KeywordInfo>("IS", KeywordCategory::WORD_OP, static_cast<int>(WordOperatorKeyword::IS))},
    {"NOT", std::make_shared<KeywordInfo>("NOT", KeywordCategory::WORD_OP, static_cast<int>(WordOperatorKeyword::NOT))},
    {"LIKE", std::make_shared<KeywordInfo>("LIKE", KeywordCategory::WORD_OP, static_cast<int>(WordOperatorKeyword::LIKE))},
    {"BETWEEN", std::make_shared<KeywordInfo>("BETWEEN", KeywordCategory::WORD_OP, static_cast<int>(WordOperatorKeyword::BETWEEN))},
    {"EXISTS", std::make_shared<KeywordInfo>("EXISTS", KeywordCategory::WORD_OP, static_cast<int>(WordOperatorKeyword::EXISTS))},
    {"ALL", std::make_shared<KeywordInfo>("ALL", KeywordCategory::WORD_OP, static_cast<int>(WordOperatorKeyword::ALL))},
    {"ANY", std::make_shared<KeywordInfo>("ANY", KeywordCategory::WORD_OP, static_cast<int>(WordOperatorKeyword::ANY))},
    {"SOME", std::make_shared<KeywordInfo>("SOME", KeywordCategory::WORD_OP, static_cast<int>(WordOperatorKeyword::SOME))},
    {"UNIQUE", std::make_shared<KeywordInfo>("UNIQUE", KeywordCategory::WORD_OP, static_cast<int>(WordOperatorKeyword::UNIQUE))}
};

/**
 * @var Lexer::logicalConstMap
 * @brief Map for logical constant keywords.
 */
const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> Lexer::logicalConstMap = {
    {"NULL", std::make_shared<KeywordInfo>("NULL", KeywordCategory::LOGICAL_CONST, static_cast<int>(LogicalConstantKeyword::NULL_KEYWORD))},
    {"TRUE", std::make_shared<KeywordInfo>("TRUE", KeywordCategory::LOGICAL_CONST, static_cast<int>(LogicalConstantKeyword::TRUE_KEYWORD))},
    {"FALSE", std::make_shared<KeywordInfo>("FALSE", KeywordCategory::LOGICAL_CONST, static_cast<int>(LogicalConstantKeyword::FALSE_KEYWORD))}
};

/**
 * @var Lexer::tranMap
 * @brief Map for transaction control keywords.
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
 * @var Lexer::secMap
 * @brief Map for security-related keywords.
 */
const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> Lexer::secMap = {
    {"GRANT", std::make_shared<KeywordInfo>("GRANT", KeywordCategory::SECURITY, static_cast<int>(SecurityKeyword::GRANT))},
    {"REVOKE", std::make_shared<KeywordInfo>("REVOKE", KeywordCategory::SECURITY, static_cast<int>(SecurityKeyword::REVOKE))},
    {"DENY", std::make_shared<KeywordInfo>("DENY", KeywordCategory::SECURITY, static_cast<int>(SecurityKeyword::DENY))},
    {"ON", std::make_shared<KeywordInfo>("ON", KeywordCategory::SECURITY, static_cast<int>(SecurityKeyword::ON))},
    {"TO", std::make_shared<KeywordInfo>("TO", KeywordCategory::SECURITY, static_cast<int>(SecurityKeyword::TO))}
};

/**
 * @var Lexer::progMap
 * @brief Map for programming statement keywords (procedural, control flow, etc).
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
 * @var Lexer::miscMap
 * @brief Map for miscellaneous keywords.
 */
const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> Lexer::miscMap = {
    {"AS", std::make_shared<KeywordInfo>("AS", KeywordCategory::MISC, static_cast<int>(MiscKeyword::AS))},
    {"CASE", std::make_shared<KeywordInfo>("CASE", KeywordCategory::MISC, static_cast<int>(MiscKeyword::CASE))},
    {"WHEN", std::make_shared<KeywordInfo>("WHEN", KeywordCategory::MISC, static_cast<int>(MiscKeyword::WHEN))},
    {"THEN", std::make_shared<KeywordInfo>("THEN", KeywordCategory::MISC, static_cast<int>(MiscKeyword::THEN))},
    {"ELSE", std::make_shared<KeywordInfo>("ELSE", KeywordCategory::MISC, static_cast<int>(MiscKeyword::ELSE))},
    {"END", std::make_shared<KeywordInfo>("END", KeywordCategory::MISC, static_cast<int>(MiscKeyword::END))},
    {"CAST", std::make_shared<KeywordInfo>("CAST", KeywordCategory::MISC, static_cast<int>(MiscKeyword::CAST))},
    {"CONVERT", std::make_shared<KeywordInfo>("CONVERT", KeywordCategory::MISC, static_cast<int>(MiscKeyword::CONVERT))},
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
 * @var Lexer::aggFuncMap
 * @brief Map for aggregate functions.
 */
const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> Lexer::aggFuncMap = {
    {"COUNT", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, static_cast<int>(AggregateFunction::COUNT), "COUNT")},
    {"SUM", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, static_cast<int>(AggregateFunction::SUM), "SUM")},
    {"AVG", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, static_cast<int>(AggregateFunction::AVG), "AVG")},
    {"MIN", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, static_cast<int>(AggregateFunction::MIN), "MIN")},
    {"MAX", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, static_cast<int>(AggregateFunction::MAX), "MAX")},
    {"GROUP_CONCAT", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, static_cast<int>(AggregateFunction::GROUP_CONCAT), "GROUP_CONCAT")},
    {"ARRAY_AGG", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, static_cast<int>(AggregateFunction::ARRAY_AGG), "ARRAY_AGG")},
    {"LISTAGG", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, static_cast<int>(AggregateFunction::LISTAGG), "LISTAGG")},
    {"STDDEV", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, static_cast<int>(AggregateFunction::STDDEV), "STDDEV")},
    {"VARIANCE", std::make_shared<FunctionInfo>(FunctionCategory::AGGREGATE, static_cast<int>(AggregateFunction::VARIANCE), "VARIANCE")}
};

/**
 * @var Lexer::scalarFuncMap
 * @brief Map for scalar functions.
 */
const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> Lexer::scalarFuncMap = {
    {"CONVERT", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, static_cast<int>(ScalarFunction::CONVERT), "CONVERT")},
    {"CAST", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, static_cast<int>(ScalarFunction::CAST), "CAST")},
    {"COALESCE", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, static_cast<int>(ScalarFunction::COALESCE), "COALESCE")},
    {"NULLIF", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, static_cast<int>(ScalarFunction::NULLIF), "NULLIF")},
    {"IFNULL", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, static_cast<int>(ScalarFunction::IFNULL), "IFNULL")},
    {"LEAST", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, static_cast<int>(ScalarFunction::LEAST), "LEAST")},
    {"GREATEST", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, static_cast<int>(ScalarFunction::GREATEST), "GREATEST")},
    {"FORMAT", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, static_cast<int>(ScalarFunction::FORMAT), "FORMAT")},
    {"LENGTH", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, static_cast<int>(ScalarFunction::LENGTH), "LENGTH")},
    {"POSITION", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, static_cast<int>(ScalarFunction::POSITION), "POSITION")},
    {"ABS", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, static_cast<int>(ScalarFunction::ABS), "ABS")},
    {"ROUND", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, static_cast<int>(ScalarFunction::ROUND), "ROUND")},
    {"FLOOR", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, static_cast<int>(ScalarFunction::FLOOR), "FLOOR")},
    {"CEILING", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, static_cast<int>(ScalarFunction::CEILING), "CEILING")},
    {"DATE_TRUNC", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, static_cast<int>(ScalarFunction::DATE_TRUNC), "DATE_TRUNC")},
    {"DATE_ADD", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, static_cast<int>(ScalarFunction::DATE_ADD), "DATE_ADD")},
    {"DATE_SUB", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, static_cast<int>(ScalarFunction::DATE_SUB), "DATE_SUB")},
    {"EXTRACT", std::make_shared<FunctionInfo>(FunctionCategory::SCALAR, static_cast<int>(ScalarFunction::EXTRACT), "EXTRACT")}
};

/**
 * @var Lexer::stringFuncMap
 * @brief Map for string functions.
 */
const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> Lexer::stringFuncMap = {
    {"UPPER", std::make_shared<FunctionInfo>(FunctionCategory::STRING, static_cast<int>(StringFunction::UPPER), "UPPER")},
    {"LOWER", std::make_shared<FunctionInfo>(FunctionCategory::STRING, static_cast<int>(StringFunction::LOWER), "LOWER")},
    {"SUBSTRING", std::make_shared<FunctionInfo>(FunctionCategory::STRING, static_cast<int>(StringFunction::SUBSTRING), "SUBSTRING")},
    {"TRIM", std::make_shared<FunctionInfo>(FunctionCategory::STRING, static_cast<int>(StringFunction::TRIM), "TRIM")},
    {"LTRIM", std::make_shared<FunctionInfo>(FunctionCategory::STRING, static_cast<int>(StringFunction::LTRIM), "LTRIM")},
    {"RTRIM", std::make_shared<FunctionInfo>(FunctionCategory::STRING, static_cast<int>(StringFunction::RTRIM), "RTRIM")},
    {"CONCAT", std::make_shared<FunctionInfo>(FunctionCategory::STRING, static_cast<int>(StringFunction::CONCAT), "CONCAT")},
    {"REPLACE", std::make_shared<FunctionInfo>(FunctionCategory::STRING, static_cast<int>(StringFunction::REPLACE), "REPLACE")},
    {"SPLIT_PART", std::make_shared<FunctionInfo>(FunctionCategory::STRING, static_cast<int>(StringFunction::SPLIT_PART), "SPLIT_PART")},
    {"LEFT", std::make_shared<FunctionInfo>(FunctionCategory::STRING, static_cast<int>(StringFunction::LEFT), "LEFT")},
    {"RIGHT", std::make_shared<FunctionInfo>(FunctionCategory::STRING, static_cast<int>(StringFunction::RIGHT), "RIGHT")},
    {"REPEAT", std::make_shared<FunctionInfo>(FunctionCategory::STRING, static_cast<int>(StringFunction::REPEAT), "REPEAT")},
    {"REVERSE", std::make_shared<FunctionInfo>(FunctionCategory::STRING, static_cast<int>(StringFunction::REVERSE), "REVERSE")},
    {"CHAR_LENGTH", std::make_shared<FunctionInfo>(FunctionCategory::STRING, static_cast<int>(StringFunction::CHAR_LENGTH), "CHAR_LENGTH")},
    {"CHARACTER_LENGTH", std::make_shared<FunctionInfo>(FunctionCategory::STRING, static_cast<int>(StringFunction::CHARACTER_LENGTH), "CHARACTER_LENGTH")},
    {"POSITION", std::make_shared<FunctionInfo>(FunctionCategory::STRING, static_cast<int>(StringFunction::POSITION), "POSITION")}
};

/**
 * @var Lexer::dtFuncMap
 * @brief Map for date/time functions.
 */
const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> Lexer::dtFuncMap = {
    {"DATEPART", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, static_cast<int>(DateTimeFunction::DATEPART), "DATEPART")},
    {"GETDATE", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, static_cast<int>(DateTimeFunction::GETDATE), "GETDATE")},
    {"NOW", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, static_cast<int>(DateTimeFunction::NOW), "NOW")},
    {"CURRENT_DATE", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, static_cast<int>(DateTimeFunction::CURRENT_DATE), "CURRENT_DATE")},
    {"CURRENT_TIME", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, static_cast<int>(DateTimeFunction::CURRENT_TIME), "CURRENT_TIME")},
    {"CURRENT_TIMESTAMP", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, static_cast<int>(DateTimeFunction::CURRENT_TIMESTAMP), "CURRENT_TIMESTAMP")},
    {"LOCALTIME", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, static_cast<int>(DateTimeFunction::LOCALTIME), "LOCALTIME")},
    {"LOCALTIMESTAMP", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, static_cast<int>(DateTimeFunction::LOCALTIMESTAMP), "LOCALTIMESTAMP")},
    {"AGE", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, static_cast<int>(DateTimeFunction::AGE), "AGE")},
    {"TO_DATE", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, static_cast<int>(DateTimeFunction::TO_DATE), "TO_DATE")},
    {"TO_TIMESTAMP", std::make_shared<FunctionInfo>(FunctionCategory::DATETIME, static_cast<int>(DateTimeFunction::TO_TIMESTAMP), "TO_TIMESTAMP")}
};

/**
 * @var Lexer::mathFuncMap
 * @brief Map for math functions.
 */
const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> Lexer::mathFuncMap = {
    {"ABS", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::ABS), "ABS")},
    {"CEILING", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::CEILING), "CEILING")},
    {"FLOOR", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::FLOOR), "FLOOR")},
    {"ROUND", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::ROUND), "ROUND")},
    {"POWER", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::POWER), "POWER")},
    {"SQRT", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::SQRT), "SQRT")},
    {"EXP", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::EXP), "EXP")},
    {"LN", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::LN), "LN")},
    {"LOG", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::LOG), "LOG")},
    {"MOD", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::MOD), "MOD")},
    {"RANDOM", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::RANDOM), "RANDOM")},
    {"SIGN", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::SIGN), "SIGN")},
    {"TRUNC", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::TRUNC), "TRUNC")},
    {"PI", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::PI), "PI")},
    {"SIN", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::SIN), "SIN")},
    {"COS", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::COS), "COS")},
    {"TAN", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::TAN), "TAN")},
    {"ASIN", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::ASIN), "ASIN")},
    {"ACOS", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::ACOS), "ACOS")},
    {"ATAN", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::ATAN), "ATAN")},
    {"DEGREES", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::DEGREES), "DEGREES")},
    {"RADIANS", std::make_shared<FunctionInfo>(FunctionCategory::MATHEMATICAL, static_cast<int>(MathFunction::RADIANS), "RADIANS")}
};

/**
 * @var Lexer::sysFuncMap
 * @brief Map for system functions.
 */
const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> Lexer::sysFuncMap = {
    {"SUSER_SNAME", std::make_shared<FunctionInfo>(FunctionCategory::SYSTEM, static_cast<int>(SystemFunction::SUSER_SNAME), "SUSER_SNAME")},
    {"CURRENT_USER", std::make_shared<FunctionInfo>(FunctionCategory::SYSTEM, static_cast<int>(SystemFunction::CURRENT_USER), "CURRENT_USER")},
    {"SESSION_USER", std::make_shared<FunctionInfo>(FunctionCategory::SYSTEM, static_cast<int>(SystemFunction::SESSION_USER), "SESSION_USER")},
    {"USER", std::make_shared<FunctionInfo>(FunctionCategory::SYSTEM, static_cast<int>(SystemFunction::USER), "USER")},
    {"SYSTEM_USER", std::make_shared<FunctionInfo>(FunctionCategory::SYSTEM, static_cast<int>(SystemFunction::SYSTEM_USER), "SYSTEM_USER")},
    {"TRANCOUNT", std::make_shared<FunctionInfo>(FunctionCategory::SYSTEM, static_cast<int>(SystemFunction::TRANCOUNT), "TRANCOUNT")},
    {"VERSION", std::make_shared<FunctionInfo>(FunctionCategory::SYSTEM, static_cast<int>(SystemFunction::VERSION), "VERSION")},
    {"DATABASE", std::make_shared<FunctionInfo>(FunctionCategory::SYSTEM, static_cast<int>(SystemFunction::DATABASE), "DATABASE")},
    {"SCHEMA_NAME", std::make_shared<FunctionInfo>(FunctionCategory::SYSTEM, static_cast<int>(SystemFunction::SCHEMA_NAME), "SCHEMA_NAME")}
};

/**
 * @var Lexer::windowFuncMap
 * @brief Map for window functions.
 */
const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> Lexer::windowFuncMap = {
    {"ROW_NUMBER", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, static_cast<int>(WindowFunction::ROW_NUMBER), "ROW_NUMBER")},
    {"RANK", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, static_cast<int>(WindowFunction::RANK), "RANK")},
    {"DENSE_RANK", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, static_cast<int>(WindowFunction::DENSE_RANK), "DENSE_RANK")},
    {"NTILE", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, static_cast<int>(WindowFunction::NTILE), "NTILE")},
    {"LEAD", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, static_cast<int>(WindowFunction::LEAD), "LEAD")},
    {"LAG", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, static_cast<int>(WindowFunction::LAG), "LAG")},
    {"FIRST_VALUE", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, static_cast<int>(WindowFunction::FIRST_VALUE), "FIRST_VALUE")},
    {"LAST_VALUE", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, static_cast<int>(WindowFunction::LAST_VALUE), "LAST_VALUE")},
    {"NTH_VALUE", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, static_cast<int>(WindowFunction::NTH_VALUE), "NTH_VALUE")},
    {"PERCENT_RANK", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, static_cast<int>(WindowFunction::PERCENT_RANK), "PERCENT_RANK")},
    {"CUME_DIST", std::make_shared<FunctionInfo>(FunctionCategory::WINDOW, static_cast<int>(WindowFunction::CUME_DIST), "CUME_DIST")}
};

/**
 * @var Lexer::arithMap
 * @brief Map for arithmetic operators.
 */
const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> Lexer::arithMap = {
    {"+", std::make_shared<OperatorInfo>(OperatorCategory::ARITHMETIC, static_cast<int>(ArithmeticOp::PLUS), "+")},
    {"-", std::make_shared<OperatorInfo>(OperatorCategory::ARITHMETIC, static_cast<int>(ArithmeticOp::MINUS), "-")},
    {"*", std::make_shared<OperatorInfo>(OperatorCategory::ARITHMETIC, static_cast<int>(ArithmeticOp::MULTIPLY), "*")},
    {"/", std::make_shared<OperatorInfo>(OperatorCategory::ARITHMETIC, static_cast<int>(ArithmeticOp::DIVIDE), "/")},
    {"%", std::make_shared<OperatorInfo>(OperatorCategory::ARITHMETIC, static_cast<int>(ArithmeticOp::MOD), "%")}
};

/**
 * @var Lexer::assignMap
 * @brief Map for assignment operators.
 */
const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> Lexer::assignMap = {
    {"=", std::make_shared<OperatorInfo>(OperatorCategory::ASSIGN, static_cast<int>(AssignOp::ASSIGN), "=")},
    {":=", std::make_shared<OperatorInfo>(OperatorCategory::ASSIGN, static_cast<int>(AssignOp::COLON_ASSIGN), ":=")}
};

/**
 * @var Lexer::compMap
 * @brief Map for comparison operators.
 */
const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> Lexer::compMap = {
    {"<", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, static_cast<int>(ComparisonOp::LESS), "<")},
    {">", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, static_cast<int>(ComparisonOp::GREATER), ">")},
    {"<=", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, static_cast<int>(ComparisonOp::LESS_EQUAL), "<=")},
    {">=", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, static_cast<int>(ComparisonOp::GREATER_EQUAL), ">=")},
    {"!=", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, static_cast<int>(ComparisonOp::NOT_EQUAL), "!=")},
    {"<>", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, static_cast<int>(ComparisonOp::NOT_EQUAL), "<>")},
    {"=", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, static_cast<int>(ComparisonOp::EQUAL), "=")},
    {"IS DISTINCT FROM", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, static_cast<int>(ComparisonOp::IS_DISTINCT_FROM), "IS DISTINCT FROM")},
    {"IS NOT DISTINCT FROM", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, static_cast<int>(ComparisonOp::IS_NOT_DISTINCT_FROM), "IS NOT DISTINCT FROM")},
    {"LIKE", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, static_cast<int>(ComparisonOp::LIKE), "LIKE")},
    {"NOT LIKE", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, static_cast<int>(ComparisonOp::NOT_LIKE), "NOT LIKE")},
    {"ILIKE", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, static_cast<int>(ComparisonOp::ILIKE), "ILIKE")},
    {"NOT ILIKE", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, static_cast<int>(ComparisonOp::NOT_ILIKE), "NOT ILIKE")},
    {"SIMILAR TO", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, static_cast<int>(ComparisonOp::SIMILAR_TO), "SIMILAR TO")},
    {"NOT SIMILAR TO", std::make_shared<OperatorInfo>(OperatorCategory::COMPARISON, static_cast<int>(ComparisonOp::NOT_SIMILAR_TO), "NOT SIMILAR TO")}
};

/**
 * @var Lexer::logMap
 * @brief Map for logical operators.
 */
const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> Lexer::logMap = {
    {"AND", std::make_shared<OperatorInfo>(OperatorCategory::LOGICAL, static_cast<int>(LogicalOp::AND), "AND")},
    {"OR", std::make_shared<OperatorInfo>(OperatorCategory::LOGICAL, static_cast<int>(LogicalOp::OR), "OR")},
    {"NOT", std::make_shared<OperatorInfo>(OperatorCategory::LOGICAL, static_cast<int>(LogicalOp::NOT), "NOT")},
    {"XOR", std::make_shared<OperatorInfo>(OperatorCategory::LOGICAL, static_cast<int>(LogicalOp::XOR), "XOR")},
    {"IMPLIES", std::make_shared<OperatorInfo>(OperatorCategory::LOGICAL, static_cast<int>(LogicalOp::IMPLIES), "IMPLIES")}
};

/**
 * @var Lexer::bitMap
 * @brief Map for bitwise operators.
 */
const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> Lexer::bitMap = {
    {"&", std::make_shared<OperatorInfo>(OperatorCategory::BITWISE, static_cast<int>(BitwiseOp::BITWISE_AND), "&")},
    {"|", std::make_shared<OperatorInfo>(OperatorCategory::BITWISE, static_cast<int>(BitwiseOp::BITWISE_OR), "|")},
    {"^", std::make_shared<OperatorInfo>(OperatorCategory::BITWISE, static_cast<int>(BitwiseOp::BITWISE_XOR), "^")},
    {"~", std::make_shared<OperatorInfo>(OperatorCategory::BITWISE, static_cast<int>(BitwiseOp::BITWISE_NOT), "~")},
    {"<<", std::make_shared<OperatorInfo>(OperatorCategory::BITWISE, static_cast<int>(BitwiseOp::LEFT_SHIFT), "<<")},
    {">>", std::make_shared<OperatorInfo>(OperatorCategory::BITWISE, static_cast<int>(BitwiseOp::RIGHT_SHIFT), ">>")}
};

/**
 * @var Lexer::concatMap
 * @brief Map for concatenation operators.
 */
const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> Lexer::concatMap = {
    {"||", std::make_shared<OperatorInfo>(OperatorCategory::CONCAT, static_cast<int>(ConcatOp::CONCAT), "||")}
};

/**
 * @var Lexer::commonSymMap
 * @brief Map for common SQL punctuation symbols.
 */
const std::unordered_map<char, std::shared_ptr<PunctuatorInfo>> Lexer::commonSymMap = {
    {',', std::make_shared<PunctuatorInfo>(PunctuatorCategory::COMMON, static_cast<int>(CommonSymbol::COMMA), ",")},
    {';', std::make_shared<PunctuatorInfo>(PunctuatorCategory::COMMON, static_cast<int>(CommonSymbol::SEMICOLON), ";")},
    {'(', std::make_shared<PunctuatorInfo>(PunctuatorCategory::COMMON, static_cast<int>(CommonSymbol::LPAREN), "(")},
    {')', std::make_shared<PunctuatorInfo>(PunctuatorCategory::COMMON, static_cast<int>(CommonSymbol::RPAREN), ")")},
    {'{', std::make_shared<PunctuatorInfo>(PunctuatorCategory::COMMON, static_cast<int>(CommonSymbol::LBRACE), "{")},
    {'}', std::make_shared<PunctuatorInfo>(PunctuatorCategory::COMMON, static_cast<int>(CommonSymbol::RBRACE), "}")},
    {'[', std::make_shared<PunctuatorInfo>(PunctuatorCategory::COMMON, static_cast<int>(CommonSymbol::LBRACKET), "[")},
    {']', std::make_shared<PunctuatorInfo>(PunctuatorCategory::COMMON, static_cast<int>(CommonSymbol::RBRACKET), "]")},
    {'.', std::make_shared<PunctuatorInfo>(PunctuatorCategory::COMMON, static_cast<int>(CommonSymbol::DOT), ".")},
    {':', std::make_shared<PunctuatorInfo>(PunctuatorCategory::COMMON, static_cast<int>(CommonSymbol::COLON), ":")},
    {'?', std::make_shared<PunctuatorInfo>(PunctuatorCategory::COMMON, static_cast<int>(CommonSymbol::PARAM_MARKER), "?")}
};

/**
 * @var Lexer::tsqlSymMap
 * @brief Map for TSQL-specific punctuation symbols.
 */
const std::unordered_map<std::string, std::shared_ptr<PunctuatorInfo>> Lexer::tsqlSymMap = {
    {".", std::make_shared<PunctuatorInfo>(PunctuatorCategory::TSQL, static_cast<int>(TSQLSymbol::DOT), ".")},
    {":", std::make_shared<PunctuatorInfo>(PunctuatorCategory::TSQL, static_cast<int>(TSQLSymbol::COLON), ":")},
    {"::", std::make_shared<PunctuatorInfo>(PunctuatorCategory::TSQL, static_cast<int>(TSQLSymbol::DOUBLE_COLON), "::")}
};

/**
 * @var Lexer::strDelimMap
 * @brief Map for string delimiters (single quote, double quote, backtick).
 */
const std::unordered_map<char, std::shared_ptr<PunctuatorInfo>> Lexer::strDelimMap = {
    {'\'', std::make_shared<PunctuatorInfo>(PunctuatorCategory::STRING_DELIM, static_cast<int>(StringDelimiter::SINGLE_QUOTE), "'")},
    {'"', std::make_shared<PunctuatorInfo>(PunctuatorCategory::STRING_DELIM, static_cast<int>(StringDelimiter::DOUBLE_QUOTE), "\"")},
    {'`', std::make_shared<PunctuatorInfo>(PunctuatorCategory::STRING_DELIM, static_cast<int>(StringDelimiter::BACKTICK), "`")}
};

/**
 * @var Lexer::dateTimePartsMap
 * @brief Map for SQL date/time part identifiers.
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

// === Lexer Core Methods ===

/**
 * @brief Construct a Lexer for the given SQL input.
 * @param query The SQL query string to lex.
 */
Lexer::Lexer(const std::string& query)
    : input(query), pos(0), tokenIndex(0)
{
    tokens.clear();
}

/**
 * @brief Advances pos past any whitespace and newlines.
 */
void Lexer::skipWhitespaceAndNewlines() {
    while (pos < input.size() && (input[pos] == ' ' || input[pos] == '\t' || input[pos] == '\r' || input[pos] == '\n')) {
        ++pos;
    }
}

/**
 * @brief Returns true if c can start an identifier.
 */
bool Lexer::isIdentifierStart(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

/**
 * @brief Returns true if c can be part of an identifier (after the first char).
 */
bool Lexer::isIdentifierPart(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

/**
 * @brief Returns the next token from the input stream.
 * @return Unique pointer to the next Token.
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
            pos + 1 < input.size() && input[pos + 1] == '\'') ||
        (c == 'E' && pos + 1 < input.size() && input[pos + 1] == '\'')) {
        // String, escape string, binary, or hex literal
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

/**
 * @brief Parse SQL comments: single-line and multi-line.
 * @return CommentToken
 */
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

/**
 * @brief Dot Notation (e.g. C.CustomerID)
 * @return PunctuatorToken
 */
std::unique_ptr<Token> Lexer::lexDotOperator() {
    char c = input[pos++];
    return std::make_unique<PunctuatorToken>(TSQLSymbol::DOT, ".");
}

/**
 * @brief Lex numeric literals (integer/float/negative, etc).
 * @return LiteralToken
 */
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

/**
 * @brief Lex keywords (not functions).
 * @return KeywordToken or LiteralToken for boolean/null.
 */
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

/**
 * @brief Lex functions (must be followed by '(' and match a function name).
 * @return FunctionToken or nullptr.
 */
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

/**
 * @brief Lex identifiers: regular, delimited, user/system variables, temp tables.
 * @return IdentifierToken
 */
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

/**
 * @brief Lex delimited identifier: [foo] or "foo".
 * @return IdentifierToken
 */
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

/**
 * @brief Check if the input at current pos starts with a dollar-quote ($...$).
 *        If so, returns true and sets tag to the full opening delimiter (e.g. "$abc$").
 *        Otherwise, returns false and tag is unchanged.
 */
bool Lexer::isDollarQuoteStart(const std::string& text, std::string& tag) {
    if (text.size() >= 2 && text[0] == '$') {
        size_t end = text.find('$', 1);
        if (end != std::string::npos && end > 0) {
            tag = text.substr(0, end + 1); // e.g. "$abc$"
            return true;
        }
    }
    return false;
}

/**
 * @brief Detect at pos if this is a dollar-quoted string, and if so, extract the tag.
 *        Advances pos if found. Returns true if found.
 */
bool Lexer::detectDollarQuoteTag(std::string& tag) {
    if (input[pos] == '$') {
        size_t end = input.find('$', pos + 1);
        if (end != std::string::npos && end > pos + 1) {
            tag = input.substr(pos, end - pos + 1);
            pos = end + 1; // Advance past opening tag
            return true;
        }
    }
    return false;
}

/**
 * @brief Lex a dollar-quoted string literal ($tag$...$tag$).
 *        Assumes pos is at the opening '$'.
 * @return LiteralToken
 */
std::unique_ptr<Token> Lexer::lexDollarQuotedString() {
    size_t start = pos;
    std::string tag;
    // Detect opening tag
    if (!detectDollarQuoteTag(tag))
        throw std::runtime_error("Invalid dollar-quote string at position " + std::to_string(start));

    size_t contentStart = pos;
    size_t endTagPos = input.find(tag, pos);
    if (endTagPos == std::string::npos)
        throw std::runtime_error("Unterminated dollar-quoted string starting at position " + std::to_string(start));

    std::string value = input.substr(contentStart, endTagPos - contentStart);
    pos = endTagPos + tag.length(); // Advance past closing tag

    // You can use a custom LiteralCategory if needed, here we use STRING
    return std::make_unique<LiteralToken>(LiteralCategory::STRING, input.substr(start, pos - start));
}

/**
 * @brief Lex string, escape string, dollar-quoted string, binary, hex, date, time, datetime literals.
 * @return LiteralToken
 */
std::unique_ptr<Token> Lexer::lexString() {
    size_t start = pos;
    char first = input[pos];

    // Dollar-quoted string ($tag$...$tag$)
    if (first == '$') {
        std::string tag;
        if (detectDollarQuoteTag(tag)) {
            // We already advanced past opening tag; rewind to original pos for full value
            pos = start;
            return lexDollarQuotedString();
        }
    }

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

    // Escape string literal (E'...')
    if (first == 'E' && pos + 1 < input.size() && input[pos + 1] == '\'') {
        pos += 2;
        size_t literalStart = pos;
        while (pos < input.size() && input[pos] != '\'') {
            // Handle escape sequences
            if (input[pos] == '\\' && pos + 1 < input.size()) {
                pos += 2;
            }
            else {
                ++pos;
            }
        }
        if (pos >= input.size())
            throw std::runtime_error("Unterminated escape string literal at position " + std::to_string(start));
        ++pos; // skip closing quote
        std::string word = input.substr(start, pos - start);
        return std::make_unique<LiteralToken>(LiteralCategory::ESCAPE_STRING, word);
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

/**
 * @brief Lex JSON literal: {...}
 * @return LiteralToken
 */
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

/**
 * @brief Lex operators.
 * @return OperatorToken
 */
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

/**
 * @brief Lex punctuators: comma, paren, bracket, etc.
 * @return PunctuatorToken
 */
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

// =======================================================================
// ========== Literal Recognition Utilities ==========
// =======================================================================

/**
 * @brief Checks if string is a (double-quoted or single-quoted) string literal (not char).
 */
bool Lexer::isStringLiteral(const std::string& word) {
    // Accepts any quoted string (length >= 2)
    return (
        (word.size() >= 2 && word.front() == '"' && word.back() == '"') ||
        (word.size() >= 2 && word.front() == '\'' && word.back() == '\'')
        );
}

/**
 * @brief Checks if string is a single character in single quotes (e.g. 'A').
 */
bool Lexer::isCharLiteral(const std::string& word) {
    // Only allow single-character (not escape) in single quotes
    return (word.size() == 3 && word.front() == '\'' && word.back() == '\'');
}

/**
 * @brief Checks if string is an integer (optionally negative).
 */
bool Lexer::isIntegerLiteral(const std::string& word) {
    if (word.empty()) return false;
    size_t startIdx = (word[0] == '-') ? 1 : 0;
    if (startIdx == word.size()) return false;
    for (size_t i = startIdx; i < word.size(); ++i)
        if (!std::isdigit(static_cast<unsigned char>(word[i]))) return false;
    return true;
}

/**
 * @brief Checks if string is a float (optionally negative, must contain one '.').
 */
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

/**
 * @brief Checks if string is a binary literal: B'0101'.
 */
bool Lexer::isBinaryLiteral(const std::string& word) {
    if (word.size() < 4) return false;
    if ((word[0] != 'B' && word[0] != 'b') || word[1] != '\'' || word.back() != '\'') return false;
    for (size_t i = 2; i < word.size() - 1; ++i)
        if (word[i] != '0' && word[i] != '1') return false;
    return true;
}

/**
 * @brief Checks if string is a hex literal: X'1A2B'.
 */
bool Lexer::isHexLiteral(const std::string& word) {
    if (word.size() < 4) return false;
    if ((word[0] != 'X' && word[0] != 'x') || word[1] != '\'' || word.back() != '\'') return false;
    for (size_t i = 2; i < word.size() - 1; ++i)
        if (!std::isxdigit(static_cast<unsigned char>(word[i]))) return false;
    return true;
}

/**
 * @brief Checks if string is an escape string literal: E'...'
 */
bool Lexer::isEscapeStringLiteral(const std::string& word) {
    return (word.size() >= 3 && word[0] == 'E' && word[1] == '\'' && word.back() == '\'');
}

/**
 * @brief Checks if string is a date literal: 'YYYY-MM-DD'.
 */
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

/**
 * @brief Checks if string is a time literal: 'HH:MM:SS'.
 */
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

/**
 * @brief Checks if string is a datetime literal: 'YYYY-MM-DD HH:MM:SS'.
 */
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

/**
 * @brief Checks if string is a JSON literal: {...}.
 */
bool Lexer::isJSONLiteral(const std::string& word) {
    return (word.size() >= 2 && word.front() == '{' && word.back() == '}');
}

/**
 * @brief Checks if string is boolean: TRUE or FALSE (case-insensitive).
 */
bool Lexer::isBooleanLiteral(const std::string& word) {
    if (word.empty()) return false;
    std::string up = word;
    std::transform(up.begin(), up.end(), up.begin(), ::toupper);
    return (up == "TRUE" || up == "FALSE");
}

/**
 * @brief Checks if string is NULL literal (case-insensitive).
 */
bool Lexer::isNullLiteral(const std::string& word) {
    if (word.empty()) return false;
    std::string up = word;
    std::transform(up.begin(), up.end(), up.begin(), ::toupper);
    return (up == "NULL");
}

/**
 * @brief Checks if string is an array literal: [ ... ] (simple check).
 */
bool Lexer::isArrayLiteral(const std::string& word) {
    return word.size() >= 2 && word.front() == '[' && word.back() == ']';
}

/**
 * @brief Checks if string is a UUID literal: 8-4-4-4-12 hex+hyphen.
 */
bool Lexer::isUUIDLiteral(const std::string& word) {
    if (word.size() != 36) return false;
    for (size_t i : {8, 13, 18, 23})
        if (word[i] != '-') return false;
    for (size_t i = 0; i < word.size(); ++i) {
        if (i == 8 || i == 13 || i == 18 || i == 23) continue;
        if (!std::isxdigit(static_cast<unsigned char>(word[i]))) return false;
    }
    return true;
}

/**
 * @brief Checks if string is an XML literal: <...> (very basic).
 */
bool Lexer::isXMLLiteral(const std::string& word) {
    return word.size() >= 3 && word.front() == '<' && word.back() == '>';
}

/**
 * @brief Returns true if c is a valid operator symbol.
 */
bool Lexer::isOperatorChar(char c) {
    return std::string("=!<>+-*/%^&|~").find(c) != std::string::npos;
}

// =======================================================================
// ========== Token Creation Methods ======================================
// =======================================================================

/**
 * @brief Creates a KeywordToken from a string, or nullptr if not a keyword.
 */
std::unique_ptr<Token> Lexer::createKeywordToken(const std::string& word) {
    std::string upper = word;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    // Use a vector of maps for easy extension/maintenance
    const std::vector<const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>>*> keywordMaps = {
        &dmlMap, &ddlMap, &clauseMap, &cteMap, &setOpMap, &wordOpMap,
        &logicalConstMap, &tranMap, &secMap, &progMap, &miscMap
    };
    for (const auto* mapPtr : keywordMaps) {
        auto it = mapPtr->find(upper);
        if (it != mapPtr->end())
            return std::make_unique<KeywordToken>(it->second, word);
    }
    return nullptr;
}

/**
 * @brief Creates a FunctionToken from a string, or nullptr if not a function.
 */
std::unique_ptr<Token> Lexer::createFunctionToken(const std::string& word) {
    std::string upper = word;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

    const std::vector<const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>>*> functionMaps = {
        &aggFuncMap, &scalarFuncMap, &stringFuncMap, &dtFuncMap,
        &mathFuncMap, &sysFuncMap, &windowFuncMap
    };
    for (const auto* mapPtr : functionMaps) {
        auto it = mapPtr->find(upper);
        if (it != mapPtr->end())
            return std::make_unique<FunctionToken>(it->second, word);
    }
    return nullptr;
}

/**
 * @brief Creates a LiteralToken of the appropriate type from a string.
 */
std::unique_ptr<Token> Lexer::createLiteralToken(const std::string& word) {
    // Order matters: more specific types first!
    if (isEscapeStringLiteral(word)) return std::make_unique<LiteralToken>(LiteralCategory::ESCAPE_STRING, word);
    if (isBinaryLiteral(word))       return std::make_unique<LiteralToken>(LiteralCategory::BINARY, word);
    if (isHexLiteral(word))          return std::make_unique<LiteralToken>(LiteralCategory::HEX, word);
    if (isDateTimeLiteral(word))     return std::make_unique<LiteralToken>(LiteralCategory::DATETIME, word);
    if (isDateLiteral(word))         return std::make_unique<LiteralToken>(LiteralCategory::DATE, word);
    if (isTimeLiteral(word))         return std::make_unique<LiteralToken>(LiteralCategory::TIME, word);
    if (isJSONLiteral(word))         return std::make_unique<LiteralToken>(LiteralCategory::JSON, word);
    if (isBooleanLiteral(word))      return std::make_unique<LiteralToken>(LiteralCategory::BOOLEAN, word);
    if (isNullLiteral(word))         return std::make_unique<LiteralToken>(LiteralCategory::NULL_VALUE, word);
    if (isFloatLiteral(word))        return std::make_unique<LiteralToken>(LiteralCategory::FLOAT, word);
    if (isIntegerLiteral(word))      return std::make_unique<LiteralToken>(LiteralCategory::INTEGER, word);
    if (isCharLiteral(word))         return std::make_unique<LiteralToken>(LiteralCategory::CHAR, word);
    if (isStringLiteral(word))       return std::make_unique<LiteralToken>(LiteralCategory::STRING, word);
    if (isArrayLiteral(word))        return std::make_unique<LiteralToken>(LiteralCategory::ARRAY, word);
    if (isUUIDLiteral(word))         return std::make_unique<LiteralToken>(LiteralCategory::UUID, word);
    if (isXMLLiteral(word))          return std::make_unique<LiteralToken>(LiteralCategory::XML, word);

    // If nothing matches, treat as unknown literal
    return std::make_unique<LiteralToken>(LiteralCategory::UNKNOWN, word);
}

/**
 * @brief Creates an OperatorToken from a string, or UNKNOWN if not a known operator.
 */
std::unique_ptr<Token> Lexer::createOperatorToken(const std::string& word) {
    // Use a vector of maps for extensibility and DRYness
    const std::vector<const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>>*> opMaps = {
        &compMap, &assignMap, &logMap, &bitMap, &concatMap, &arithMap
    };
    for (const auto* mapPtr : opMaps) {
        auto it = mapPtr->find(word);
        if (it != mapPtr->end())
            return std::make_unique<OperatorToken>(it->second, word);
    }
    // Fallback: unknown operator
    return std::make_unique<OperatorToken>(ArithmeticOp::UNKNOWN, word);
}

/**
 * @brief Creates a PunctuatorToken from a string, or UNKNOWN if not a known punctuator.
 */
std::unique_ptr<Token> Lexer::createPunctuatorToken(const std::string& word) {
    if (word.empty())
        return std::make_unique<PunctuatorToken>(CommonSymbol::UNKNOWN, word);

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

    // If not found, unknown punctuator
    return std::make_unique<PunctuatorToken>(CommonSymbol::UNKNOWN, word);
}