#pragma once

#include "export.h"
#include "section/AbstractSection.h"
#include "SectionType.h"

#include <qmap.h>
#include <qstring.h>
#include <qstringlist.h>

#include <functional>
#include <variant>

class MARKDOWNPARSER_API SectionParser {
public:
	SectionParser() = delete;
	SectionParser(QStringListIterator& it) : it(it) {}
	SectionParser(const SectionParser&) = delete;
	SectionParser(SectionParser&&) noexcept = delete;
	SectionParser& operator=(const SectionParser&) = delete;
	SectionParser& operator=(SectionParser&&) noexcept = delete;

	AbstractSection* next();

private:
	QStringListIterator& it;

	const QMap<SectionType, std::function<
		std::variant<AbstractSection1::LinesType, AbstractSection2::LinesType>()>> parsers = {
			{ SectionType::CODE_BLOCK, std::bind(&SectionParser::parseCodeBlock, this) },
			{ SectionType::INDENTED_CODE_BLOCK, std::bind(&SectionParser::parseIndentedCodeBlock, this) },
			{ SectionType::HORIZONTAL_RULE, [this]() { return this->parseSingleLine(SectionType::HORIZONTAL_RULE); } },
			{ SectionType::HTML, std::bind(&SectionParser::parseHtmlBlock, this) },
			{ SectionType::ORDERED_LIST, [this]() { return this->parseList(SectionType::ORDERED_LIST); } },
			{ SectionType::QUOTE, std::bind(&SectionParser::parseQuote, this) },
			{ SectionType::TABLE, [this]() { return this->parseTable(false); } },
			{ SectionType::TEXT, std::bind(&SectionParser::parseText, this) },
			{ SectionType::TITLE, [this]() { return this->parseSingleLine(SectionType::TITLE); } },
			{ SectionType::UNORDERED_LIST, [this]() { return this->parseList(SectionType::UNORDERED_LIST); } },
	};

	AbstractSection1::LinesType parseSingleLine(SectionType type);
	AbstractSection1::LinesType parseCodeBlock();
	AbstractSection1::LinesType parseHtmlBlock();
	AbstractSection1::LinesType parseIndentedCodeBlock();
	AbstractSection1::LinesType parseTable(bool trimmed = false);
	AbstractSection1::LinesType parseText();

	AbstractSection2::LinesType parseList(SectionType type);
	AbstractSection2::LinesType parseQuote();

	AbstractSection* tryParseTable(bool trimmed = false);

	static inline QString trimmedTransform(const QString& line, bool trimmed);
};