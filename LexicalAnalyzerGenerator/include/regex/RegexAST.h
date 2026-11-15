#pragma once

#include "common/Types.h"
#include <memory>
#include <string>
#include <vector>

namespace compiler {

enum class RegexNodeType {
    EPSILON,
    LITERAL,
    RANGE,
    CONCAT,
    UNION,
    STAR,
    PLUS
};

class RegexNode {
public:
    virtual ~RegexNode() = default;
    virtual RegexNodeType getType() const = 0;
};

class EpsilonNode : public RegexNode {
public:
    RegexNodeType getType() const override { return RegexNodeType::EPSILON; }
};

class LiteralNode : public RegexNode {
public:
    explicit LiteralNode(char ch) : character_(ch) {}
    RegexNodeType getType() const override { return RegexNodeType::LITERAL; }
    char getCharacter() const { return character_; }

private:
    char character_;
};

class RangeNode : public RegexNode {
public:
    RangeNode(char start, char end) : start_(start), end_(end) {}
    RegexNodeType getType() const override { return RegexNodeType::RANGE; }
    char getStart() const { return start_; }
    char getEnd() const { return end_; }

private:
    char start_;
    char end_;
};

class ConcatNode : public RegexNode {
public:
    ConcatNode(std::shared_ptr<RegexNode> left, std::shared_ptr<RegexNode> right)
        : left_(std::move(left)), right_(std::move(right)) {}
    RegexNodeType getType() const override { return RegexNodeType::CONCAT; }
    const std::shared_ptr<RegexNode>& getLeft() const { return left_; }
    const std::shared_ptr<RegexNode>& getRight() const { return right_; }

private:
    std::shared_ptr<RegexNode> left_;
    std::shared_ptr<RegexNode> right_;
};

class UnionNode : public RegexNode {
public:
    UnionNode(std::shared_ptr<RegexNode> left, std::shared_ptr<RegexNode> right)
        : left_(std::move(left)), right_(std::move(right)) {}
    RegexNodeType getType() const override { return RegexNodeType::UNION; }
    const std::shared_ptr<RegexNode>& getLeft() const { return left_; }
    const std::shared_ptr<RegexNode>& getRight() const { return right_; }

private:
    std::shared_ptr<RegexNode> left_;
    std::shared_ptr<RegexNode> right_;
};

class StarNode : public RegexNode {
public:
    explicit StarNode(std::shared_ptr<RegexNode> child) : child_(std::move(child)) {}
    RegexNodeType getType() const override { return RegexNodeType::STAR; }
    const std::shared_ptr<RegexNode>& getChild() const { return child_; }

private:
    std::shared_ptr<RegexNode> child_;
};

class PlusNode : public RegexNode {
public:
    explicit PlusNode(std::shared_ptr<RegexNode> child) : child_(std::move(child)) {}
    RegexNodeType getType() const override { return RegexNodeType::PLUS; }
    const std::shared_ptr<RegexNode>& getChild() const { return child_; }

private:
    std::shared_ptr<RegexNode> child_;
};

}
