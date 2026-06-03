#pragma once

#include "SectionType.h"
#include "export.h"
#include "node/Node.h"

#include <QString>
#include <QStringList>

#include <memory>

class MARKDOWNPARSER_API SectionParser {
public:
    SectionParser() = delete;
    SectionParser(QStringListIterator &it) : it(it) {}
    SectionParser(const SectionParser &) = delete;
    SectionParser(SectionParser &&) noexcept = delete;
    SectionParser &operator=(const SectionParser &) = delete;
    SectionParser &operator=(SectionParser &&) noexcept = delete;

    std::unique_ptr<Node> nextNode();

private:
    QStringListIterator &it;

    std::unique_ptr<Node> buildCodeBlockNode();
    std::unique_ptr<Node> buildIndentedCodeBlockNode();
    std::unique_ptr<Node> buildHtmlBlockNode();
    std::unique_ptr<Node> buildThematicBreakNode();
    std::unique_ptr<Node> buildHeadingNode();
    std::unique_ptr<Node> buildParagraphNode();
    std::unique_ptr<Node> tryBuildTableNode();
    std::unique_ptr<Node> buildListNode(SectionType type);
    std::unique_ptr<Node> buildQuoteNode();
};
