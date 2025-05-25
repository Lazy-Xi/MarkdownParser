#include "MarkdownParser.h"

#include "config.h"

#include <qcontainerfwd.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qiodevice.h>
#include <qlogging.h>
#include <qstring.h>
#include <qstringconverter.h>
#include <qtextstream.h>

#include <exception>
#include <stdexcept>

void translate(const QString& input_path, const QString& output_path);

QString readFile(const QString& path, const char* encoding);
void writeFile(const QString& path, const QString& content);

int main() {
    const QDir resource_dir(QString::fromUtf8(RESOURCE_PATH));
    const QDir input_dir(resource_dir.filePath("tests/samples"));
    const QDir output_dir(resource_dir.filePath("tests_output"));
    output_dir.mkpath(".");

    QStringList filters;
    filters << "*.md";
    QFileInfoList md_files = input_dir.entryInfoList(filters, QDir::Files);

    if (QFileInfo readme(resource_dir.filePath("README.md")); readme.exists()) {
        md_files.append(readme);
    }

    for (const QFileInfo& file_info : md_files) {
        const QString input_path = file_info.absoluteFilePath();
        const QString output_path = output_dir.absolutePath() + "/" + file_info.completeBaseName() + ".html";
        translate(input_path, output_path);
    }

    return 0;
}

void translate(const QString& input_path, const QString& output_path) {
    static MarkdownParser parser;

    try {
        const QString markdown = readFile(input_path, "utf-8");
        writeFile(output_path, parser.toHtml(markdown));
    }
    catch (const std::exception& e) {
        qWarning().noquote() << "Error processing" << input_path << ":" << e.what();
    }
}

QString readFile(const QString& path, const char* encoding) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error(QString("Cannot open file: %1").arg(path).toStdString());
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    QStringDecoder decoder(encoding);
    if (!decoder.isValid()) {
        throw std::runtime_error("Unsupported encoding");
    }

    return decoder.decode(content.toUtf8());
}

void writeFile(const QString& path, const QString& content) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error(QString("Cannot open file: %1").arg(path).toStdString());
    }

    QTextStream out(&file);
    out << content;
    file.close();
}