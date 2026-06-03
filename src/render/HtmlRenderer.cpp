#include "render/HtmlRenderer.h"

#include "node/BlockNodes.h"
#include "node/InlineNodes.h"

#include <QRegularExpression>

#include <memory>

static std::pair<const char *, const char *> spanTags(SpanKind k) {
    switch (k) {
    case SpanKind::Strong: return {"<strong>", "</strong>"};
    case SpanKind::StrongEmphasis:
        return {"<strong><em>", "</em></strong>"};
    case SpanKind::Strikethrough: return {"<del>", "</del>"};
    case SpanKind::Highlight: return {"<mark>", "</mark>"};
    default: return {"<em>", "</em>"};
    }
}

namespace {
QString alignStr(Align a) {
    switch (a) {
    case Align::CENTER: return "center";
    case Align::RIGHT: return "right";
    case Align::LEFT:
    default: return "left";
    }
}

// Plain (unescaped, tag-free) text of an inline subtree, for heading
// slug ids.
QString plainText(const Node &n) {
    switch (n.type()) {
    case NodeType::Text:
        return static_cast<const TextNode &>(n).text();
    case NodeType::RawText:
        return static_cast<const RawTextNode &>(n).text();
    case NodeType::InlineCode:
        return static_cast<const InlineCodeNode &>(n).code();
    default: break;
    }
    QString s;
    for (const auto &c : n.children()) {
        s += plainText(*c);
    }
    return s;
}

// Heading slug: plain text, lower-cased, runs of space/underscore →
// '-'.
QString slugify(const Node &heading) {
    return plainText(heading).trimmed().toLower().replace(
        QRegularExpression(R"([\s_]+)"), "-");
}

// DFS-collect headings into a flat list for table-of-contents
// generation.
void collectHeadings(
    const Node &n, std::vector<std::pair<int, QString>> &out) {
    for (const auto &c : n.children()) {
        if (c->type() == NodeType::Heading) {
            const auto &h = static_cast<const HeadingNode &>(*c);
            out.emplace_back(h.level(), plainText(h).trimmed());
        }
        collectHeadings(*c, out);
    }
}
} // namespace

QString HtmlRenderer::render(const Node &root) {
    out_.clear();
    // Pre-collect headings so a TocNode anywhere in the tree can
    // render them.
    headings_.clear();
    std::vector<std::pair<int, QString>> hs;
    collectHeadings(root, hs);
    for (const auto &[level, text] : hs) {
        QString slug = text.toLower().replace(
            QRegularExpression(R"([\s_]+)"), "-");
        headings_.push_back({level, text, slug});
    }
    root.accept(*this);
    return out_;
}

QString HtmlRenderer::renderInlines(
    const std::vector<std::unique_ptr<Node>> &inlines) {
    out_.clear();
    for (const auto &c : inlines) {
        c->accept(*this);
    }
    return out_;
}

void HtmlRenderer::renderChildren(const Node &n) {
    for (const auto &c : n.children()) {
        c->accept(*this);
    }
}

// ── block nodes
// ──────────────────────────────────────────────────────────────

void HtmlRenderer::visit(const DocumentNode &n) {
    out_ += "<article>";
    renderChildren(n);
    out_ += "</article>";
}

void HtmlRenderer::visit(const HeadingNode &n) {
    out_ +=
        QString(R"(<h%1 id="%2">)").arg(n.level()).arg(slugify(n));
    renderChildren(n);
    out_ += QString("</h%1>").arg(n.level());
}

void HtmlRenderer::visit(const ParagraphNode &n) {
    out_ += "<p>";
    renderChildren(n);
    out_ += "</p>";
}

void HtmlRenderer::visit(const CodeBlockNode &n) {
    out_ += "<pre><code>";
    for (const auto &l : n.lines()) {
        out_ += escape(l) + "\n";
    }
    out_ += "</code></pre>";
}

void HtmlRenderer::visit(const HtmlBlockNode &n) {
    // Reproduce HtmlSection::toHtml: trim each line, join with space
    // when the current line doesn't end with '>' and the next doesn't
    // start with '<'.
    out_ += "<div>";
    const auto &lines = n.lines();
    for (qsizetype i = 0; i < lines.size(); ++i) {
        out_ += lines[i].trimmed();
        if (i < lines.size() - 1 &&
            !lines[i].trimmed().endsWith(">") &&
            !lines[i + 1].trimmed().startsWith("<")) {
            out_ += " ";
        }
    }
    out_ += "</div>";
}

void HtmlRenderer::visit(const ThematicBreakNode &) {
    out_ += "<hr />";
}

void HtmlRenderer::visit(const TableNode &n) {
    out_ += "<table>";
    renderChildren(n);
    out_ += "</tbody></table>";
}

void HtmlRenderer::visit(const TableRowNode &n) {
    if (n.header()) {
        out_ += "<thead><tr>";
        renderChildren(n);
        out_ += "</tr></thead><tbody>";
    }
    else {
        out_ += "<tr>";
        renderChildren(n);
        out_ += "</tr>";
    }
}

void HtmlRenderer::visit(const TableCellNode &n) {
    const QString tag = n.header() ? "th" : "td";
    out_ += QString(R"(<%1 align="%2">)")
                .arg(tag)
                .arg(alignStr(n.align()));
    renderChildren(n);
    out_ += QString("</%1>").arg(tag);
}

void HtmlRenderer::visit(const ListNode &n) {
    const QString tag = n.ordered() ? "ol" : "ul";
    out_ += "<" + tag + ">";
    renderChildren(n);
    out_ += "</" + tag + ">";
}

void HtmlRenderer::visit(const ListItemNode &n) {
    out_ += "<li>";
    renderChildren(n);
    out_ += "</li>";
}

void HtmlRenderer::visit(const BlockQuoteNode &n) {
    out_ += "<blockquote>";
    renderChildren(n);
    out_ += "</blockquote>";
}

void HtmlRenderer::visit(const TocNode &) {
    if (headings_.empty()) return;

    // Base = shallowest heading level present; deeper levels nest.
    int base = headings_[0].level;
    for (const auto &h : headings_) base = qMin(base, h.level);

    auto item = [&](const HeadingEntry &h) {
        out_ += QString(R"(<li><a href="#%1">%2</a>)")
                    .arg(h.slug)
                    .arg(escape(h.text));
    };

    out_ += "<nav class=\"toc\"><ul>";
    int prev = 0; // nesting depth of previous item (0 = base)
    bool first = true;
    for (const auto &h : headings_) {
        int depth = h.level - base;
        if (first) {
            first = false;
        }
        else if (depth > prev) {
            for (int d = prev; d < depth; ++d) out_ += "<ul>";
        }
        else if (depth == prev) {
            out_ += "</li>";
        }
        else {
            out_ += "</li>";
            for (int d = prev; d > depth; --d) out_ += "</ul></li>";
        }
        item(h);
        prev = depth;
    }
    out_ += "</li>";
    for (int d = prev; d > 0; --d) out_ += "</ul></li>";
    out_ += "</ul></nav>";
}

// ── inline nodes
// ─────────────────────────────────────────────────────────────

void HtmlRenderer::visit(const TextNode &n) {
    out_ += escape(n.text());
}

void HtmlRenderer::visit(const RawTextNode &n) { out_ += n.text(); }

void HtmlRenderer::visit(const EmphasisNode &n) {
    const auto [open, close] = spanTags(n.kind());
    out_ += open;
    renderChildren(n);
    out_ += close;
}

void HtmlRenderer::visit(const InlineCodeNode &n) {
    out_ += "<code>" + escape(n.code()) + "</code>";
}

void HtmlRenderer::visit(const LinkNode &n) {
    out_ += QString(R"(<a href="%1">)").arg(n.href());
    renderChildren(n);
    out_ += "</a>";
}

void HtmlRenderer::visit(const ImageNode &n) {
    out_ += QString(R"(<img src="%1" alt="%2" />)")
                .arg(n.src())
                .arg(n.alt());
}

void HtmlRenderer::visit(const LineBreakNode &) { out_ += "<br />"; }
