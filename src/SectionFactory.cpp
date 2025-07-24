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
#include "utils/TypeUtils.h"

#include <qmap.h>
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
	{ SectionType::HTML, TypeUtils::isHtmlBlock },
	{ SectionType::INDENTED_CODE_BLOCK, TypeUtils::isIndentedCodeBlock },
	{ SectionType::HORIZONTAL_RULE, TypeUtils::isHorizontalRule },
	{ SectionType::TITLE, TypeUtils::isTitle },
	{ SectionType::CODE_BLOCK, TypeUtils::isCodeBlock },
	{ SectionType::QUOTE, TypeUtils::isQuote },
	{ SectionType::TABLE, TypeUtils::isTable },
	{ SectionType::EMPTY, [](const QString& line) { return line.trimmed().isEmpty(); } },
};

AbstractSection* SectionFactory::createSection(const RawSection& section) {
	if (creators.contains(section.type)) { return creators[section.type](section); }
	return creators[SectionType::TEXT](section);
}

SectionType SectionFactory::getLineType(const QString& line) {
	bool is_ordered_list = TypeUtils::isOrderedList(line),
		is_unordered_list = TypeUtils::isUnorderedList(line);

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