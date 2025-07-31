#include "test_Section.h"

#include "config.h"
#include "section/AbstractSection.h"
#include "SectionFactory.h"
#include "SectionType.h"

#include <qdir.h>
#include <qfile.h>
#include <qiodevice.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qset.h>
#include <qstring.h>
#include <qtestcase.h>
#include <qtestdata.h>

static AbstractSection2::LinesType parseItem(const QJsonValue& val);

void test_Section1::testToHtml_data() {
	QTest::addColumn<AbstractSection1::LinesType>("lines");
	QTest::addColumn<QString>("expected");

	loadTestData(data_path, "to html");
}

void test_Section1::testToHtml() {
	QFETCH(AbstractSection1::LinesType, lines);
	QFETCH(QString, expected);

	auto section(SectionFactory::createSection({ lines, type }));

	QCOMPARE(section->toHtml(), expected);
}

void test_Section1::loadTestData(const QString& path, const QString& item) {
    static const QDir cases_dir(QDir(QString::fromUtf8(RESOURCE_PATH)).filePath("cases"));

    QFile file(cases_dir.filePath(path));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open the file: %s", qUtf8Printable(path));
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray array = doc.object()[item].toArray();

    for (const QJsonValue& val : array) {
        QJsonObject obj = val.toObject();
        QString name = obj["name"].toString();
        AbstractSection1::LinesType lines;
        for (const QJsonValue& line : obj["lines"].toArray()) {
            lines << line.toString();
        }
        QString expected = obj["expected"].toString();
        QTest::newRow(name.toUtf8().constData()) << lines << expected;
    }
}

void test_Section2::testToHtml_data() {
	QTest::addColumn<AbstractSection2::LinesType>("lines");
	QTest::addColumn<QString>("expected");
	loadTestData(data_path, "to html");
}

void test_Section2::testToHtml() {
	QFETCH(AbstractSection2::LinesType, lines);
	QFETCH(QString, expected);

	auto section(SectionFactory::createSection({ lines, type }));

	QCOMPARE(section->toHtml(), expected);
}

void test_Section2::loadTestData(const QString& path, const QString& item) {
    static const QDir cases_dir(QDir(QString::fromUtf8(RESOURCE_PATH)).filePath("cases"));

    QFile file(cases_dir.filePath(path));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open the file: %s", qUtf8Printable(path));
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray array = doc.object()[item].toArray();

    for (const QJsonValue& val : array) {
        QJsonObject obj = val.toObject();
        QString name = obj["name"].toString();
        AbstractSection2::LinesType lines = parseItem(obj["lines"]);
        QString expected = obj["expected"].toString();
        QTest::newRow(name.toUtf8().constData()) << lines << expected;
    }
}

static AbstractSection2::LinesType parseItem(const QJsonValue& val) {
    AbstractSection2::LinesType lines = {};
    for (const QJsonValue& item : val.toArray()) {
        if (item.isString()) {
            lines << item.toString();
            continue;
        }

        QJsonObject obj = item.toObject();
        SectionType type = sectionTypeReflect(obj["section"].toString());
        if (QSet({ SectionType::ORDERED_LIST, SectionType::QUOTE, 
            SectionType::UNORDERED_LIST }).contains(type)) {
            lines << SectionFactory::createSection({ parseItem(obj["lines"]), type });
        }
        else {
            AbstractSection1::LinesType _lines = {};
            for (const QJsonValue& line : obj["lines"].toArray()) {
                _lines << line.toString();
            }
            lines << SectionFactory::createSection({ _lines, type });
        }
    }
    return lines;
};