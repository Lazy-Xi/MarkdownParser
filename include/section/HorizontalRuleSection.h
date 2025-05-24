#pragma once

#include "section/AbstractSection.h"
#include "SectionType.h"

#include <qcontainerfwd.h>
#include <qstring.h>

#include <utility>

class HorizontalRuleSection : public AbstractSection1 {
public:
    static constexpr const SectionType type = SectionType::HORIZONTAL_RULE;

    HorizontalRuleSection() = delete;
    HorizontalRuleSection(const QStringList& lines) : AbstractSection1(lines) {}
    HorizontalRuleSection(const HorizontalRuleSection& other) : AbstractSection1(other) {}
    HorizontalRuleSection(HorizontalRuleSection&& other) noexcept : AbstractSection1(std::move(other)) {}
    HorizontalRuleSection& operator=(const HorizontalRuleSection&);
    HorizontalRuleSection& operator=(HorizontalRuleSection&&) noexcept;

    QString toHtml();

private:
    QString before();
    QString after();
};