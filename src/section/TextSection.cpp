#include "section/TextSection.h"

#include "InlineParser.h"

#include <qstring.h>

#include <utility>

TextSection& TextSection::operator=(const TextSection& other) {
	if (this != &other) {
		this->lines = other.lines;
	}
	return *this;
}

TextSection& TextSection::operator=(TextSection&& other) noexcept {
	if (this != &other) {
		this->lines = std::move(other.lines);
	}
	return *this;
}

QString TextSection::toHtml() {
	QString content("");
	for (auto& l : this->lines) {
		content.append(l);
		if (!l.endsWith("\n")) {
			content.append(" ");
		}
	}

	return QString("%1%2%3")
		.arg(this->before())
		.arg(InlineParser(content.trimmed()).toHtml())
		.arg(this->after());
}

QString TextSection::before() {
	return QString("<p>");
}

QString TextSection::after() {
	return QString("</p>");
}