#pragma once

#include "export.h"
#include "section/AbstractSection.h"

#include <qlist.h>
#include <qstring.h>

#include <utility>

class MARKDOWNPARSER_API SectionList : public QList<AbstractSection*> {
public:
	SectionList() = delete;
	SectionList(const QString& markdown) : markdown(markdown) {}
	SectionList(const SectionList& other) : markdown(other.markdown) {}
	SectionList(SectionList&& other) noexcept : markdown(std::move(other.markdown)) {}
	SectionList& operator=(const SectionList&);
	SectionList& operator=(SectionList&&) noexcept;

	QString toHtml();

private:
	QString markdown;

	QString before();
	QString after();
};