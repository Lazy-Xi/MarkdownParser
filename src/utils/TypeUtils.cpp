#include "utils/TypeUtils.h"

#include "utils/RegexUtils.h"

#include <qset.h>

namespace TypeUtils {

bool isIndentedCodeBlock(const QString& line) {
    return RegexUtils::indentedCodeBlock().match(line).hasMatch();
}

bool isHorizontalRule(const QString& line) {
    return RegexUtils::horizontalRule().match(line).hasMatch();
}

bool isTitle(const QString& line) {
    return RegexUtils::title().match(line).hasMatch();
}

bool isCodeBlock(const QString& line) {
    return RegexUtils::codeBlock().match(line).hasMatch();
}

bool isOrderedList(const QString& line) {
    return RegexUtils::orderedList(true).match(line).hasMatch();
}

bool isQuote(const QString& line) {
    return RegexUtils::quote().match(line).hasMatch();
}

bool isTable(const QString& line) {
    return RegexUtils::table().match(line).hasMatch();
}

bool isTableHeaderSeparator(const QString& line) {
    return RegexUtils::tableHeaderSeparator().match(line).hasMatch();
}

bool isUnorderedList(const QString& line) {
    return RegexUtils::unorderedList(true).match(line).hasMatch();
}

bool isHtmlBlock(const QString& line) {
    const QString& trimmed = line.trimmed();
    return trimmed.startsWith('<') && !trimmed.startsWith("<!--") && RegexUtils::htmlBlock().match(line).hasMatch();
}

bool isHtmlVoidTag(const QString& tag) {
    static const QSet<QString> voidTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    return voidTags.contains(tag.toLower());
}

}