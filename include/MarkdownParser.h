#pragma once

#include "export.h"

#include <qstring.h>

class MARKDOWNPARSER_API MarkdownParser {
public:
	static const QString default_style_sheet;
	MarkdownParser();

	void setStyleSheet(const QString& style_sheet);
	QString toHtml(const QString& md);

private:
	QString style_sheet;

	void compressStyleSheet();
	QString before(const QString& title);
	QString after();
};