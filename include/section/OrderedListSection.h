#pragma once

#include "export.h"
#include "section/AbstractSection.h"
#include "SectionType.h"

#include <qstring.h>

#include <utility>

class MARKDOWNPARSER_API OrderedListSection : public AbstractSection2 {
public:
	static constexpr const SectionType type = SectionType::ORDERED_LIST;

	OrderedListSection() = delete;
	OrderedListSection(const LinesType& lines) : AbstractSection2(lines) {}
	OrderedListSection(const OrderedListSection& other) : AbstractSection2(other) {}
	OrderedListSection(OrderedListSection&& other) noexcept : AbstractSection2(std::move(other)) {}
	OrderedListSection& operator=(const OrderedListSection&);
	OrderedListSection& operator=(OrderedListSection&&) noexcept;

	QString toHtml();

private:
	QString before();
	QString after();
};