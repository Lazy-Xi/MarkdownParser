#include "section/TitleSection.h"

#include "InlineParser.h"
#include "utils/RegexUtils.h"

#include <qregularexpression.h>
#include <qstring.h>

#include <utility>

TitleSection& TitleSection::operator=(const TitleSection& other) {
	if (this != &other) { this->lines = other.lines; }
	return *this;
}

TitleSection& TitleSection::operator=(TitleSection&& other) noexcept {
	if (this != &other) { this->lines = std::move(other.lines); }
	return *this;
}

QString TitleSection::toHtml() {
	QRegularExpression re(RegexUtils::title());
	QString content = re.match(this->lines.at(0)).captured(2);

	return QString("%1%2%3")
		.arg(this->before())
		.arg(InlineParser(content).toHtml())
		.arg(this->after());
}

QString TitleSection::before() {
	QRegularExpression re(RegexUtils::title());
	QString content = re.match(this->lines.at(0)).captured(2);
	return QString(R"(<h%1 id="%2">)")
		.arg(this->getLevel())
		.arg(content.trimmed().toLower().replace(QRegularExpression(R"([\s_]+)"), "-"));
}

QString TitleSection::after() {
	return QString("</h%1>").arg(this->getLevel());
}

short TitleSection::getLevel() {
	if (this->level != -1) { return this->level; }
	QRegularExpression re(RegexUtils::title());
	this->level = re.match(this->lines.at(0)).captured(1).length();
	return this->level;
}