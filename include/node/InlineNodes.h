#pragma once

#include "export.h"
#include "node/Node.h"

#include <qstring.h>

#include <utility>

// Inline-level nodes. Text-bearing nodes carry RAW (unescaped) text; the
// renderer escapes at emit time.

// Literal text run. Renderer HTML-escapes on emit.
class MARKDOWNPARSER_API TextNode : public Node {
public:
    explicit TextNode(QString text)
        : Node(NodeType::Text), text_(std::move(text)) {}
    const QString &text() const noexcept { return text_; }
    void accept(NodeVisitor &v) const override;

private:
    QString text_;
};

// Verbatim passthrough (e.g. backslash-escaped char already resolved to a
// literal, or raw HTML). Renderer emits as-is, no escaping.
class MARKDOWNPARSER_API RawTextNode : public Node {
public:
    explicit RawTextNode(QString text)
        : Node(NodeType::RawText), text_(std::move(text)) {}
    const QString &text() const noexcept { return text_; }
    void accept(NodeVisitor &v) const override;

private:
    QString text_;
};

// The five emphasis-like spans differ only by tag pair, which is renderer data.
enum class SpanKind {
    Strong,
    Emphasis,
    StrongEmphasis,
    Strikethrough,
    Highlight,
};

// Emphasis span; children are the span content.
class MARKDOWNPARSER_API EmphasisNode : public Node {
public:
    explicit EmphasisNode(SpanKind kind)
        : Node(NodeType::Emphasis), kind_(kind) {}
    SpanKind kind() const noexcept { return kind_; }
    void accept(NodeVisitor &v) const override;

private:
    SpanKind kind_;
};

// Inline code span; raw code, not inline-parsed. Renderer escapes on emit.
class MARKDOWNPARSER_API InlineCodeNode : public Node {
public:
    explicit InlineCodeNode(QString code)
        : Node(NodeType::InlineCode), code_(std::move(code)) {}
    const QString &code() const noexcept { return code_; }
    void accept(NodeVisitor &v) const override;

private:
    QString code_;
};

// Hyperlink; children are the (inline-parsed) link text.
class MARKDOWNPARSER_API LinkNode : public Node {
public:
    LinkNode(QString href, QString title = {})
        : Node(NodeType::Link), href_(std::move(href)),
          title_(std::move(title)) {}
    const QString &href() const noexcept { return href_; }
    const QString &title() const noexcept { return title_; }
    void accept(NodeVisitor &v) const override;

private:
    QString href_;
    QString title_;
};

// Image; alt is plain text (not inline-parsed today).
class MARKDOWNPARSER_API ImageNode : public Node {
public:
    ImageNode(QString src, QString alt)
        : Node(NodeType::Image), src_(std::move(src)), alt_(std::move(alt)) {}
    const QString &src() const noexcept { return src_; }
    const QString &alt() const noexcept { return alt_; }
    void accept(NodeVisitor &v) const override;

private:
    QString src_;
    QString alt_;
};

// Hard line break. Reserved; not emitted in phase 1.
class MARKDOWNPARSER_API LineBreakNode : public Node {
public:
    LineBreakNode() : Node(NodeType::LineBreak) {}
    void accept(NodeVisitor &v) const override;
};
