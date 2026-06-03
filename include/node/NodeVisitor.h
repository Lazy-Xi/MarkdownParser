#pragma once

// Forward declarations of every concrete node type. Concrete visitors
// include node/BlockNodes.h and node/InlineNodes.h for the full
// definitions.
class DocumentNode;
class HeadingNode;
class ParagraphNode;
class CodeBlockNode;
class HtmlBlockNode;
class ThematicBreakNode;
class TableNode;
class TableRowNode;
class TableCellNode;
class ListNode;
class ListItemNode;
class BlockQuoteNode;
class TocNode;
class TextNode;
class RawTextNode;
class EmphasisNode;
class InlineCodeNode;
class LinkNode;
class ImageNode;
class LineBreakNode;

// Visitor over the AST. A new output format (plain text, markdown,
// TOC) is a new NodeVisitor; node classes stay untouched.
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    virtual void visit(const DocumentNode &) = 0;
    virtual void visit(const HeadingNode &) = 0;
    virtual void visit(const ParagraphNode &) = 0;
    virtual void visit(const CodeBlockNode &) = 0;
    virtual void visit(const HtmlBlockNode &) = 0;
    virtual void visit(const ThematicBreakNode &) = 0;
    virtual void visit(const TableNode &) = 0;
    virtual void visit(const TableRowNode &) = 0;
    virtual void visit(const TableCellNode &) = 0;
    virtual void visit(const ListNode &) = 0;
    virtual void visit(const ListItemNode &) = 0;
    virtual void visit(const BlockQuoteNode &) = 0;
    virtual void visit(const TocNode &) = 0;
    virtual void visit(const TextNode &) = 0;
    virtual void visit(const RawTextNode &) = 0;
    virtual void visit(const EmphasisNode &) = 0;
    virtual void visit(const InlineCodeNode &) = 0;
    virtual void visit(const LinkNode &) = 0;
    virtual void visit(const ImageNode &) = 0;
    virtual void visit(const LineBreakNode &) = 0;
};
