#include "section/CodeBlockSection.h"

#include <qcontainerfwd.h>
#include <qstring.h>

#include <utility>

CodeBlockSection& CodeBlockSection::operator=(const CodeBlockSection& other) {
    if (this != &other) { this->lines = other.lines; }
    return *this;
}

CodeBlockSection& CodeBlockSection::operator=(CodeBlockSection&& other) noexcept {
    if (this != &other) { this->lines = std::move(other.lines); }
    return *this;
}

QString CodeBlockSection::toHtml() {
    QString content("");
    for (auto& l : lines) {
        content.append(l);
        content.append("\n");
    }

    return QString("%1%2%3")
        .arg(this->before())
        .arg(content)
        .arg(this->after());
}

QString CodeBlockSection::before() {
    return QString("<pre><code>");
}

QString CodeBlockSection::after() {
    return QString("</code></pre>");
}