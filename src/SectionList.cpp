#include "SectionList.h"

#include "SectionParser.h"

#include <qcontainerfwd.h>
#include <qstring.h>
#include <qstringlist.h>

#include <utility>

SectionList& SectionList::operator=(const SectionList& other) {
    if (this != &other) { this->markdown = other.markdown; }
    return *this;
}

SectionList& SectionList::operator=(SectionList&& other) noexcept {
    if (this != &other) { this->markdown = std::move(other.markdown); }
    return *this;
}

QString SectionList::toHtml() {
    QStringList lines(markdown.split('\n'));
    QStringListIterator it(lines);
    SectionParser spliter(it);

    QString content("");
    for (auto n = spliter.next(); n != nullptr; n = spliter.next()) {
        this->append(n);
        content.append(n->toHtml());
    }

    return QString("%1%2%3")
        .arg(this->before())
        .arg(content)
        .arg(this->after());
}

QString SectionList::before() {
    return "";
}

QString SectionList::after() {
    return "";
}
