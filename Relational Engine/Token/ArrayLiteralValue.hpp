/**
 * @file ArrayLiteralValue.hpp
 * @brief Definition of ArrayLiteralValue class.
 */

#pragma once
#include "LiteralValue.hpp"
#include <vector>
#include <memory>

class ArrayLiteralValue : public LiteralValue {
public:
    // === Constructors and Destructor ===
    explicit ArrayLiteralValue(
        std::vector<std::shared_ptr<LiteralValue>> elems);
    ArrayLiteralValue(); // Creates empty array

    // Rule of five
    ArrayLiteralValue(const ArrayLiteralValue&) = default;
    ArrayLiteralValue(ArrayLiteralValue&&) noexcept = default;
    ArrayLiteralValue& operator=(const ArrayLiteralValue&) = default;
    ArrayLiteralValue& operator=(ArrayLiteralValue&&) noexcept = default;
    ~ArrayLiteralValue() override = default;

    // === Core Interface ===
    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    /**
     * @brief Creates string representation of array.
     * @param elements Array elements to stringify
     * @return Formatted string representation
     */
    static std::string elementsToString(
        const std::vector<std::shared_ptr<LiteralValue>>& elements) noexcept;

    // === Validation ===
    bool isValid() const override;
    void validate() const override;
    bool equals(const LiteralValue& other) const override;

    // === Operations ===
    std::unique_ptr<LiteralValue> applyConcat(
        const LiteralValue& rhs, ConcatOp op) const override;
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

    // === Array-specific Operations ===
    /**
     * @brief Gets element at specified index.
     * @param index Index of element to retrieve
     * @return Element at index or nullptr if out of bounds
     */
    std::shared_ptr<LiteralValue> at(size_t index) const noexcept;

    /**
     * @brief Gets number of elements in array.
     * @return Size of array
     */
    size_t size() const noexcept { return elements.size(); }

    /**
     * @brief Checks if array is empty.
     * @return true if array has no elements
     */
    bool empty() const noexcept { return elements.empty(); }

    // === Element Access ===
    const std::vector<std::shared_ptr<LiteralValue>>& getElements() const {
        return elements;
    }

private:
    std::vector<std::shared_ptr<LiteralValue>> elements;

protected:
    /**
     * @brief Validates array elements.
     * @return true if all elements are valid
     */
    bool validateElements() const noexcept;
};