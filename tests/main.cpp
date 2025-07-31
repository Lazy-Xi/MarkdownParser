#include "test_Section.h"

#include "SectionType.h"

#include <qcontainerfwd.h>
#include <qset.h>
#include <qstring.h>
#include <qtestcase.h>

int main(int argc, char* argv[]) {
	int status = 0;

	const QSet<const QPair<const QString, const SectionType>> tests1 = {
		{ "section/test_CodeBlockSection.json", SectionType::CODE_BLOCK },
		{ "section/test_HorizontalRuleSection.json", SectionType::HORIZONTAL_RULE },
		{ "section/test_HtmlSection.json", SectionType::HTML },
		{ "section/test_TableSection.json", SectionType::TABLE },
		{ "section/test_TextSection.json", SectionType::TEXT },
		{ "section/test_TitleSection.json", SectionType::TITLE },
	};

	const QSet<const QPair<const QString, const SectionType>> tests2 = {
		{ "section/test_OrderedListSection.json", SectionType::ORDERED_LIST },
		{ "section/test_QuoteSection.json", SectionType::QUOTE },
		{ "section/test_UnorderedListSection.json", SectionType::UNORDERED_LIST },
	};

	for (auto& t : tests1)
	{
		test_Section1 test(t.first, t.second);
		status |= QTest::qExec(&test, argc, argv);
	}

	for (auto& t : tests2) {
		test_Section2 test(t.first, t.second);
		status |= QTest::qExec(&test, argc, argv);
	}

	return status;
}