#pragma once

#include <qstring.h>

namespace TypeUtils {
    bool isIndentedCodeBlock(const QString& line);
    bool isHorizontalRule(const QString& line);
    bool isTitle(const QString& line);
    bool isCodeBlock(const QString& line);
    bool isOrderedList(const QString& line);
    bool isQuote(const QString& line);
    bool isTable(const QString& line);
    bool isTableHeaderSeparator(const QString& line);
    bool isUnorderedList(const QString& line);
    bool isHtmlBlock(const QString& line);
    bool isHtmlVoidTag(const QString& tag);
}