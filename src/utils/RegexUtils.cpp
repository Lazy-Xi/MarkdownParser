#include "utils/RegexUtils.h"

#include <qregularexpression.h>

namespace RegexUtils {

const QRegularExpression& indentedCodeBlock() {
	static const QRegularExpression re(R"(^ {4}(.*)$)");
	return re;
}

const QRegularExpression& horizontalRule() {
	static const QRegularExpression re(R"(^ *([*\-_])(?: *\1){2,} *$)");
	return re;
}

const QRegularExpression& title() {
	static const QRegularExpression re(R"(^ {0,3}(#{1,6}) *(.*?) *$)");
	return re;
}

const QRegularExpression& codeBlock() {
	static const QRegularExpression re(R"(^ {0,3}``` *(.*?) *$)");
	return re;
}

const QRegularExpression& orderedList(bool ignore_start_space) {
	static const QRegularExpression re_0(R"(^ *(\d*)\. +(.*?) *$)");
	static const QRegularExpression re_1(R"(^ {0,3}(\d+)\. +(.*?) *$)");
	return ignore_start_space ? re_0 : re_1;
}

const QRegularExpression& quote() {
	static const QRegularExpression re(R"(^ {0,3}&gt; *(.*?) *$)");
	return re;
}

const QRegularExpression& table() {
	static const QRegularExpression re(R"(^ {0,3}\|.*\|.*$)");
	return re;
}

const QRegularExpression& tableHeaderSeparator() {
	static const QRegularExpression re(R"(^ {0,3}(?:\| *:?-*:? *)+ *$)");
	return re;
}

const QRegularExpression& unorderedList(bool ignore_start_space) {
	static const QRegularExpression re_0(R"(^ *([*+-]) +(.*?) *$)");
	static const QRegularExpression re_1(R"(^ {0,3}([*+-]) +(.*?) *$)");
	return ignore_start_space ? re_0 : re_1;
}

const QRegularExpression& htmlBlock() {
	static const QRegularExpression re(R"(^\s*(?:<!--.*?-->|<(?!\!--)(?:[a-zA-Z][\w:\-]*)(\s[^>]*)?>))",
		QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
	return re;
}

}