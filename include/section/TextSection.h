#pragma once

#include "export.h"
#include "section/AbstractSection.h"
#include "SectionType.h"

#include <qcontainerfwd.h>
#include <qstring.h>

#include <utility>

class MARKDOWNPARSER_API TextSection : public AbstractSection1 {
public:
    static constexpr const SectionType type = SectionType::TEXT;

    TextSection() = delete;
    TextSection(const QStringList& lines) : AbstractSection1(lines) {}
    TextSection(const TextSection& other) : AbstractSection1(other) {}
    TextSection(TextSection&& other) noexcept : AbstractSection1(std::move(other)) {}
    TextSection& operator=(const TextSection&);
    TextSection& operator=(TextSection&&) noexcept;

    QString toHtml();

private:
    QString before();
    QString after();
};