#pragma once

#include "export.h"
#include "section/AbstractSection.h"
#include "SectionType.h"

#include <qstring.h>

#include <utility>

class MARKDOWNPARSER_API HtmlSection : public AbstractSection1 {
public:
    static constexpr const SectionType type = SectionType::HTML;

    HtmlSection(const LinesType& lines) : AbstractSection1(lines, false) {}
    HtmlSection(const HtmlSection& other) : AbstractSection1(other) {}
    HtmlSection(HtmlSection&& other) noexcept : AbstractSection1(std::move(other)) {}
    HtmlSection& operator=(const HtmlSection& other);
    HtmlSection& operator=(HtmlSection&& other) noexcept;

    QString toHtml() override;

protected:
    QString before() override;
    QString after() override;
};