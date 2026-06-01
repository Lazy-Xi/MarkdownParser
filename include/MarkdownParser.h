#pragma once

#include "export.h"
#include "node/Node.h"

#include <memory>
#include <qstring.h>

class MARKDOWNPARSER_API MarkdownParser {
public:
    static const QString default_style_sheet;
    MarkdownParser();

    // Parse Markdown into a DocumentNode AST (move-only owner).
    static std::unique_ptr<Node> parse(const QString &md);

    void setStyleSheet(const QString &style_sheet);
    QString toHtml(const QString &md);

private:
    QString style_sheet;

    void compressStyleSheet();
    QString before(const QString &title);
    QString after();
};