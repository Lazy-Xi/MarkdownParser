#include "MarkdownParser.h"

#include "SectionParser.h"
#include "node/BlockNodes.h"
#include "render/HtmlRenderer.h"
#include "default_style_sheet.h"

#include <qregularexpression.h>
#include <qstring.h>
#include <qstringlist.h>

const QString MarkdownParser::default_style_sheet = __default_style_sheet;

MarkdownParser::MarkdownParser() : style_sheet(default_style_sheet) {
    compressStyleSheet();
}

std::unique_ptr<Node> MarkdownParser::parse(const QString &md) {
    QString _md(md);
    _md = _md.replace("\r\n", "\n").replace("\r", "\n").replace("\t", "    ");
    QStringList lines(_md.split('\n'));
    QStringListIterator it(lines);
    SectionParser parser(it);
    auto doc = std::make_unique<DocumentNode>();
    for (auto n = parser.nextNode(); n; n = parser.nextNode())
        doc->appendChild(std::move(n));
    return doc;
}

// Walk the AST to find the first level-1 heading and return its inner HTML.
static QString extractTitle(const Node &doc) {
    for (const auto &child : doc.children()) {
        if (child->type() == NodeType::Heading) {
            const auto &h = static_cast<const HeadingNode &>(*child);
            if (h.level() == 1)
                return HtmlRenderer().renderInlines(h.children()).trimmed();
        }
    }
    return {};
}

QString MarkdownParser::toHtml(const QString &md) {
    auto doc = parse(md);
    QString content = HtmlRenderer().render(*doc);
    QString title = extractTitle(*doc);
    if (title.isEmpty()) title = "Markdown";
    return before(title) + content + after();
}

void MarkdownParser::setStyleSheet(const QString &style_sheet) {
    this->style_sheet = style_sheet;
    compressStyleSheet();
}

QString MarkdownParser::before(const QString &title) {
    return QString(
        R"(<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1"><title>%1</title><style>%2</style></head><body>)")
        .arg(title).arg(style_sheet);
}

QString MarkdownParser::after() { return "</body></html>"; }

void MarkdownParser::compressStyleSheet() {
    style_sheet =
        style_sheet
            .remove(QRegularExpression(R"(/\*[^*]*\*+([^/*][^*]*\*+)*/)"))
            .replace(QRegularExpression(R"(\s+)"), " ")
            .replace(QRegularExpression(R"(\s*([{}:;,])\s*)"), "\\1")
            .trimmed();
}
