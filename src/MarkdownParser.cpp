#include "MarkdownParser.h"

#include "default_style_sheet.h"
#include "SectionList.h"

#include <qregularexpression.h>
#include <qstring.h>

const QString MarkdownParser::default_style_sheet = __default_style_sheet;

MarkdownParser::MarkdownParser() : style_sheet(default_style_sheet) {
    this->compressStyleSheet();
}

QString MarkdownParser::toHtml(const QString& md) {
    QString _md(md);
    _md = _md.replace("\r\n", "\n")
        .replace("\r", "\n")
        .replace("\t", "    ");
    QString content = SectionList(_md).toHtml();

    QRegularExpression regex("<h1[^>]*>(.*?)</h1>", 
        QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch match = regex.match(content);
    QString title = "Markdown";
    if (match.hasMatch()) { title = match.captured(1).trimmed(); }

    return QString("%1%2%3")
        .arg(this->before(title))
        .arg(content)
        .arg(this->after());
}

void MarkdownParser::setStyleSheet(const QString& style_sheet) {
    this->style_sheet = style_sheet;
    this->compressStyleSheet();
}

QString MarkdownParser::before(const QString& title) {
    return QString(R"(<html lang="en"><head><title>%1</title><style>%2</style></head><body><article>)")
        .arg(title)
        .arg(this->style_sheet);
}

QString MarkdownParser::after() {
    return "</article></body></html>";
}

void MarkdownParser::compressStyleSheet() {
    this->style_sheet = this->style_sheet
        .remove(QRegularExpression(R"(/\*[^*]*\*+([^/*][^*]*\*+)*/)"))
        .replace(QRegularExpression(R"(\s+)"), " ")
        .replace(QRegularExpression(R"(\s*([{}:;,])\s*)"), "\\1")
        .trimmed();
}
