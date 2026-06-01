#include "SectionFactory.h"

#include "SectionType.h"
#include "utils/TypeUtils.h"

#include <qmap.h>
#include <qstring.h>

static const QMap<SectionType, std::function<bool(const QString &)>> checks = {
    {SectionType::HTML, TypeUtils::isHtmlBlock},
    {SectionType::INDENTED_CODE_BLOCK, TypeUtils::isIndentedCodeBlock},
    {SectionType::TITLE, TypeUtils::isTitle},
    {SectionType::CODE_BLOCK, TypeUtils::isCodeBlock},
    {SectionType::QUOTE, TypeUtils::isQuote},
    {SectionType::TABLE, TypeUtils::isTable},
    {SectionType::EMPTY,
        [](const QString &line) { return line.trimmed().isEmpty(); }},
};

SectionType SectionFactory::getLineType(const QString &line) {
    if (TypeUtils::isHorizontalRule(line)) return SectionType::HORIZONTAL_RULE;

    bool is_ordered = TypeUtils::isOrderedList(line),
         is_unordered = TypeUtils::isUnorderedList(line);
    if (is_ordered || is_unordered)
        return is_ordered ? SectionType::ORDERED_LIST : SectionType::UNORDERED_LIST;

    for (auto it = checks.begin(); it != checks.end(); ++it)
        if (it.value()(line)) return it.key();

    return SectionType::TEXT;
}
