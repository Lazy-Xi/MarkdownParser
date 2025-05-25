#pragma once

#include "export.h"
#include "SectionType.h"

#include <qcontainerfwd.h>
#include <qlist.h>
#include <qstring.h>

#include <utility>
#include <variant>

class MARKDOWNPARSER_API AbstractSection {
public:
    static constexpr const SectionType type = SectionType::UNKNOWN;

    virtual QString toHtml() = 0;

protected:
    virtual QString before() = 0;
    virtual QString after() = 0;
};

class MARKDOWNPARSER_API AbstractSection1 : public AbstractSection {
public:
    static constexpr const SectionType type = SectionType::UNKNOWN;

    using LinesType = QStringList;

    AbstractSection1(const LinesType& lines);
    AbstractSection1(const AbstractSection1& other) : lines(other.lines) {}
    AbstractSection1(AbstractSection1&& other) noexcept : lines(std::move(other.lines)) {}
    AbstractSection1& operator=(const AbstractSection1& other);
    AbstractSection1& operator=(AbstractSection1&& other) noexcept;

    virtual QString toHtml() = 0;

protected:
    LinesType lines;

    virtual QString before() = 0;
    virtual QString after() = 0;
};

class MARKDOWNPARSER_API AbstractSection2 : public AbstractSection {
public:
    static constexpr const SectionType type = SectionType::UNKNOWN;

    using LinesType = QList<std::variant<QString, AbstractSection*>>;

    AbstractSection2(const LinesType& lines);
    AbstractSection2(const AbstractSection2& other) : lines(other.lines) {}
    AbstractSection2(AbstractSection2&& other) noexcept : lines(std::move(other.lines)) {}
    AbstractSection2& operator=(const AbstractSection2& other);
    AbstractSection2& operator=(AbstractSection2&& other) noexcept;

    virtual QString toHtml() = 0;

protected:
    LinesType lines;

    virtual QString before() = 0;
    virtual QString after() = 0;
};