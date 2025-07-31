#include "SectionType.h"

#include <qmap.h>
#include <qstring.h>

#include <stdexcept>

const QMap<QString, SectionType> reflect_map = {
	{ "HORIZONTAL_RULE", SectionType::HORIZONTAL_RULE },
	{ "TITLE", SectionType::TITLE },

	{ "CODE_BLOCK", SectionType::CODE_BLOCK },
	{ "HTML", SectionType::HTML },
	{ "ORDERED_LIST", SectionType::ORDERED_LIST },
	{ "QUOTE", SectionType::QUOTE },
	{ "TABLE", SectionType::TABLE },
	{ "TEXT", SectionType::TEXT },
	{ "UNORDERED_LIST", SectionType::UNORDERED_LIST },
};

SectionType sectionTypeReflect(const QString& type) {
	if (reflect_map.contains(type)) {
		return reflect_map.value(type);
	}
	throw std::runtime_error(QString("SectionType %1 cannot be reflected")
		.arg(type).toUtf8());
}