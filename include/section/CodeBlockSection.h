#pragma once

#include "section/AbstractSection.h"
#include "SectionType.h"

#include <qcontainerfwd.h>
#include <qstring.h>

#include <utility>

class CodeBlockSection : public AbstractSection1 {
public:
    static constexpr const SectionType type = SectionType::CODE_BLOCK;

    CodeBlockSection() = delete;
    CodeBlockSection(const QStringList& lines) : AbstractSection1(lines) {}
    CodeBlockSection(const CodeBlockSection& other) : AbstractSection1(other) {}
    CodeBlockSection(CodeBlockSection&& other) noexcept : AbstractSection1(std::move(other)) {}
    CodeBlockSection& operator=(const CodeBlockSection&);
    CodeBlockSection& operator=(CodeBlockSection&&) noexcept;

    QString toHtml();

private:
    QString before();
    QString after();
};