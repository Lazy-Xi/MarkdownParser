#pragma once

#include "export.h"
#include "section/AbstractSection.h"
#include "SectionType.h"

#include <qmap.h>
#include <qstring.h>

#include <functional>
#include <variant>

struct MARKDOWNPARSER_API RawSection {
    std::variant<AbstractSection1::LinesType, AbstractSection2::LinesType> lines = {};
    SectionType type = SectionType::UNKNOWN;
};

class MARKDOWNPARSER_API SectionFactory {
public:
    static AbstractSection* createSection(const RawSection& section);
    static SectionType getLineType(const QString& line);

private:
    static const QMap<SectionType, std::function<bool(const QString&)>> checks;
    static const QMap<SectionType, std::function<AbstractSection* (const RawSection&)>> creators;
};