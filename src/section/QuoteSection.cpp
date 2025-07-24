#include "section/QuoteSection.h"

#include "InlineParser.h"
#include "section/AbstractSection.h"
#include "utils/RegexUtils.h"

#include <qregularexpression.h>
#include <qstring.h>

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
	for (auto& i : lines) {
		if (std::holds_alternative<QString>(i)) {
			QRegularExpressionMatch match = RegexUtils::quote().match(std::get<QString>(i));
			const QString& text = match.captured(1);

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
				content.append(" ");
			}
		}
		else if (std::holds_alternative<AbstractSection*>(i)) {
			if (!need_add_p_tag) {
				content.append("</p>");
				need_add_p_tag = true;
			}
			content.append(std::get<AbstractSection*>(i)->toHtml());
		}
	}

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