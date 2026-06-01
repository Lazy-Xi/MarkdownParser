#pragma once

#include "SectionType.h"
#include "export.h"

#include <qstring.h>

class MARKDOWNPARSER_API SectionFactory {
public:
    static SectionType getLineType(const QString &line);
};
