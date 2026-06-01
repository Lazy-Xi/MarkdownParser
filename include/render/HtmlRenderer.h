#pragma once

#include "export.h"
#include "node/NodeVisitor.h"

#include <qstring.h>
#include <qstringlist.h>

#include <memory>
#include <vector>

enum class SpanKind;
class Node;

// Walks the AST and produces an HTML body. HTML-escaping happens here, at emit
// time: text-bearing nodes carry raw text and are escaped on output. Span tag
// literals live in HtmlRenderer.cpp (spanTags), a rendering concern.
class MARKDOWNPARSER_API HtmlRenderer : public NodeVisitor {
public:
    QString render(const Node &root);
    // Renders a list of inline nodes to their HTML (no enclosing block tag).
    QString renderInlines(const std::vector<std::unique_ptr<Node>> &inlines);

    void visit(const DocumentNode &) override;
    void visit(const HeadingNode &) override;
    void visit(const ParagraphNode &) override;
    void visit(const CodeBlockNode &) override;
    void visit(const HtmlBlockNode &) override;
    void visit(const ThematicBreakNode &) override;
    void visit(const TableNode &) override;
    void visit(const TableRowNode &) override;
    void visit(const TableCellNode &) override;
    void visit(const ListNode &) override;
    void visit(const ListItemNode &) override;
    void visit(const BlockQuoteNode &) override;
    void visit(const TocNode &) override;
    void visit(const TextNode &) override;
    void visit(const RawTextNode &) override;
    void visit(const EmphasisNode &) override;
    void visit(const InlineCodeNode &) override;
    void visit(const LinkNode &) override;
    void visit(const ImageNode &) override;
    void visit(const LineBreakNode &) override;

private:
    QString out_;

    // Collected headings (level, text, slug) for table-of-contents generation.
    struct HeadingEntry { int level; QString text; QString slug; };
    std::vector<HeadingEntry> headings_;

    void renderChildren(const Node &n);
    static QString escape(const QString &raw) { return raw.toHtmlEscaped(); }
};
