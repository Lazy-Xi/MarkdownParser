#include "test_Section.h"

#include "SectionParser.h"
#include "SectionType.h"
#include "config.h"
#include "render/HtmlRenderer.h"

#include <qdir.h>
#include <qfile.h>
#include <qiodevice.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtestcase.h>
#include <qtestdata.h>

static QStringList toMarkdownLines(const QStringList &lines, SectionType type) {
    if (type == SectionType::CODE_BLOCK) {
        QStringList out;
        out << "```" << lines << "```";
        return out;
    }
    return lines;
}

static QString renderLines(const QStringList &lines) {
    QStringListIterator it(lines);
    SectionParser parser(it);
    QString out;
    for (auto n = parser.nextNode(); n; n = parser.nextNode())
        out += HtmlRenderer().render(*n);
    return out;
}

// Flatten JSON lines array (may contain nested section objects) into raw
// Markdown lines. Nested list items get 2-space indent; nested quotes get "> ".
static QStringList flattenLines(const QJsonValue &val, int indent = 0,
    int quote_depth = 0);
static QStringList flattenLines(const QJsonValue &val, int indent,
    int quote_depth) {
    QStringList result;
    const QString pad(indent * 2, ' ');
    const QString qpfx(quote_depth, '>');
    const QString qsep = quote_depth > 0 ? " " : "";
    for (const QJsonValue &item : val.toArray()) {
        if (item.isString()) {
            result << pad + qpfx + qsep + item.toString();
            continue;
        }
        QJsonObject obj = item.toObject();
        const QString sec = obj["section"].toString();
        if (sec == "QUOTE") {
            result << flattenLines(obj["lines"], indent, quote_depth + 1);
        } else if (sec == "CODE_BLOCK") {
            const QString subpad((indent + 1) * 2, ' ');
            result << subpad + "```";
            for (const QJsonValue &l : obj["lines"].toArray())
                result << subpad + l.toString();
            result << subpad + "```";
        } else {
            result << flattenLines(obj["lines"], indent + 1, quote_depth);
        }
    }
    return result;
}

// ── test_Section1 ─────────────────────────────────────────────────────────────

void test_Section1::testToHtml_data() {
    QTest::addColumn<QStringList>("lines");
    QTest::addColumn<QString>("expected");
    loadTestData(data_path, "to html");
}

void test_Section1::testToHtml() {
    QFETCH(QStringList, lines);
    QFETCH(QString, expected);
    QCOMPARE(renderLines(lines), expected);
}

void test_Section1::loadTestData(const QString &path, const QString &item) {
    static const QDir cases_dir(
        QDir(QString::fromUtf8(RESOURCE_PATH)).filePath("cases"));
    QFile file(cases_dir.filePath(path));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open the file: %s", qUtf8Printable(path));
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    for (const QJsonValue &val : doc.object()[item].toArray()) {
        QJsonObject obj = val.toObject();
        QStringList raw;
        for (const QJsonValue &l : obj["lines"].toArray()) raw << l.toString();
        QTest::newRow(obj["name"].toString().toUtf8().constData())
            << toMarkdownLines(raw, type) << obj["expected"].toString();
    }
}

// ── test_Section2 ─────────────────────────────────────────────────────────────

void test_Section2::testToHtml_data() {
    QTest::addColumn<QStringList>("lines");
    QTest::addColumn<QString>("expected");
    loadTestData(data_path, "to html");
}

void test_Section2::testToHtml() {
    QFETCH(QStringList, lines);
    QFETCH(QString, expected);
    QCOMPARE(renderLines(lines), expected);
}

void test_Section2::loadTestData(const QString &path, const QString &item) {
    static const QDir cases_dir(
        QDir(QString::fromUtf8(RESOURCE_PATH)).filePath("cases"));
    QFile file(cases_dir.filePath(path));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open the file: %s", qUtf8Printable(path));
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    for (const QJsonValue &val : doc.object()[item].toArray()) {
        QJsonObject obj = val.toObject();
        QTest::newRow(obj["name"].toString().toUtf8().constData())
            << flattenLines(obj["lines"]) << obj["expected"].toString();
    }
}
