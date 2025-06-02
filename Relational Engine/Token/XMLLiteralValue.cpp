/**
 * @file XMLLiteralValue.cpp
 * @brief Implementation of XMLLiteralValue methods.
 */

#include "XMLLiteralValue.hpp"
#include "StringLiteralValue.hpp"
#include <sstream>
#include <stdexcept>

 // === Constructors ===

XMLLiteralValue::XMLLiteralValue()
    : value("<?xml version=\"1.0\"?><root/>")
{
}

XMLLiteralValue::XMLLiteralValue(const std::string& v)
    : value(v)
{
    validate();
}

// === Core Interface ===

std::string XMLLiteralValue::toString() const {
    return value;
}

std::unique_ptr<LiteralValue> XMLLiteralValue::clone() const {
    return std::make_unique<XMLLiteralValue>(value);
}

// === Protected Methods ===

std::pair<pugi::xml_document, pugi::xml_parse_result>
XMLLiteralValue::parseXML() const noexcept {
    pugi::xml_document doc;
    auto result = doc.load_string(value.c_str());
    return { std::move(doc), result };
}

std::string XMLLiteralValue::serializeNode(
    const pugi::xml_node& node) noexcept
{
    std::ostringstream oss;
    node.print(oss, "", pugi::format_raw);
    return oss.str();
}

bool XMLLiteralValue::validateXMLFormat(
    const pugi::xml_document& doc) noexcept
{
    return doc.first_child() != nullptr;
}

// === Validation ===

bool XMLLiteralValue::isValid() const {
    auto [doc, result] = parseXML();
    return result && validateXMLFormat(doc);
}

void XMLLiteralValue::validate() const {
    auto [doc, result] = parseXML();
    if (!result) {
        throw std::invalid_argument(
            "Invalid XML format: " + std::string(result.description()));
    }
    if (!validateXMLFormat(doc)) {
        throw std::invalid_argument("XML document has no root element");
    }
}

bool XMLLiteralValue::equals(const LiteralValue& other) const {
    const auto* xmlOther =
        dynamic_cast<const XMLLiteralValue*>(&other);
    if (!xmlOther) return false;

    auto [doc1, result1] = parseXML();
    auto [doc2, result2] = xmlOther->parseXML();

    if (!result1 || !result2) return false;

    return serializeNode(doc1) == serializeNode(doc2);
}

// === XML Operations ===

std::unique_ptr<LiteralValue> XMLLiteralValue::applyXml(
    const std::string& xpath) const noexcept
{
    auto [doc, result] = parseXML();
    if (!result) return nullptr;

    pugi::xpath_node_set nodes = doc.select_nodes(xpath.c_str());
    if (nodes.empty()) return nullptr;

    const auto& node = nodes.first();
    if (node.node()) {
        return std::make_unique<XMLLiteralValue>(
            serializeNode(node.node()));
    }
    if (node.attribute()) {
        return std::make_unique<StringLiteralValue>(
            node.attribute().value());
    }
    return nullptr;
}

bool XMLLiteralValue::compare(
    const LiteralValue& rhs, ComparisonOp op) const
{
    const auto* r = dynamic_cast<const XMLLiteralValue*>(&rhs);
    if (!r) return false;

    auto [doc1, result1] = parseXML();
    auto [doc2, result2] = r->parseXML();

    if (!result1 || !result2) return false;

    std::string xml1 = serializeNode(doc1);
    std::string xml2 = serializeNode(doc2);

    switch (op) {
    case ComparisonOp::EQUAL:     return xml1 == xml2;
    case ComparisonOp::NOT_EQUAL: return xml1 != xml2;
    default:                      return false;
    }
}

bool XMLLiteralValue::hasElement(
    const std::string& name) const noexcept
{
    auto [doc, result] = parseXML();
    if (!result) return false;

    std::string xpath = "//" + name;
    return !doc.select_nodes(xpath.c_str()).empty();
}

// === Access Operations ===

std::unique_ptr<LiteralValue> XMLLiteralValue::getAttribute(
    const std::string& attr) const noexcept
{
    auto [doc, result] = parseXML();
    if (!result) return nullptr;

    pugi::xml_node root = doc.first_child();
    if (!root) return nullptr;

    pugi::xml_attribute attribute = root.attribute(attr.c_str());
    if (!attribute) return nullptr;

    return std::make_unique<StringLiteralValue>(attribute.value());
}

std::unique_ptr<LiteralValue> XMLLiteralValue::getText() const noexcept {
    auto [doc, result] = parseXML();
    if (!result) return nullptr;

    pugi::xml_node root = doc.first_child();
    if (!root) return nullptr;

    const char* text = root.text().get();
    if (!text) return nullptr;

    return std::make_unique<StringLiteralValue>(text);
}

std::vector<std::unique_ptr<LiteralValue>> XMLLiteralValue::findByXPath(
    const std::string& xpath) const noexcept
{
    std::vector<std::unique_ptr<LiteralValue>> results;

    auto [doc, result] = parseXML();
    if (!result) return results;

    pugi::xpath_node_set nodes = doc.select_nodes(xpath.c_str());
    for (const auto& node : nodes) {
        if (node.node()) {
            results.push_back(std::make_unique<XMLLiteralValue>(
                serializeNode(node.node())));
        }
        else if (node.attribute()) {
            results.push_back(std::make_unique<StringLiteralValue>(
                node.attribute().value()));
        }
    }

    return results;
}