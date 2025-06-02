/**
 * @file XMLLiteralValue.hpp
 * @brief Definition of XMLLiteralValue class.
 */

#pragma once
#include "LiteralValue.hpp"
#include <string>
#include <vector>
#include <pugixml.hpp>

class XMLLiteralValue : public LiteralValue {
public:
    // === Constructors and Destructor ===
    explicit XMLLiteralValue(const std::string& v);
    XMLLiteralValue(); // Creates empty XML document

    // Rule of five
    XMLLiteralValue(const XMLLiteralValue&) = default;
    XMLLiteralValue(XMLLiteralValue&&) noexcept = default;
    XMLLiteralValue& operator=(const XMLLiteralValue&) = default;
    XMLLiteralValue& operator=(XMLLiteralValue&&) noexcept = default;
    ~XMLLiteralValue() override = default;

    // === Core Interface ===
    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    // === Validation ===
    bool isValid() const override;
    void validate() const override;
    bool equals(const LiteralValue& other) const override;
    bool hasElement(const std::string& name) const noexcept;

    // === XML Operations ===
    std::unique_ptr<LiteralValue> applyXml(
        const std::string& xpath) const noexcept;
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

    // === Access Operations ===
    std::unique_ptr<LiteralValue> getAttribute(
        const std::string& attr) const noexcept;
    std::unique_ptr<LiteralValue> getText() const noexcept;
    std::vector<std::unique_ptr<LiteralValue>> findByXPath(
        const std::string& xpath) const noexcept;

    // === Getters ===
    const std::string& getValue() const { return value; }

private:
    std::string value;  ///< XML value as string

protected:
    /**
     * @brief Parses XML string to document.
     * @return Pair of document and parse result
     */
    std::pair<pugi::xml_document, pugi::xml_parse_result>
        parseXML() const noexcept;

    /**
     * @brief Serializes XML node to string.
     * @param node Node to serialize
     * @return Serialized XML string
     */
    static std::string serializeNode(
        const pugi::xml_node& node) noexcept;

    /**
     * @brief Validates XML format.
     * @param doc Parsed XML document
     * @return true if document is valid
     */
    static bool validateXMLFormat(
        const pugi::xml_document& doc) noexcept;
};