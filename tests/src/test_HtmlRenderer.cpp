#include "test_HtmlRenderer.h"

#include "node/BlockNodes.h"
#include "node/InlineNodes.h"
#include "render/HtmlRenderer.h"

#include <QtTest/QTest>

#include <memory>
#include <utility>

namespace {
std::unique_ptr<TextNode> text(const QString &s) {
    return std::make_unique<TextNode>(s);
}
} // namespace

void test_HtmlRenderer::testHeading() {
    auto h = std::make_unique<HeadingNode>(2, "Title 2");
    h->appendChild(text("Title 2"));
    QCOMPARE(HtmlRenderer().render(*h),
        QString(R"(<h2 id="title-2">Title 2</h2>)"));
}

void test_HtmlRenderer::testParagraphWithInlines() {
    auto p = std::make_unique<ParagraphNode>();
    p->appendChild(text("This is a "));
    auto em = std::make_unique<EmphasisNode>(SpanKind::Emphasis);
    em->appendChild(text("italic"));
    p->appendChild(std::move(em));
    p->appendChild(text(" example."));
    QCOMPARE(HtmlRenderer().render(*p),
        QString("<p>This is a <em>italic</em> example.</p>"));
}

void test_HtmlRenderer::testCodeBlockEscapes() {
    // Raw text in the node; renderer escapes at emit time.
    auto cb = std::make_unique<CodeBlockNode>(
        QStringList {"a < b && c > d"});
    QCOMPARE(HtmlRenderer().render(*cb),
        QString("<pre><code>a &lt; b &amp;&amp; c &gt; "
                "d\n</code></pre>"));
}

void test_HtmlRenderer::testList() {
    auto ul = std::make_unique<ListNode>(false);
    for (const QString &s : {QString("one"), QString("two")}) {
        auto li = std::make_unique<ListItemNode>();
        li->appendChild(text(s));
        ul->appendChild(std::move(li));
    }
    QCOMPARE(HtmlRenderer().render(*ul),
        QString("<ul><li>one</li><li>two</li></ul>"));
}

void test_HtmlRenderer::testNestedQuote() {
    auto outer = std::make_unique<BlockQuoteNode>();
    auto p1 = std::make_unique<ParagraphNode>();
    p1->appendChild(text("outer"));
    outer->appendChild(std::move(p1));
    auto inner = std::make_unique<BlockQuoteNode>();
    auto p2 = std::make_unique<ParagraphNode>();
    p2->appendChild(text("inner"));
    inner->appendChild(std::move(p2));
    outer->appendChild(std::move(inner));
    QCOMPARE(HtmlRenderer().render(*outer),
        QString("<blockquote><p>outer</p><blockquote><p>inner</p>"
                "</blockquote></blockquote>"));
}

void test_HtmlRenderer::testTable() {
    auto table = std::make_unique<TableNode>();
    auto head = std::make_unique<TableRowNode>(true);
    auto hc = std::make_unique<TableCellNode>(Align::CENTER, true);
    hc->appendChild(text("H"));
    head->appendChild(std::move(hc));
    table->appendChild(std::move(head));
    auto row = std::make_unique<TableRowNode>(false);
    auto dc = std::make_unique<TableCellNode>(Align::LEFT, false);
    dc->appendChild(text("C"));
    row->appendChild(std::move(dc));
    table->appendChild(std::move(row));
    QCOMPARE(HtmlRenderer().render(*table),
        QString(
            R"(<table><thead><tr><th align="center">H</th></tr></thead>)"
            R"(<tbody><tr><td align="left">C</td></tr></tbody></table>)"));
}

void test_HtmlRenderer::testLinkAndImage() {
    auto p = std::make_unique<ParagraphNode>();
    auto link = std::make_unique<LinkNode>("https://x.y");
    link->appendChild(text("label"));
    p->appendChild(std::move(link));
    p->appendChild(std::make_unique<ImageNode>("i.png", "alt"));
    QCOMPARE(HtmlRenderer().render(*p),
        QString(R"(<p><a href="https://x.y">label</a>)"
                R"(<img src="i.png" alt="alt" /></p>)"));
}

void test_HtmlRenderer::testHeadingSlugPlainText() {
    // Slug is built from plain text, stripping inline markers/tags.
    auto h = std::make_unique<HeadingNode>(
        1, "Title 1 *italic* **bold** `code`");
    h->appendChild(text("Title 1 "));
    auto em = std::make_unique<EmphasisNode>(SpanKind::Emphasis);
    em->appendChild(text("italic"));
    h->appendChild(std::move(em));
    h->appendChild(text(" "));
    auto st = std::make_unique<EmphasisNode>(SpanKind::Strong);
    st->appendChild(text("bold"));
    h->appendChild(std::move(st));
    h->appendChild(text(" "));
    h->appendChild(std::make_unique<InlineCodeNode>("code"));
    QCOMPARE(HtmlRenderer().render(*h),
        QString(
            R"(<h1 id="title-1-italic-bold-code">Title 1 <em>italic</em> )"
            R"(<strong>bold</strong> <code>code</code></h1>)"));
}

void test_HtmlRenderer::testToc() {
    // Document: TocNode followed by headings of varying levels. The
    // TocNode renders a nested <nav> list mirroring heading levels.
    auto heading = [](int level, const QString &s) {
        auto h = std::make_unique<HeadingNode>(level, s);
        h->appendChild(std::make_unique<TextNode>(s));
        return h;
    };
    auto doc = std::make_unique<DocumentNode>();
    doc->appendChild(std::make_unique<TocNode>());
    doc->appendChild(heading(1, "A"));
    doc->appendChild(heading(2, "B"));
    doc->appendChild(heading(2, "C"));
    doc->appendChild(heading(3, "D"));
    doc->appendChild(heading(1, "E"));

    const QString toc = R"(<nav class="toc"><ul>)"
                        R"(<li><a href="#a">A</a><ul>)"
                        R"(<li><a href="#b">B</a></li>)"
                        R"(<li><a href="#c">C</a><ul>)"
                        R"(<li><a href="#d">D</a></li>)"
                        R"(</ul></li></ul></li>)"
                        R"(<li><a href="#e">E</a></li>)"
                        R"(</ul></nav>)";
    QCOMPARE(HtmlRenderer().render(*doc),
        "<article>" + toc +
            R"(<h1 id="a">A</h1><h2 id="b">B</h2><h2 id="c">C</h2>)"
            R"(<h3 id="d">D</h3><h1 id="e">E</h1></article>)");
}
