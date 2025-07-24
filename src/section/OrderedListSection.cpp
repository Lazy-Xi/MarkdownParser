#include "section/OrderedListSection.h"

#include "InlineParser.h"
#include "section/AbstractSection.h"
#include "utils/RegexUtils.h"

#include <qstring.h>

#include <utility>
#include <variant>

OrderedListSection& OrderedListSection::operator=(const OrderedListSection& other) {
	if (this != &other) { this->lines = other.lines; }
	return *this;
}

OrderedListSection& OrderedListSection::operator=(OrderedListSection&& other) noexcept {
	if (this != &other) { this->lines = std::move(other.lines); }
	return *this;
}

QString OrderedListSection::toHtml() {
	QString content("");
	bool flag = false;
	for (auto& i : lines) {
		if (std::holds_alternative<QString>(i)) {
			if (flag) { content.append("</li>"); }
			const QString& text = RegexUtils::orderedList(true).match(std::get<QString>(i)).captured(2);
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

QString OrderedListSection::before() {
	return QString("<ol>");
}

QString OrderedListSection::after() {
	return QString("</ol>");
}