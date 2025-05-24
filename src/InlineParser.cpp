#include "InlineParser.h"

#include "SectionType.h"

#include <qchar.h>
#include <qmap.h>
#include <qstring.h>
#include <qtypes.h>

#include <functional>

const QMap<SectionType, QString> InlineParser::open_tags = {
    { SectionType::BOLD, "<strong>" },
    { SectionType::ITALIC, "<em>" },
    { SectionType::BOLD_ITALIC, "<strong><em>" },
    { SectionType::STRIKETHROUGH, "<del>" },
    { SectionType::HIGHLIGHT, "<mark>" },
    { SectionType::INLINE_CODE, "<code>" }
};

const QMap<SectionType, QString> InlineParser::close_tags = {
    { SectionType::BOLD, "</strong>" },
    { SectionType::ITALIC, "</em>" },
    { SectionType::BOLD_ITALIC, "</em></strong>" },
    { SectionType::STRIKETHROUGH, "</del>" },
    { SectionType::HIGHLIGHT, "</mark>" },
    { SectionType::INLINE_CODE, "</code>" }
};

const QMap<SectionType, std::function<void(InlineParser*, qsizetype&, const QChar&)>> InlineParser::state_handlers = {
    { SectionType::NORMAL, &InlineParser::handleNormal },
    { SectionType::INLINE_CODE, &InlineParser::handleInlineCode },
    { SectionType::BOLD, &InlineParser::handleBold },
    { SectionType::ITALIC, &InlineParser::handleItalic },
    { SectionType::BOLD_ITALIC, &InlineParser::handleBoldItalic },
    { SectionType::STRIKETHROUGH, &InlineParser::handleStrikethrough },
    { SectionType::HIGHLIGHT, &InlineParser::handleHighlight },
    { SectionType::LINK_TEXT, &InlineParser::handleLinkText },
    { SectionType::LINK_URL, &InlineParser::handleLinkUrl },
    { SectionType::IMAGE_ALT, &InlineParser::handleImageAlt },
    { SectionType::IMAGE_URL, &InlineParser::handleImageUrl }
};

void InlineParser::pushState(SectionType new_state, QString* target) {
    state_stack.push(state);
    state = { new_state, target };
}

void InlineParser::popState() {
    if (!state_stack.isEmpty()) { state = state_stack.pop(); }
    else { state = { SectionType::NORMAL, &html }; }
}

QString InlineParser::toHtml() {
    html.clear();
    buffer.clear();
    temp_text.clear();
    temp_url.clear();
    state = { SectionType::NORMAL, &html };
    state_stack.clear();

    for (qsizetype i = 0; i < line.length(); ++i) {
        handleChar(i, line[i]);
    }
    while (!state_stack.isEmpty()) {
        state = state_stack.pop();
        if (close_tags.contains(state.type)) { 
            html.append(close_tags[state.type]); 
        }
    }
    return html;
}

void InlineParser::handleChar(qsizetype& i, const QChar& ch) {
    if (state.type == SectionType::ESCAPE) {
        state.target->append(ch);
        popState();
    }
    else if (state_handlers.contains(state.type)) { state_handlers[state.type](this, i, ch); }
    else { state.target->append(ch); }
}

void InlineParser::handleNormal(qsizetype& i, const QChar& ch) {
    struct Pattern {
        QString marker;
        SectionType type;
    };
    static const Pattern append_patterns[] = {
        { "***", SectionType::BOLD_ITALIC },
        { "~~", SectionType::STRIKETHROUGH },
        { "==", SectionType::HIGHLIGHT },
        { "**", SectionType::BOLD },
        { "__", SectionType::BOLD },
        { "*", SectionType::ITALIC },
        { "_", SectionType::ITALIC },
        { "`", SectionType::INLINE_CODE },
    };
    static const Pattern buffer_patterns[] = {
        { "![", SectionType::IMAGE_ALT },
        { "[", SectionType::LINK_TEXT }
    };

    for (const auto& p : append_patterns) {
        if (line.mid(i, p.marker.length()) == p.marker) {
            if (!state_stack.isEmpty() && state.type == p.type) {
                state.target->append(close_tags[p.type]);
                popState();
            }
            else {
                state.target->append(open_tags[p.type]);
                pushState(p.type, state.target);
            }
            i += p.marker.length() - 1;
            return;
        }
    }
    for (const auto& p : buffer_patterns) {
        if (line.mid(i, p.marker.length()) == p.marker) {
            buffer.clear();
            pushState(p.type, &buffer);
            i += p.marker.length() - 1;
            return;
        }
    }
    state.target->append(ch);
}

void InlineParser::handleInlineCode(qsizetype& i, const QChar& ch) {
    if (ch == '`') {
        state.target->append(close_tags[SectionType::INLINE_CODE]);
        popState();
    }
    else { state.target->append(ch); }
}

void InlineParser::handleBold(qsizetype& i, const QChar& ch) {
    if (line.mid(i, 2) == "**" || line.mid(i, 2) == "__") {
        state.target->append(close_tags[SectionType::BOLD]);
        popState();
        ++i;
    }
    else { handleNormal(i, ch); }
}

void InlineParser::handleItalic(qsizetype& i, const QChar& ch) {
    if (line.mid(i, 1) == "*" || line.mid(i, 1) == "_") {
        state.target->append(close_tags[SectionType::ITALIC]);
        popState();
        ++i;
    }
    else { handleNormal(i, ch); }
}

void InlineParser::handleBoldItalic(qsizetype& i, const QChar& ch) {
    if (line.mid(i, 3) == "***") {
        state.target->append(close_tags[SectionType::BOLD_ITALIC]);
        popState();
        i += 2;
    }
    else { handleNormal(i, ch); }
}

void InlineParser::handleStrikethrough(qsizetype& i, const QChar& ch) {
    if (line.mid(i, 2) == "~~") {
        state.target->append(close_tags[SectionType::STRIKETHROUGH]);
        popState();
        ++i;
    }
    else { handleNormal(i, ch); }
}

void InlineParser::handleHighlight(qsizetype& i, const QChar& ch) {
    if (line.mid(i, 2) == "==") {
        state.target->append(close_tags[SectionType::HIGHLIGHT]);
        popState();
        ++i;
    }
    else { handleNormal(i, ch); }
}

void InlineParser::handleLinkText(qsizetype& i, const QChar& ch) {
    if (ch == ']') {
        temp_text = buffer;
        buffer.clear();
        if (i + 1 < line.length() && line[i + 1] == '(') {
            ++i;
            state.type = SectionType::LINK_URL;
            state.target = &buffer;
        }
        else {
            html.append(QString("[%1]").arg(temp_text));
            popState();
        }
    }
    else { handleNormal(i, ch); }
}

void InlineParser::handleLinkUrl(qsizetype& i, const QChar& ch) {
    if (ch == ')') {
        temp_url = buffer;
        html.append(QString(R"(<a href="%1">%2</a>)").arg(temp_url).arg(temp_text));
        popState();
    }
    else { state.target->append(ch); }
}

void InlineParser::handleImageAlt(qsizetype& i, const QChar& ch) {
    if (ch == ']') {
        temp_text = buffer;
        buffer.clear();
        if (i + 1 < line.length() && line[i + 1] == '(') {
            ++i;
            state.type = SectionType::IMAGE_URL;
        }
        else {
            html.append(QString("![%1]").arg(temp_text));
            popState();
        }
    }
    else { state.target->append(ch); }
}

void InlineParser::handleImageUrl(qsizetype& i, const QChar& ch) {
    if (ch == ')') {
        temp_url = buffer;
        html.append(QString(R"(<img src="%1" alt="%2">)").arg(temp_url).arg(temp_text));
        popState();
    }
    else { state.target->append(ch); }
}
