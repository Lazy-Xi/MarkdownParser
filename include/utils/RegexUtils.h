#pragma once

#include <qregularexpression.h>

namespace RegexUtils {
    const QRegularExpression& indentedCodeBlock();
    const QRegularExpression& horizontalRule();
    const QRegularExpression& title();
    const QRegularExpression& codeBlock();
    const QRegularExpression& orderedList(bool ignore_start_space = false);
    const QRegularExpression& quote();
    const QRegularExpression& table();
    const QRegularExpression& tableHeaderSeparator();
    const QRegularExpression& unorderedList(bool ignore_start_space = false);
    const QRegularExpression& htmlBlock();
}