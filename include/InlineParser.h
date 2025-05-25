#pragma once

#include "export.h"
#include "SectionType.h"

#include <qchar.h>
#include <qmap.h>
#include <qstack.h>
#include <qstring.h>
#include <qtypes.h>

#include <functional>

class MARKDOWNPARSER_API InlineParser {
public:
    InlineParser() = delete;
    InlineParser(const QString& line) : line(line) {}
    InlineParser(const InlineParser&) = delete;
    InlineParser(InlineParser&&) noexcept = delete;
    InlineParser& operator=(const InlineParser&) = delete;
    InlineParser& operator=(InlineParser&&) noexcept = delete;

    QString toHtml();

private:
    struct StateFrame {
        SectionType type = SectionType::NORMAL;
        QString* target = nullptr;
    };

    QString line;
    QString html = "";
    QString buffer = "";
    QString temp_text = "";
    QString temp_url = "";

    StateFrame state;
    QStack<StateFrame> state_stack;

    void pushState(SectionType new_state, QString* target);
    void popState();

    void handleBold(qsizetype& i, const QChar& ch);
    void handleBoldItalic(qsizetype& i, const QChar& ch);
    void handleChar(qsizetype& i, const QChar& ch);
    void handleHighlight(qsizetype& i, const QChar& ch);
    void handleImageAlt(qsizetype& i, const QChar& ch);
    void handleImageUrl(qsizetype& i, const QChar& ch);
    void handleInlineCode(qsizetype& i, const QChar& ch);
    void handleItalic(qsizetype& i, const QChar& ch);
    void handleLinkText(qsizetype& i, const QChar& ch);
    void handleLinkUrl(qsizetype& i, const QChar& ch);
    void handleNormal(qsizetype& i, const QChar& ch);
    void handleStrikethrough(qsizetype& i, const QChar& ch);

    static const QMap<SectionType, QString> open_tags;
    static const QMap<SectionType, QString> close_tags;
    static const QMap<SectionType, std::function<void(InlineParser*, qsizetype&, const QChar&)>> state_handlers;
};