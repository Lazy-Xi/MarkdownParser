#include "section/HorizontalRuleSection.h"

#include <qstring.h>

#include <utility>

HorizontalRuleSection& HorizontalRuleSection::operator=(const HorizontalRuleSection& other) {
    if (this != &other) { this->lines = other.lines; }
    return *this;
}

HorizontalRuleSection& HorizontalRuleSection::operator=(HorizontalRuleSection&& other) noexcept {
    if (this != &other) { this->lines = std::move(other.lines); }
    return *this;
}

QString HorizontalRuleSection::toHtml() {
    return QString("%1<hr>%2")
        .arg(this->before())
        .arg(this->after());
}

QString HorizontalRuleSection::before() {
    return "";
}

QString HorizontalRuleSection::after() {
    return "";
}
