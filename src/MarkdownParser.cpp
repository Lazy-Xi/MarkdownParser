#include "MarkdownParser.h"

#include "SectionList.h"

#include <qregularexpression.h>
#include <qstring.h>

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

const QString MarkdownParser::default_style_sheet = R"(
body {
  font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
  line-height: 1.6;
  color: #333;
  background-color: #fff;
  padding: 2rem;
  max-width: 1000px;
  margin: auto;
}

h1, h2, h3, h4, h5, h6 {
  font-weight: 600;
  line-height: 1.25;
  margin-top: 1.5em;
  margin-bottom: 0.5em;
}

h1 { font-size: 2em; border-bottom: 2px solid #eaecef; padding-bottom: 0.3em; }
h2 { font-size: 1.75em; border-bottom: 1px solid #eaecef; padding-bottom: 0.3em; }
h3 { font-size: 1.5em; }
h4 { font-size: 1.25em; }
h5 { font-size: 1em; }
h6 { font-size: 0.875em; color: #6a737d; }

p {
  margin: 1em 0;
}

strong {
  font-weight: bold;
}
em {
  font-style: italic;
}

a {
  color: #0366d6;
  text-decoration: none;
}
a:hover {
  text-decoration: underline;
}

ul, ol {
  padding-left: 2em;
  margin: 1em 0;
}
li {
  margin: 0.5em 0;
}

code {
  font-family: SFMono-Regular, Consolas, "Liberation Mono", Menlo, monospace;
  background-color: #f6f8fa;
  padding: 0.2em 0.4em;
  border-radius: 3px;
  font-size: 0.95em;
}
pre {
  background-color: #f6f8fa;
  padding: 1em;
  overflow: auto;
  border-radius: 5px;
}
pre code {
  background: none;
  padding: 0;
  font-size: 0.95em;
}

blockquote {
  border-left: 4px solid #dfe2e5;
  padding: 0.5em 1em;
  color: #6a737d;
  background-color: #f9f9f9;
  margin: 1em 0;
}

table {
  border-collapse: collapse;
  width: 100%;
  margin: 1em 0;
}
th, td {
  border: 1px solid #dfe2e5;
  padding: 0.6em 1em;
  text-align: left;
}
th {
  background-color: #f6f8fa;
}

img {
  max-width: 100%;
  height: auto;
  display: block;
  margin: auto;
  margin-top: 1em;
  margin-bottom: 1em;
}

hr {
  border: 0;
  border-top: 1px solid #eaecef;
  margin: 2em 0;
})";