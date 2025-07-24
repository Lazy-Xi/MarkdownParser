#include "section/AbstractSection.h"

#include <qlist.h>
#include <qstring.h>

#include <utility>
#include <variant>

AbstractSection1::AbstractSection1(const LinesType& lines, bool escape_html)
	: lines(lines) {
	if (escape_html) {
		for (QString& line : this->lines) { line = line.toHtmlEscaped(); }
	}
}

AbstractSection1& AbstractSection1::operator=(const AbstractSection1& other) {
	if (this != &other) { this->lines = other.lines; }
	return *this;
}

AbstractSection1& AbstractSection1::operator=(AbstractSection1&& other) noexcept {
	if (this != &other) { this->lines = std::move(other.lines); }
	return *this;
}

AbstractSection2::AbstractSection2(const LinesType& lines) : lines(lines) {
	for (LinesType::Type& line : this->lines) {
		if (std::holds_alternative<QString>(line)) {
			line = std::get<QString>(line).toHtmlEscaped();
		}
	}
}

AbstractSection2& AbstractSection2::operator=(const AbstractSection2& other) {
	if (this != &other) { this->lines = other.lines; }
	return *this;
}

AbstractSection2& AbstractSection2::operator=(AbstractSection2&& other) noexcept {
	if (this != &other) { this->lines = std::move(other.lines); }
	return *this;
}
