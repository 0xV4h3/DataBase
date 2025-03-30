#pragma once

enum class LiteralType {
    STRING,   // "..."
    CHAR,     // '...'
    INTEGER,  // 123, -456
    FLOAT,    // 3.14, -0.001
    BINARY,   // 0101
    HEX,       // X'01AF'
    DATE,     // '2025-03-30'
    TIME,     // '14:30:00'
    DATETIME, // '2025-03-30 14:30:00'
    JSON,     // '{ "key": "value" }'
    BOOLEAN,  // TRUE, FALSE
    NULL_VALUE, // NULL 
    UNKNOWN   // unknown literal
};