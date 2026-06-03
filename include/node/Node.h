#pragma once

#include "export.h"

#include <memory>
#include <vector>

class NodeVisitor;

// One enum spanning the whole tree. Block and inline are a logical
// split, not a type-system one: a single children_ collection serves
// both (a Heading holds inlines; an Emphasis holds inlines), and one
// visitor interface covers it all.
enum class NodeType {
    Document,

    // block
    Heading,
    Paragraph,
    CodeBlock,
    HtmlBlock,
    ThematicBreak,
    Table,
    TableRow,
    TableCell,
    List,
    ListItem,
    BlockQuote,
    Toc,

    // inline
    Text,
    RawText,
    Emphasis,
    InlineCode,
    Link,
    Image,
    LineBreak,
};

// Base of the AST. A parent owns its children via unique_ptr, so
// destroying the Document frees the whole tree. Nodes are move-only:
// there is no reason to value-copy a polymorphic tree.
class MARKDOWNPARSER_API Node {
public:
    explicit Node(NodeType type) : node_type_(type) {}
    virtual ~Node() = default;

    Node(const Node &) = delete;
    Node &operator=(const Node &) = delete;

    NodeType type() const noexcept { return node_type_; }

    const std::vector<std::unique_ptr<Node>> &
    children() const noexcept {
        return children_;
    }
    // Non-const handle for builders (e.g. InlineParser targets a
    // node's child list directly while constructing a span).
    std::vector<std::unique_ptr<Node>> &children() noexcept {
        return children_;
    }
    void appendChild(std::unique_ptr<Node> child) {
        children_.push_back(std::move(child));
    }
    Node *lastChild() noexcept {
        return children_.empty() ? nullptr : children_.back().get();
    }
    bool hasChildren() const noexcept { return !children_.empty(); }

    // Double dispatch: each concrete node calls v.visit(*this).
    virtual void accept(NodeVisitor &visitor) const = 0;

protected:
    NodeType node_type_;
    std::vector<std::unique_ptr<Node>> children_;
};
