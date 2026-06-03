#include "utils/RegexUtils.h"

namespace RegexUtils {

const QRegularExpression &indentedCodeBlock() {
    static const QRegularExpression re(R"(^ {4}(.*)$)");
    return re;
}

const QRegularExpression &horizontalRule() {
    static const QRegularExpression re(
        R"(^ *([*\-_])(?: *\1){2,} *$)");
    return re;
}

const QRegularExpression &title() {
    // Trailing " #+" is stripped per CommonMark spec
    static const QRegularExpression re(
        R"(^ {0,3}(#{1,6}) +(.*?)(?:\s+#+)? *$)");
    return re;
}

const QRegularExpression &codeBlock() {
    static const QRegularExpression re(R"(^ {0,3}``` *(.*?) *$)");
    return re;
}

const QRegularExpression &orderedList(bool ignore_start_space) {
    static const QRegularExpression re_0(R"(^ *(\d*)\. +(.*?) *$)");
    static const QRegularExpression re_1(
        R"(^ {0,3}(\d+)\. +(.*?) *$)");
    return ignore_start_space ? re_0 : re_1;
}

const QRegularExpression &quote() {
    // Match the quote marker both before HTML-escaping (raw '>', as
    // seen during line classification) and after (escaped '&gt;', as
    // seen during rendering).
    static const QRegularExpression re(
        R"(^ {0,3}(?:>|&gt;) *(.*?) *$)");
    return re;
}

const QRegularExpression &table() {
    static const QRegularExpression re(R"(^ {0,3}\|.*\|.*$)");
    return re;
}

const QRegularExpression &tableHeaderSeparator() {
    static const QRegularExpression re(
        R"(^ {0,3}(?:\| *:?-*:? *)+ *$)");
    return re;
}

const QRegularExpression &unorderedList(bool ignore_start_space) {
    static const QRegularExpression re_0(R"(^ *([*+-]) +(.*?) *$)");
    static const QRegularExpression re_1(
        R"(^ {0,3}([*+-]) +(.*?) *$)");
    return ignore_start_space ? re_0 : re_1;
}

const QRegularExpression &htmlBlock() {
    static const QRegularExpression re(
        R"(^\s*(?:<!--.*?-->|<(?!\!--)(?:[a-zA-Z][\w:\-]*)(\s[^>]*)?>))",
        QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption);
    return re;
}

} // namespace RegexUtils