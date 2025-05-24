#pragma once

#include "section/AbstractSection.h"
#include "SectionType.h"

#include <qstring.h>

#include <utility>

class UnorderedListSection : public AbstractSection2 {
public:
    static constexpr const SectionType type = SectionType::UNORDERED_LIST;

    UnorderedListSection() = delete;
    UnorderedListSection(const LinesType& lines) : AbstractSection2(lines) {}
    UnorderedListSection(const UnorderedListSection& other) : AbstractSection2(other) {}
    UnorderedListSection(UnorderedListSection&& other) noexcept : AbstractSection2(std::move(other)) {}
    UnorderedListSection& operator=(const UnorderedListSection&);
    UnorderedListSection& operator=(UnorderedListSection&&) noexcept;

    QString toHtml();

private:
    QString before();
    QString after();
};