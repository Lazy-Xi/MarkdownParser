#pragma once

#include "SectionType.h"
#include "export.h"
#include "node/Node.h"

#include <QStack>
#include <QString>

#include <memory>
#include <vector>

// Span-level parser. Pushdown automaton (a current frame + a stack
// for nesting) that builds an inline AST: handlers construct nodes
// instead of appending HTML tag strings. The open/close tag literals
// now live in HtmlRenderer.
//
// parse() returns the inline node list. toHtml() is a thin
// convenience that renders that list (kept so existing block sections
// keep compiling unchanged).
class MARKDOWNPARSER_API InlineParser {
public:
    using NodeList = std::vector<std::unique_ptr<Node>>;

    InlineParser() = delete;
    // raw_text=true: plain-text runs are emitted as RawTextNode (no
    // re-escape). Use this when the input has already been
    // HTML-escaped (legacy Section path).
    InlineParser(const QString &line, bool raw_text = false)
        : line(line), raw_text(raw_text) {}
    InlineParser(const InlineParser &) = delete;
    InlineParser(InlineParser &&) noexcept = delete;
    InlineParser &operator=(const InlineParser &) = delete;
    InlineParser &operator=(InlineParser &&) noexcept = delete;

    NodeList parse();
    QString toHtml();

private:
    // A frame names the currently-open span and where its children
    // go. The span node itself was already appended to its parent at
    // open time, so an unclosed span still renders both tags
    // (matching the old auto-close).
    struct StateFrame {
        SectionType type = SectionType::NORMAL;
        NodeList *target = nullptr;
    };

    QString line;
    bool raw_text; // if true, plain-text runs flush as RawTextNode
    NodeList root; // result list (children of the NORMAL frame)
    NodeList captured; // link-text children, committed when the link
                       // closes
    QString pending_text;   // literal-run accumulator, flushed to a
                            // TextNode
    QString buffer;         // inline-code / image-alt / url capture
    QString temp_text = ""; // captured image alt
    QString temp_url = "";

    StateFrame state;
    QStack<StateFrame> state_stack;

    void flushText(); // pending_text -> TextNode into *state.target
    void pushSpan(SectionType type, std::unique_ptr<Node> node);
    void popState();

    void handleBold(qsizetype &i, const QChar &ch);
    void handleBoldItalic(qsizetype &i, const QChar &ch);
    void handleChar(qsizetype &i, const QChar &ch);
    void handleHighlight(qsizetype &i, const QChar &ch);
    void handleImageAlt(qsizetype &i, const QChar &ch);
    void handleImageUrl(qsizetype &i, const QChar &ch);
    void handleInlineCode(qsizetype &i, const QChar &ch);
    void handleItalic(qsizetype &i, const QChar &ch);
    void handleLinkText(qsizetype &i, const QChar &ch);
    void handleLinkUrl(qsizetype &i, const QChar &ch);
    void handleNormal(qsizetype &i, const QChar &ch);
    void handleStrikethrough(qsizetype &i, const QChar &ch);
};
