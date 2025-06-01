/**
 * @file XMLLiteralValue.cpp
 * @brief Implementation of XMLLiteralValue methods.
 * @details
 * Implements construction, string conversion, deep copy, XPath queries, comparison,
 * validation, element and attribute access, and XPath node finding for XMLLiteralValue.
 */

#include "LiteralValue.hpp"
#include <pugixml.hpp>
#include <memory>
#include <sstream>

// --- Constructor ---
XMLLiteralValue::XMLLiteralValue(const std::string& v) : value(v) {}

// --- Return XML as string ---
std::string XMLLiteralValue::toString() const {
    return value;
}

// --- Deep copy ---
std::unique_ptr<LiteralValue> XMLLiteralValue::clone() const {
    return std::make_unique<XMLLiteralValue>(value);
}

// --- Run XPath query, return first result as XML or string ---
// If node found: XMLLiteralValue (element) or StringLiteralValue (attribute/text)
// If not found: nullptr
std::unique_ptr<LiteralValue> XMLLiteralValue::applyXml(const std::string& xpath) const {
    pugi::xml_document doc;
    pugi::xml_parse_result ok = doc.load_string(value.c_str());
    if (!ok) return nullptr;
    pugi::xpath_node_set nodes = doc.select_nodes(xpath.c_str());
    if (nodes.empty()) return nullptr;
    const auto& n = nodes.first();
    if (n.node()) {
        std::ostringstream oss;
        n.node().print(oss, "", pugi::format_raw);
        return std::unique_ptr<LiteralValue>(new XMLLiteralValue(oss.str()));
    }
    if (n.attribute()) {
        return std::unique_ptr<LiteralValue>(new StringLiteralValue(n.attribute().value()));
    }
    return nullptr;
}

// --- Compare by canonicalized XML, not text ---
// Only EQUAL/NOT_EQUAL supported, others return false
bool XMLLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    const auto* r = dynamic_cast<const XMLLiteralValue*>(&rhs);
    if (!r) return false;
    pugi::xml_document doc_a, doc_b;
    if (!doc_a.load_string(value.c_str()) || !doc_b.load_string(r->value.c_str())) return false;
    std::ostringstream ossa, ossb;
    doc_a.print(ossa, "", pugi::format_raw);
    doc_b.print(ossb, "", pugi::format_raw);
    switch (op) {
    case ComparisonOp::EQUAL:      return ossa.str() == ossb.str();
    case ComparisonOp::NOT_EQUAL:  return ossa.str() != ossb.str();
    default: return false;
    }
}

// --- Check XML validity (parsing and root) ---
bool XMLLiteralValue::isValid() const {
    pugi::xml_document doc;
    return doc.load_string(value.c_str()) && doc.first_child();
}

// --- Check for presence of element (by name, at any depth) ---
bool XMLLiteralValue::hasElement(const std::string& name) const {
    pugi::xml_document doc;
    if (!doc.load_string(value.c_str())) return false;
    std::string xpath = "//" + name;
    pugi::xpath_node_set nodes = doc.select_nodes(xpath.c_str());
    return !nodes.empty();
}

// --- Get attribute value from root node (as string literal) ---
// If not found: nullptr
std::unique_ptr<LiteralValue> XMLLiteralValue::getAttribute(const std::string& attr) const {
    pugi::xml_document doc;
    if (!doc.load_string(value.c_str())) return nullptr;
    pugi::xml_node root = doc.first_child();
    if (!root) return nullptr;
    pugi::xml_attribute a = root.attribute(attr.c_str());
    if (!a) return nullptr;
    return std::unique_ptr<LiteralValue>(new StringLiteralValue(a.value()));
}

// --- Get text content of root element (as string literal) ---
// If not found: nullptr
std::unique_ptr<LiteralValue> XMLLiteralValue::getText() const {
    pugi::xml_document doc;
    if (!doc.load_string(value.c_str())) return nullptr;
    pugi::xml_node root = doc.first_child();
    if (!root) return nullptr;
    const char* text = root.text().get();
    if (!text) return nullptr;
    return std::unique_ptr<LiteralValue>(new StringLiteralValue(text));
}

// --- Find nodes by XPath, return as XMLLiteralValue or StringLiteralValue vector ---
// If attribute/text node: StringLiteralValue, else XMLLiteralValue
std::vector<std::unique_ptr<LiteralValue>> XMLLiteralValue::findByXPath(const std::string& xpath) const {
    std::vector<std::unique_ptr<LiteralValue>> out;
    pugi::xml_document doc;
    if (!doc.load_string(value.c_str())) return out;
    pugi::xpath_node_set nodes = doc.select_nodes(xpath.c_str());
    for (auto& n : nodes) {
        if (n.node()) {
            std::ostringstream oss;
            n.node().print(oss, "", pugi::format_raw);
            out.emplace_back(new XMLLiteralValue(oss.str()));
        }
        else if (n.attribute()) {
            out.emplace_back(new StringLiteralValue(n.attribute().value()));
        }
    }
    return out;
}