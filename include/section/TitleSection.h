#pragma once

#include "section/AbstractSection.h"
#include "SectionType.h"

#include <qcontainerfwd.h>
#include <qstring.h>

#include <utility>

class TitleSection : public AbstractSection1 {
public:
    static constexpr const SectionType type = SectionType::TITLE;

    TitleSection() = delete;
    TitleSection(const QStringList& lines) : AbstractSection1(lines) {}
    TitleSection(const TitleSection& other) : AbstractSection1(other) {}
    TitleSection(TitleSection&& other) noexcept : AbstractSection1(std::move(other)) {}
    TitleSection& operator=(const TitleSection&);
    TitleSection& operator=(TitleSection&&) noexcept;

    QString toHtml();

private:
    short level = -1;

    QString before();
    QString after();

    short getLevel();
};