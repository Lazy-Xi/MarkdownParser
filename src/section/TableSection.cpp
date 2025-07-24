#include "section/TableSection.h"

#include "InlineParser.h"

#include <qcontainerfwd.h>
#include <qlist.h>
#include <qmap.h>
#include <qregularexpression.h>
#include <qstring.h>
#include <qtypes.h>

#include <utility>

TableSection& TableSection::operator=(const TableSection& other) {
	if (this != &other) { this->lines = other.lines; }
	return *this;
}

TableSection& TableSection::operator=(TableSection&& other) noexcept {
	if (this != &other) { this->lines = std::move(other.lines); }
	return *this;
}

QString TableSection::toHtml() {
	static const auto min = [](qsizetype a, qsizetype b) { return a < b ? a : b; };
	static const auto max = [](qsizetype a, qsizetype b) { return a > b ? a : b; };
	static const QMap<Align, QString> align_map = {
		{ Align::LEFT, "left" },
		{ Align::CENTER, "center" },
		{ Align::RIGHT, "right" }
	};

	QString content("");
	QStringList headers = this->getCeils(0);
	const QStringList& separators = this->getCeils(1);
	QList<Align> aligns;
	for (auto& i : separators) {
		const bool start = i.startsWith(":"), end = i.endsWith(":");
		if (start && end) { aligns.append(Align::CENTER); }
		else if (end) { aligns.append(Align::RIGHT); }
		else { aligns.append(Align::LEFT); }
	}

	const qsizetype col_count = max(headers.size(), aligns.size());
	if (headers.size() < col_count) {
		for (qsizetype i = headers.size(); i < col_count; i++) {
			headers.append("");
		}
	}
	else if (aligns.size() < col_count) {
		for (qsizetype i = aligns.size(); i < col_count; i++) {
			aligns.append(Align::LEFT);
		}
	}

	content.append("<thead><tr>");
	for (qsizetype i = 0; i < col_count; i++) {
		content.append(QString(R"(<th align="%1">%2</th>)")
			.arg(align_map[aligns[i]])
			.arg(InlineParser(headers[i]).toHtml()));
	}
	content.append("</tr></thead><tbody>");
	for (qsizetype i = 2; i < this->lines.size(); i++) {
		content.append("<tr>");
		const QStringList& ceils = this->getCeils(i);
		for (qsizetype j = 0; j < min(col_count, ceils.size()); j++) {
			content.append(QString(R"(<td align="%1">%2</td>)")
				.arg(align_map[aligns[j]])
				.arg(InlineParser(ceils[j]).toHtml()));
		}
		content.append("</tr>");
	}
	content.append("</tbody>");

	return QString("%1%2%3")
		.arg(this->before())
		.arg(content)
		.arg(this->after());
}

QString TableSection::before() {
	return QString("<table>");
}

QString TableSection::after() {
	return QString("</table>");
}

QStringList TableSection::getCeils(qsizetype i) {
	QRegularExpression re(R"(\|(?= *\S) *(.*?)(?= *(?<!\\)\|| *$))");
	QRegularExpressionMatchIterator it = re.globalMatch(this->lines.at(i));
	QStringList matches{};
	while (it.hasNext()) {
		const QRegularExpressionMatch& match = it.next();
		if (match.hasMatch()) { matches.append(match.captured(1)); }
	}
	return matches;
}