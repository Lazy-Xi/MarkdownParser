#include "section/HtmlSection.h"

#include <qstring.h>
#include <utility>

HtmlSection& HtmlSection::operator=(const HtmlSection& other) {
	if (this != &other) { this->lines = other.lines; }
	return *this;
}

HtmlSection& HtmlSection::operator=(HtmlSection&& other) noexcept {
	if (this != &other) { this->lines = std::move(other.lines); }
	return *this;
}

QString HtmlSection::toHtml() {
	QString content;
	for (const auto& l : this->lines) {
		content.append(l);
		content.append("\n");
	}
	return QString("%1%2%3")
		.arg(this->before())
		.arg(content)
		.arg(this->after());
}

QString HtmlSection::before() { 
	return "<div>"; 
}

QString HtmlSection::after() { 
	return "</div>"; 
}