#pragma once

#include "export.h"
#include "section/AbstractSection.h"
#include "SectionType.h"

#include <qcontainerfwd.h>
#include <qstring.h>
#include <qtypes.h>

#include <utility>

class MARKDOWNPARSER_API TableSection : public AbstractSection1 {
public:
	static constexpr const SectionType type = SectionType::TABLE;

	enum class Align {
		LEFT,
		CENTER,
		RIGHT
	};

	TableSection() = delete;
	TableSection(const QStringList& lines) : AbstractSection1(lines) {}
	TableSection(const TableSection& other) : AbstractSection1(other) {}
	TableSection(TableSection&& other) noexcept : AbstractSection1(std::move(other)) {}
	TableSection& operator=(const TableSection&);
	TableSection& operator=(TableSection&&) noexcept;

	QString toHtml();

private:
	QString before();
	QString after();

	QStringList getCeils(qsizetype i);
};