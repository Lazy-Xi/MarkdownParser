#include "MarkdownParser.h"

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

QString readFile(const QString& path, const char* encoding);
void writeFile(const QString& path, const QString& content);

int main() {
    const QDir input_dir("samples");
    const QDir output_dir("output");
    output_dir.mkpath(".");

    QStringList filters;
    filters << "*.md";
    const QFileInfoList md_files = input_dir.entryInfoList(filters, QDir::Files);

    MarkdownParser parser;
    for (const QFileInfo& fileInfo : md_files) {
        const QString input_path = fileInfo.absoluteFilePath();
        const QString output_path = output_dir.absolutePath() + "/" + fileInfo.completeBaseName() + ".html";

        try {
            const QString markdown = readFile(input_path, "utf-8");
            writeFile(output_path, parser.toHtml(markdown));
        }
        catch (const std::exception& e) {
            qFatal() << "Error processing" << input_path << ":" << e.what();
        }
    }

    return 0;
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