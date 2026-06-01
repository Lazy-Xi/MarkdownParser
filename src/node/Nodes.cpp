#include "node/BlockNodes.h"
#include "node/InlineNodes.h"
#include "node/NodeVisitor.h"

// accept() bodies for every concrete node. Centralizing them here also anchors
// each class's vtable to this translation unit, keeping the headers light.

void DocumentNode::accept(NodeVisitor &v) const { v.visit(*this); }
void HeadingNode::accept(NodeVisitor &v) const { v.visit(*this); }
void ParagraphNode::accept(NodeVisitor &v) const { v.visit(*this); }
void CodeBlockNode::accept(NodeVisitor &v) const { v.visit(*this); }
void HtmlBlockNode::accept(NodeVisitor &v) const { v.visit(*this); }
void ThematicBreakNode::accept(NodeVisitor &v) const { v.visit(*this); }
void TableNode::accept(NodeVisitor &v) const { v.visit(*this); }
void TableRowNode::accept(NodeVisitor &v) const { v.visit(*this); }
void TableCellNode::accept(NodeVisitor &v) const { v.visit(*this); }
void ListNode::accept(NodeVisitor &v) const { v.visit(*this); }
void ListItemNode::accept(NodeVisitor &v) const { v.visit(*this); }
void BlockQuoteNode::accept(NodeVisitor &v) const { v.visit(*this); }
void TocNode::accept(NodeVisitor &v) const { v.visit(*this); }

void TextNode::accept(NodeVisitor &v) const { v.visit(*this); }
void RawTextNode::accept(NodeVisitor &v) const { v.visit(*this); }
void EmphasisNode::accept(NodeVisitor &v) const { v.visit(*this); }
void InlineCodeNode::accept(NodeVisitor &v) const { v.visit(*this); }
void LinkNode::accept(NodeVisitor &v) const { v.visit(*this); }
void ImageNode::accept(NodeVisitor &v) const { v.visit(*this); }
void LineBreakNode::accept(NodeVisitor &v) const { v.visit(*this); }
