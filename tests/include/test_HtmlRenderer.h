#pragma once

#include <QObject>

// Stage 0: validates HtmlRenderer against hand-built AST trees,
// independent of the parser. Confirms each visit() emits the same
// HTML the old section classes produced for the equivalent structure.
class test_HtmlRenderer : public QObject {
    Q_OBJECT

private slots:
    void testHeading();
    void testParagraphWithInlines();
    void testCodeBlockEscapes();
    void testList();
    void testNestedQuote();
    void testTable();
    void testLinkAndImage();
    void testHeadingSlugPlainText();
    void testToc();
};
