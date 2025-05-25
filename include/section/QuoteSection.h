#pragma once

#include "export.h"
#include "section/AbstractSection.h"
#include "SectionType.h"

#include <qstring.h>

#include <utility>

class MARKDOWNPARSER_API QuoteSection : public AbstractSection2 {
public:
    static constexpr const SectionType type = SectionType::QUOTE;

    QuoteSection() = delete;
    QuoteSection(const LinesType& lines) : AbstractSection2(lines) {}
    QuoteSection(const QuoteSection& other) : AbstractSection2(other) {}
    QuoteSection(QuoteSection&& other) noexcept : AbstractSection2(std::move(other)) {}
    QuoteSection& operator=(const QuoteSection&);
    QuoteSection& operator=(QuoteSection&&) noexcept;

    QString toHtml();

private:
    QString before();
    QString after();
};