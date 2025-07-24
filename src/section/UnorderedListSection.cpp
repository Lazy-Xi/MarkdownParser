#include "section/UnorderedListSection.h"

#include "InlineParser.h"
#include "section/AbstractSection.h"
#include "utils/RegexUtils.h"

#include <qstring.h>

#include <utility>
#include <variant>

UnorderedListSection& UnorderedListSection::operator=(const UnorderedListSection& other) {
	if (this != &other) { this->lines = other.lines; }
	return *this;
}

UnorderedListSection& UnorderedListSection::operator=(UnorderedListSection&& other) noexcept {
	if (this != &other) { this->lines = std::move(other.lines); }
	return *this;
}

QString UnorderedListSection::toHtml() {
	QString content("");
	bool flag = false;
	for (auto& i : this->lines) {
		if (std::holds_alternative<QString>(i)) {
			if (flag) { content.append("</li>"); }
			const QString& text = RegexUtils::unorderedList(true).match(std::get<QString>(i)).captured(2);
			content.append(QString("<li>%1").arg(InlineParser(text).toHtml()));
		}
		else if (std::holds_alternative<AbstractSection*>(i)) {
			content.append(std::get<AbstractSection*>(i)->toHtml());
		}
		flag = true;
	}

	return QString("%1%2%3")
		.arg(this->before())
		.arg(content)
		.arg(this->after());
}

QString UnorderedListSection::before() {
	return QString("<ul>");
}

QString UnorderedListSection::after() {
	return QString("</ul>");
}