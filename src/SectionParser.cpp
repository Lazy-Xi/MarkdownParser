#include "SectionParser.h"

#include "section/AbstractSection.h"
#include "section/TextSection.h"
#include "SectionFactory.h"
#include "SectionType.h"
#include "utils/RegexUtils.h"
#include "utils/TypeUtils.h"

#include <qchar.h>
#include <qcontainerfwd.h>
#include <qmap.h>
#include <qregularexpression.h>
#include <qset.h>
#include <qstack.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtypes.h>

#include <stdexcept>
#include <variant>

AbstractSection* SectionParser::next() {
	while (it.hasNext() && it.peekNext().isEmpty()) {
		it.next();
	}
	if (!it.hasNext()) {
		return nullptr;
	}

	const QString& line = it.peekNext();
	if (line == "<!--TOC-->" || line == "<!--/TOC-->") {
		it.next();
		return this->next();
	}

	RawSection section;
	section.type = SectionFactory::getLineType(line);
	if (section.type == SectionType::TABLE) {
		return this->tryParseTable();
	}
	section.lines = parsers[section.type]();
	return SectionFactory::createSection(section);
}

AbstractSection1::LinesType SectionParser::parseCodeBlock() {
	QStringList lines {};
	it.next();
	while (it.hasNext()) {
		const QString& line = it.next();
		if (TypeUtils::isCodeBlock(line)) {
			break;
		}
		lines.append(line);
	}
	return lines;
}

AbstractSection1::LinesType SectionParser::parseIndentedCodeBlock() {
	QStringList lines {};
	lines.append(it.next().mid(4));
	bool is_last_empty = false;
	while (it.hasNext()) {
		if (is_last_empty) {
			lines.append("");
			is_last_empty = false;
		}

		const QString& line = it.peekNext();
		const SectionType& type = SectionFactory::getLineType(line);
		if (type == SectionType::EMPTY) {
			it.next();
			is_last_empty = true;
		}
		else if (type != SectionType::INDENTED_CODE_BLOCK) {
			break;
		}
		else {
			lines.append(line.mid(4));
			it.next();
		}
	}
	if (lines.last().isEmpty()) {
		lines.pop_back();
	}
	return lines;
}

AbstractSection1::LinesType SectionParser::parseSingleLine(SectionType type) {
	static const QSet<SectionType> single_line_types { SectionType::HORIZONTAL_RULE, SectionType::TITLE };
	if (!single_line_types.contains(type)) {
		throw std::runtime_error("Invalid single line type");
	}
	return { it.next() };
}

AbstractSection1::LinesType SectionParser::parseTable(bool trimmed) {
	QStringList lines {};
	while (it.hasNext()) {
		if (!TypeUtils::isTable(it.peekNext())) {
			break;
		}
		lines.append(it.next());
	}
	return lines;
}

AbstractSection1::LinesType SectionParser::parseText() {
	QStringList lines {};
	while (it.hasNext()) {
		if (SectionFactory::getLineType(it.peekNext()) != SectionType::TEXT) {
			break;
		}
		lines.append(it.next().trimmed());
	}
	return lines;
}

AbstractSection2::LinesType SectionParser::parseList(SectionType type) {
	if (type != SectionType::ORDERED_LIST && type != SectionType::UNORDERED_LIST) {
		throw std::runtime_error("Invalid list type");
	}

	static const QMap<SectionType, SectionType> another_map {
		{ SectionType::ORDERED_LIST, SectionType::UNORDERED_LIST },
		{ SectionType::UNORDERED_LIST, SectionType::ORDERED_LIST } };
	static const auto getLineType = [](const QString& line) {
		return SectionFactory::getLineType(line.trimmed());
	};
	static const auto getIndentLevel = [](const QString& line) {
		qsizetype count = 0;
		for (QChar ch : line) {
			if (ch == ' ') count++;
			else break;
		}
		return count >> 1;
	};

	AbstractSection2::LinesType lines {};
	qsizetype base_indent_level = -1;
	while (it.hasNext()) {
		const QString& line = it.peekNext();
		const SectionType& line_type = getLineType(line);
		const bool is_allowed = getLineType(line) != SectionType::TITLE && getLineType(line) != SectionType::EMPTY;
		const qsizetype indent_level = getIndentLevel(line);

		if (base_indent_level == -1 && line_type == type) {
			base_indent_level = indent_level;
		}
		if (!is_allowed || indent_level < base_indent_level ||
			(indent_level == base_indent_level && line_type == another_map[type])) {
			break;
		}

		if (indent_level > base_indent_level) {
			QStringList _lines {};
			_lines.append(it.next().mid(indent_level << 1));
			while (it.hasNext()) {
				const QString& _line = it.peekNext();
				const SectionType& _type = getLineType(_line);
				if (type == SectionType::EMPTY) {
					it.next();
				}
				else if (_type == SectionType::TITLE || _type == SectionType::EMPTY
					|| getIndentLevel(_line) < indent_level) {
					break;
				}
				_lines.append(it.next().mid(indent_level << 1));
			}
			QStringListIterator _it(_lines);
			SectionParser parser(_it);
			for (auto n = parser.next(); n != nullptr; n = parser.next()) {
				lines.append(n);
			}
		}
		else if (line_type == type) {
			QString content(it.next());
			while (it.hasNext()) {
				const QString& _line = it.peekNext().trimmed();
				const SectionType& _type = getLineType(_line);
				if (_type != SectionType::TEXT || getIndentLevel(_line) < indent_level) {
					break;
				}
				content.append(" ");
				content.append(_line.mid(indent_level << 1));
				it.next();
			}
			lines.append(content);
		}
		else if (line_type == SectionType::TABLE) {
			lines.append(this->tryParseTable(true));
		}
		else {
			QStringList _lines {};
			_lines.append(it.next().trimmed());
			while (it.hasNext()) {
				const QString& _line = it.peekNext().trimmed();
				const SectionType& _type = getLineType(_line);
				if (_type == SectionType::EMPTY) {
					it.next();
				}
				else if (_type != SectionType::TEXT || getIndentLevel(_line) < indent_level) {
					break;
				}
				_lines.append(_line.mid(indent_level << 1));
				it.next();
			}
			lines.append(new TextSection(_lines));
		}
	}
	return lines;
}

AbstractSection2::LinesType SectionParser::parseQuote() {
	static const QSet<SectionType> finished_types = { SectionType::EMPTY,
		SectionType::TITLE };
	static const auto getLineType = [](const QString& line) {
		return SectionFactory::getLineType(line.trimmed());
	};

	AbstractSection2::LinesType lines {};
	while (it.hasNext()) {
		const QString& line = it.peekNext();
		const SectionType& type = getLineType(line);
		if (finished_types.contains(type)) {
			break;
		}
		if (type == SectionType::QUOTE) {
			QStringList _lines {};
			QRegularExpression re(RegexUtils::quote());
			_lines.append(re.match(line).captured(1));
			it.next();
			while (it.hasNext()) {
				const QString& _line = it.peekNext().trimmed();
				const SectionType& _type = getLineType(_line);
				if (_type == SectionType::QUOTE) {
					_lines.append(re.match(_line).captured(1));
				}
				else if (_type == SectionType::TEXT) {
					_lines.last().append(_line);
					_lines.last().append(" ");
				}
				else {
					break;
				}
				it.next();
			}
			QStringListIterator _it(_lines);
			SectionParser parser(_it);
			for (auto n = parser.next(); n != nullptr; n = parser.next()) {
				lines.append(n);
			}
			continue;
		}
		lines.append(this->next());
	}
	return lines;
}

AbstractSection* SectionParser::tryParseTable(bool trimmed) {
	RawSection section;
	QStringList lines {};
	bool is_seperator_break = false;
	lines.append(trimmedTransform(it.next(), trimmed));
	while (it.hasNext()) {
		const QString& line = trimmedTransform(it.peekNext(), trimmed);
		const SectionType& type = SectionFactory::getLineType(line);
		if (type != SectionType::TABLE) {
			break;
		}
		else if (TypeUtils::isTableHeaderSeparator(line)) {
			is_seperator_break = true;
			break;
		}
		else {
			lines.append(line);
			it.next();
		}
	}

	if (lines.size() != 1 || !is_seperator_break) {
		if (is_seperator_break) {
			it.previous();
			lines.pop_back();
		}
		section.type = SectionType::TEXT;
		section.lines = lines;
	}
	else {
		it.previous();
		section.type = SectionType::TABLE;
		section.lines = this->parseTable();
	}
	return SectionFactory::createSection(section);
}

AbstractSection1::LinesType SectionParser::parseHtmlBlock() {
	QStringList lines;
	QStack<QString> stack;
	QRegularExpression start_tag_regex(R"(<([a-zA-Z][\w:\-]*)[^>]*>)");
	QRegularExpression end_tag_regex(R"(</([a-zA-Z][\w:\-]*)\s*>)");
	QRegularExpression void_tag_regex(R"(<([a-zA-Z][\w:\-]*)[^>]*/?>)");

	auto needBreak = [&]() {
		return stack.isEmpty() && !it.hasNext() && it.peekNext().trimmed().isEmpty();
	};

	while (it.hasNext()) {
		const QString& line = it.peekNext();
		lines.append(it.next());

		auto void_tag_match = void_tag_regex.match(line);
		if (void_tag_match.hasMatch()) {
			const QString& tag = void_tag_match.captured(1);
			if (line.trimmed().endsWith("/>") || TypeUtils::isHtmlVoidTag(tag)) {
				if (needBreak()) {
					break;
				}
				continue;
			}
		}

		auto start_tag_match = start_tag_regex.match(line);
		if (start_tag_match.hasMatch()) {
			const QString& tag = start_tag_match.captured(1);
			if (TypeUtils::isHtmlVoidTag(tag) && !line.trimmed().endsWith("/>")) {
				if (needBreak()) {
					break;
				}
				continue;
			}
			stack.push(tag);
		}

		auto end_tag_match = end_tag_regex.match(line);
		if (end_tag_match.hasMatch() && !stack.isEmpty() && stack.top() == end_tag_match.captured(1)) {
			stack.pop();
			if (stack.isEmpty()) {
				break;
			}
		}

		if (stack.isEmpty() && line.trimmed().isEmpty()) {
			break;
		}
	}
	return lines;
}

QString SectionParser::trimmedTransform(const QString& line, bool trimmed) {
	return trimmed ? line.trimmed() : line;
}