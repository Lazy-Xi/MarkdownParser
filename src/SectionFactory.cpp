#include "SectionFactory.h"

#include "section/AbstractSection.h"
#include "section/CodeBlockSection.h"
#include "section/HorizontalRuleSection.h"
#include "section/HtmlSection.h"
#include "section/OrderedListSection.h"
#include "section/QuoteSection.h"
#include "section/TableSection.h"
#include "section/TextSection.h"
#include "section/TitleSection.h"
#include "section/UnorderedListSection.h"
#include "SectionType.h"

#include <qmap.h>
#include <qregularexpression.h>
#include <qset.h>
#include <qstring.h>

#include <functional>
#include <variant>

const QMap<SectionType, std::function<AbstractSection* (const RawSection&)>> SectionFactory::creators = {
	{ SectionType::CODE_BLOCK, [](const RawSection& section) { return new CodeBlockSection(
		std::get<AbstractSection1::LinesType>(section.lines)); } },
	{ SectionType::HTML, [](const RawSection& section) {
		return new HtmlSection(std::get<AbstractSection1::LinesType>(section.lines)); } },
	{ SectionType::INDENTED_CODE_BLOCK, [](const RawSection& section) { return new CodeBlockSection(
		std::get<AbstractSection1::LinesType>(section.lines)); } },
	{ SectionType::HORIZONTAL_RULE, [](const RawSection& section) { return new HorizontalRuleSection(
		std::get<AbstractSection1::LinesType>(section.lines)); } },
	{ SectionType::ORDERED_LIST, [](const RawSection& section) { return new OrderedListSection(
		std::get<AbstractSection2::LinesType>(section.lines)); } },
	{ SectionType::QUOTE, [](const RawSection& section) { return new QuoteSection(
		std::get<AbstractSection2::LinesType>(section.lines)); } },
	{ SectionType::TABLE, [](const RawSection& section) { return new TableSection(
		std::get<AbstractSection1::LinesType>(section.lines)); } },
	{ SectionType::TEXT, [](const RawSection& section) { return new TextSection(
		std::get<AbstractSection1::LinesType>(section.lines)); } },
	{ SectionType::TITLE, [](const RawSection& section) { return new TitleSection(
		std::get<AbstractSection1::LinesType>(section.lines)); } },
	{ SectionType::UNORDERED_LIST, [](const RawSection& section) { return new UnorderedListSection(
		std::get<AbstractSection2::LinesType>(section.lines)); } },
};

const QMap<SectionType, std::function<bool(const QString&)>> SectionFactory::checks = {
	{ SectionType::HTML, &SectionFactory::isHtmlBlock },
	{ SectionType::INDENTED_CODE_BLOCK, SectionFactory::isIndentedCodeBlock },
	{ SectionType::HORIZONTAL_RULE, SectionFactory::isHorizontalRule },
	{ SectionType::TITLE, SectionFactory::isTitle },
	{ SectionType::CODE_BLOCK, SectionFactory::isCodeBlock },
	{ SectionType::QUOTE, SectionFactory::isQuote },
	{ SectionType::TABLE, SectionFactory::isTable },
	{ SectionType::EMPTY, [](const QString& line) { return line.trimmed().isEmpty(); } },
};

AbstractSection* SectionFactory::createSection(const RawSection& section) {
	if (creators.contains(section.type)) { return creators[section.type](section); }
	return creators[SectionType::TEXT](section);
}

SectionType SectionFactory::getLineType(const QString& line) {
	bool is_ordered_list = SectionFactory::isOrderedList(line),
		is_unordered_list = SectionFactory::isUnorderedList(line);

	if (is_ordered_list || is_unordered_list) {
		return is_ordered_list ? SectionType::ORDERED_LIST : SectionType::UNORDERED_LIST;
	}

	for (auto check = checks.begin(); check != checks.end(); check++) {
		if (check.value()(line) && !is_ordered_list && !is_unordered_list) {
			return check.key();
		}
	}

	return SectionType::TEXT;
}

QRegularExpression SectionFactory::createIndentedCodeBlockRegex() {
	return QRegularExpression(R"(^ {4}(.*)$)");
}

QRegularExpression SectionFactory::createHorizontalRuleRegex() {
	return QRegularExpression(R"(^ *([*\-_])(?: *\1){2,} *$)");
}

QRegularExpression SectionFactory::createTitleRegex() {
	return QRegularExpression(R"(^ {0,3}(#{1,6}) *(.*?) *$)");
}

QRegularExpression SectionFactory::createCodeBlockRegex() {
	return QRegularExpression(R"(^ {0,3}``` *(.*?) *$)");
}

QRegularExpression SectionFactory::createOrderedListRegex(bool ignore_start_space) {
	return QRegularExpression(QString(R"(^ %1(\d*)\. +(.*?) *$)")
		.arg(ignore_start_space ? "*" : "{0, 3}"));
}

QRegularExpression SectionFactory::createQuoteRegex() {
	return QRegularExpression(R"(^ {0,3}&gt; *(.*?) *$)");
}

QRegularExpression SectionFactory::createTableRegex() {
	return QRegularExpression(R"(^ {0,3}\|.*\|.*$)");
}

QRegularExpression SectionFactory::createTableHeaderSeparatorRegex() {
	return QRegularExpression(R"(^ {0,3}(?:\| *:?-*:? *)+ *$)");
}

QRegularExpression SectionFactory::createUnorderedListRegex(bool ignore_start_space) {
	return QRegularExpression(QString(R"(^ %1([*+-]) +(.*?) *$)")
		.arg(ignore_start_space ? "*" : "{0, 3}"));
}

QRegularExpression SectionFactory::createHtmlBlockRegex() {
	return QRegularExpression(R"(^\s*<(?!\!--)(?:[a-zA-Z][\w:\-]*)(\s[^>]*)?>)",
		QRegularExpression::CaseInsensitiveOption);
}

bool SectionFactory::isIndentedCodeBlock(const QString& line) {
	return createIndentedCodeBlockRegex().match(line).hasMatch();
}

bool SectionFactory::isHorizontalRule(const QString& line) {
	return createHorizontalRuleRegex().match(line).hasMatch();
}

bool SectionFactory::isTitle(const QString& line) {
	return createTitleRegex().match(line).hasMatch();
}

bool SectionFactory::isCodeBlock(const QString& line) {
	return createCodeBlockRegex().match(line).hasMatch();
}

bool SectionFactory::isOrderedList(const QString& line) {
	return createOrderedListRegex(true).match(line).hasMatch();
}

bool SectionFactory::isQuote(const QString& line) {
	return createQuoteRegex().match(line).hasMatch();
}

bool SectionFactory::isTable(const QString& line) {
	return createTableRegex().match(line).hasMatch();
}

bool SectionFactory::isTableHeaderSeparator(const QString& line) {
	return createTableHeaderSeparatorRegex().match(line).hasMatch();
}

bool SectionFactory::isUnorderedList(const QString& line) {
	return createUnorderedListRegex(true).match(line).hasMatch();
}

bool SectionFactory::isHtmlBlock(const QString& line) {
	const QString& trimmed = line.trimmed();
	return trimmed.startsWith('<') && !trimmed.startsWith("<!--") && createHtmlBlockRegex().match(line).hasMatch();
}

bool SectionFactory::isHtmlVoidTag(const QString& tag) {
	static const QSet<QString> voidTags = {
		"area", "base", "br", "col", "embed", "hr", "img", "input",
		"link", "meta", "param", "source", "track", "wbr"
	};
	return voidTags.contains(tag.toLower());
}