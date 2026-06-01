#pragma once

#include "export.h"
#include "node/Node.h"

#include <qstring.h>
#include <qstringlist.h>

#include <utility>

// Block-level nodes.

// Root of the tree; renders as <article>…</article>.
class MARKDOWNPARSER_API DocumentNode : public Node {
public:
    DocumentNode() : Node(NodeType::Document) {}
    void accept(NodeVisitor &v) const override;
};

// Heading; children are inlines. raw_text_ is the heading text with markers
// intact (kept available for callers); the slug id is derived from plain text
// by the renderer.
class MARKDOWNPARSER_API HeadingNode : public Node {
public:
    HeadingNode(int level, QString rawText)
        : Node(NodeType::Heading), level_(level),
          raw_text_(std::move(rawText)) {}
    int level() const noexcept { return level_; }
    const QString &rawText() const noexcept { return raw_text_; }
    void accept(NodeVisitor &v) const override;

private:
    int level_;
    QString raw_text_;
};

// Paragraph; children are inlines.
class MARKDOWNPARSER_API ParagraphNode : public Node {
public:
    ParagraphNode() : Node(NodeType::Paragraph) {}
    void accept(NodeVisitor &v) const override;
};

// Fenced or indented code block; lines are RAW (renderer escapes). info_ is the
// fenced-code info string (future syntax-highlight hook).
class MARKDOWNPARSER_API CodeBlockNode : public Node {
public:
    explicit CodeBlockNode(QStringList lines, QString info = {})
        : Node(NodeType::CodeBlock), lines_(std::move(lines)),
          info_(std::move(info)) {}
    const QStringList &lines() const noexcept { return lines_; }
    const QString &info() const noexcept { return info_; }
    void accept(NodeVisitor &v) const override;

private:
    QStringList lines_;
    QString info_;
};

// Raw HTML block; lines emitted verbatim inside <div>…</div>.
class MARKDOWNPARSER_API HtmlBlockNode : public Node {
public:
    explicit HtmlBlockNode(QStringList lines)
        : Node(NodeType::HtmlBlock), lines_(std::move(lines)) {}
    const QStringList &lines() const noexcept { return lines_; }
    void accept(NodeVisitor &v) const override;

private:
    QStringList lines_;
};

// Thematic break (<hr />).
class MARKDOWNPARSER_API ThematicBreakNode : public Node {
public:
    ThematicBreakNode() : Node(NodeType::ThematicBreak) {}
    void accept(NodeVisitor &v) const override;
};

enum class Align { LEFT, CENTER, RIGHT };

// Table cell; children are inlines.
class MARKDOWNPARSER_API TableCellNode : public Node {
public:
    TableCellNode(Align align, bool header)
        : Node(NodeType::TableCell), align_(align), header_(header) {}
    Align align() const noexcept { return align_; }
    bool header() const noexcept { return header_; }
    void accept(NodeVisitor &v) const override;

private:
    Align align_;
    bool header_;
};

// Table row; children are TableCellNode.
class MARKDOWNPARSER_API TableRowNode : public Node {
public:
    explicit TableRowNode(bool header)
        : Node(NodeType::TableRow), header_(header) {}
    bool header() const noexcept { return header_; }
    void accept(NodeVisitor &v) const override;

private:
    bool header_;
};

// Table; children are TableRowNode (first is the header row).
class MARKDOWNPARSER_API TableNode : public Node {
public:
    TableNode() : Node(NodeType::Table) {}
    void accept(NodeVisitor &v) const override;
};

// Ordered or unordered list; children are ListItemNode.
class MARKDOWNPARSER_API ListNode : public Node {
public:
    explicit ListNode(bool ordered) : Node(NodeType::List), ordered_(ordered) {}
    bool ordered() const noexcept { return ordered_; }
    void accept(NodeVisitor &v) const override;

private:
    bool ordered_;
};

// List item; children are the item's blocks/inlines.
class MARKDOWNPARSER_API ListItemNode : public Node {
public:
    ListItemNode() : Node(NodeType::ListItem) {}
    void accept(NodeVisitor &v) const override;
};

// Block quote; children are ParagraphNode and nested BlockQuoteNode.
class MARKDOWNPARSER_API BlockQuoteNode : public Node {
public:
    BlockQuoteNode() : Node(NodeType::BlockQuote) {}
    void accept(NodeVisitor &v) const override;
};

// Table-of-contents placeholder. Emitted where a <!--TOC--> marker appears; the
// renderer fills it by collecting the document's headings into a nested list.
class MARKDOWNPARSER_API TocNode : public Node {
public:
    TocNode() : Node(NodeType::Toc) {}
    void accept(NodeVisitor &v) const override;
};
