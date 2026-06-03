#pragma once

#include "SectionType.h"
#include "export.h"

#include <QString>

class MARKDOWNPARSER_API SectionFactory {
public:
    static SectionType getLineType(const QString &line);
};
