#include "section/QuoteSection.h"

#include "InlineParser.h"
#include "section/AbstractSection.h"
#include "utils/RegexUtils.h"

#include <qregularexpression.h>
#include <qstring.h>
#include <qtypes.h>

#include <utility>
#include <variant>

QuoteSection& QuoteSection::operator=(const QuoteSection& other) {
	if (this != &other) { this->lines = other.lines; }
	return *this;
}

QuoteSection& QuoteSection::operator=(QuoteSection&& other) noexcept {
	if (this != &other) { this->lines = std::move(other.lines); }
	return *this;
}

QString QuoteSection::toHtml() {
	QString content("");
	bool need_add_p_tag = true;
	for (qsizetype i = 0; i < lines.size(); ++i) {
		if (std::holds_alternative<QString>(lines[i])) {
			QRegularExpressionMatch match = RegexUtils::quote().match(std::get<QString>(lines[i]));
			const QString& text = match.hasMatch() ? match.captured(1) : std::get<QString>(lines[i]).trimmed();

			if (text.isEmpty()) {
				if (!need_add_p_tag) {
					content.append("</p>");
					need_add_p_tag = true;
				}
			}
			else {
				if (need_add_p_tag) {
					content.append("<p>");
					need_add_p_tag = false;
				}
				content.append(InlineParser(text).toHtml());
				if (i + 1 < lines.size() && std::holds_alternative<QString>(lines[i + 1])) {
					content.append(" ");
				}
			}
		}
		else if (std::holds_alternative<AbstractSection*>(lines[i])) {
			if (!need_add_p_tag) {
				content.append("</p>");
				need_add_p_tag = true;
			}
			content.append(std::get<AbstractSection*>(lines[i])->toHtml());
		}
	}
	content.append("</p>");

	return QString("%1%2%3")
		.arg(this->before())
		.arg(content)
		.arg(this->after());
}

QString QuoteSection::before() {
	return QString("<blockquote>");
}

QString QuoteSection::after() {
	return QString("</blockquote>");
}