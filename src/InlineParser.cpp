#include "InlineParser.h"

#include "node/BlockNodes.h"
#include "node/InlineNodes.h"
#include "render/HtmlRenderer.h"

#include <qchar.h>
#include <qstring.h>
#include <qtypes.h>

#include <utility>

// ── helpers ──────────────────────────────────────────────────────────────────

void InlineParser::flushText() {
    if (!pending_text.isEmpty()) {
        if (raw_text)
            state.target->push_back(std::make_unique<RawTextNode>(pending_text));
        else
            state.target->push_back(std::make_unique<TextNode>(pending_text));
        pending_text.clear();
    }
}

// Append a span node to the current target, push the current frame, and make
// the new node's child list the active target.
void InlineParser::pushSpan(SectionType type, std::unique_ptr<Node> node) {
    flushText();
    Node *raw = node.get();
    state.target->push_back(std::move(node));
    state_stack.push(state);
    state = {type, &raw->children()};
}

void InlineParser::popState() {
    flushText();
    if (!state_stack.isEmpty()) {
        state = state_stack.pop();
    } else {
        state = {SectionType::NORMAL, &root};
    }
}

// ── public API ───────────────────────────────────────────────────────────────

InlineParser::NodeList InlineParser::parse() {
    root.clear();
    captured.clear();
    pending_text.clear();
    buffer.clear();
    temp_text.clear();
    temp_url.clear();
    state = {SectionType::NORMAL, &root};
    state_stack.clear();

    for (qsizetype i = 0; i < line.length(); ++i) {
        handleChar(i, line[i]);
    }
    // Auto-close any still-open spans, innermost first. Because each span node
    // was appended to its parent at open time, unclosed spans already sit in
    // the tree with their accumulated children — just flush pending text.
    while (true) {
        flushText();
        if (state_stack.isEmpty()) break;
        state = state_stack.pop();
    }
    return std::move(root);
}

QString InlineParser::toHtml() {
    return HtmlRenderer().renderInlines(parse());
}

// ── character dispatch ────────────────────────────────────────────────────────

void InlineParser::handleChar(qsizetype &i, const QChar &ch) {
    if (state.type == SectionType::ESCAPE) {
        // Escaped char is a literal — emit as RawText (no double-escape).
        state.target->push_back(std::make_unique<RawTextNode>(QString(ch)));
        popState();
    } else if (state.type == SectionType::NORMAL)      { handleNormal(i, ch);
    } else if (state.type == SectionType::INLINE_CODE) { handleInlineCode(i, ch);
    } else if (state.type == SectionType::BOLD)        { handleBold(i, ch);
    } else if (state.type == SectionType::ITALIC)      { handleItalic(i, ch);
    } else if (state.type == SectionType::BOLD_ITALIC) { handleBoldItalic(i, ch);
    } else if (state.type == SectionType::STRIKETHROUGH){ handleStrikethrough(i, ch);
    } else if (state.type == SectionType::HIGHLIGHT)   { handleHighlight(i, ch);
    } else if (state.type == SectionType::LINK_TEXT)   { handleLinkText(i, ch);
    } else if (state.type == SectionType::LINK_URL)    { handleLinkUrl(i, ch);
    } else if (state.type == SectionType::IMAGE_ALT)   { handleImageAlt(i, ch);
    } else if (state.type == SectionType::IMAGE_URL)   { handleImageUrl(i, ch);
    } else {
        pending_text += ch;
    }
}

// ── NORMAL ────────────────────────────────────────────────────────────────────

void InlineParser::handleNormal(qsizetype &i, const QChar &ch) {
    if (ch == '\\') {
        flushText();
        state_stack.push(state);
        state = {SectionType::ESCAPE, state.target};
        return;
    }

    struct Pattern { QString marker; SectionType type; SpanKind kind; };
    static const Pattern span_patterns[] = {
        {"***", SectionType::BOLD_ITALIC,   SpanKind::StrongEmphasis},
        {"___", SectionType::BOLD_ITALIC,   SpanKind::StrongEmphasis},
        {"**",  SectionType::BOLD,          SpanKind::Strong},
        {"__",  SectionType::BOLD,          SpanKind::Strong},
        {"*",   SectionType::ITALIC,        SpanKind::Emphasis},
        {"_",   SectionType::ITALIC,        SpanKind::Emphasis},
        {"~~",  SectionType::STRIKETHROUGH, SpanKind::Strikethrough},
        {"==",  SectionType::HIGHLIGHT,     SpanKind::Highlight},
    };

    for (const auto &p : span_patterns) {
        if (line.mid(i, p.marker.length()) != p.marker) continue;
        if (p.marker[0] == '_') {
            const bool left  = i > 0 && line[i - 1].isLetterOrNumber();
            const qsizetype after = i + p.marker.length();
            const bool right = after < line.length() && line[after].isLetterOrNumber();
            if (left && right) { pending_text += ch; return; }
        }
        if (!state_stack.isEmpty() && state.type == p.type) {
            popState(); // close
        } else {
            pushSpan(p.type, std::make_unique<EmphasisNode>(p.kind));
        }
        i += p.marker.length() - 1;
        return;
    }

    if (ch == '`') {
        flushText();
        buffer.clear();
        state_stack.push(state);
        state = {SectionType::INLINE_CODE, state.target};
        return;
    }

    if (line.mid(i, 2) == "![") {
        flushText();
        buffer.clear();
        state_stack.push(state);
        state = {SectionType::IMAGE_ALT, state.target};
        ++i;
        return;
    }
    if (ch == '[') {
        flushText();
        // Parse link text into `captured` (inline-parsed).
        captured.clear();
        state_stack.push(state);
        state = {SectionType::LINK_TEXT, &captured};
        return;
    }

    pending_text += ch;
}

// ── span close handlers ───────────────────────────────────────────────────────

void InlineParser::handleBold(qsizetype &i, const QChar &ch) {
    if (line.mid(i, 2) == "**" || line.mid(i, 2) == "__") { popState(); ++i; }
    else { handleNormal(i, ch); }
}

void InlineParser::handleItalic(qsizetype &i, const QChar &ch) {
    if (ch == '*' || ch == '_') { popState(); }
    else { handleNormal(i, ch); }
}

void InlineParser::handleBoldItalic(qsizetype &i, const QChar &ch) {
    if (line.mid(i, 3) == "***" || line.mid(i, 3) == "___") { popState(); i += 2; }
    else { handleNormal(i, ch); }
}

void InlineParser::handleStrikethrough(qsizetype &i, const QChar &ch) {
    if (line.mid(i, 2) == "~~") { popState(); ++i; }
    else { handleNormal(i, ch); }
}

void InlineParser::handleHighlight(qsizetype &i, const QChar &ch) {
    if (line.mid(i, 2) == "==") { popState(); ++i; }
    else { handleNormal(i, ch); }
}

// ── inline code ───────────────────────────────────────────────────────────────

void InlineParser::handleInlineCode(qsizetype &, const QChar &ch) {
    if (ch == '`') {
        // Emit InlineCodeNode into the parent frame's target.
        state_stack.top().target->push_back(
            std::make_unique<InlineCodeNode>(buffer));
        buffer.clear();
        popState();
    } else {
        buffer += ch;
    }
}

// ── link ──────────────────────────────────────────────────────────────────────
// Strategy: link text is inline-parsed into `captured`. On ']':
//   - if followed by '(' → switch to URL capture (LINK_URL), keep captured.
//   - else → fallback: emit "[" + captured children as text + "]".
// On ')': build LinkNode(href, children=captured), append to parent target.

void InlineParser::handleLinkText(qsizetype &i, const QChar &ch) {
    if (ch == ']') {
        flushText(); // flush into captured
        if (i + 1 < line.length() && line[i + 1] == '(') {
            ++i;
            buffer.clear();
            state.type = SectionType::LINK_URL;
            // state.target stays &captured (unused in URL mode)
        } else {
            // Fallback: reconstruct "[text]" as a plain TextNode.
            // Collect plain text from captured (best-effort).
            QString flat = "[";
            for (const auto &c : captured) {
                if (c->type() == NodeType::Text)
                    flat += static_cast<const TextNode &>(*c).text();
                else if (c->type() == NodeType::RawText)
                    flat += static_cast<const RawTextNode &>(*c).text();
                else if (c->type() == NodeType::InlineCode)
                    flat += static_cast<const InlineCodeNode &>(*c).code();
            }
            flat += "]";
            captured.clear();
            // Restore parent frame and emit fallback text.
            state = state_stack.pop();
            state.target->push_back(std::make_unique<TextNode>(flat));
        }
    } else {
        handleNormal(i, ch);
    }
}

void InlineParser::handleLinkUrl(qsizetype &i, const QChar &ch) {
    auto commit = [&](const QString &href) {
        // Build LinkNode; move captured children into it.
        auto link = std::make_unique<LinkNode>(href);
        for (auto &c : captured) link->appendChild(std::move(c));
        captured.clear();
        // Restore parent frame and append the link.
        state = state_stack.pop();
        state.target->push_back(std::move(link));
    };
    if (ch == ')') {
        commit(buffer);
        buffer.clear();
    } else if (ch == ' ' &&
               (line.mid(i + 1, 6) == "&quot;" || line.mid(i + 1, 1) == "\"")) {
        // Optional title starts here (raw '"' or pre-escaped '&quot;').
        QString href = buffer;
        buffer.clear();
        while (i < line.length() && line[i] != ')') ++i;
        commit(href);
    } else {
        buffer += ch;
    }
}

// ── image ─────────────────────────────────────────────────────────────────────

void InlineParser::handleImageAlt(qsizetype &i, const QChar &ch) {
    if (ch == ']') {
        temp_text = buffer;
        buffer.clear();
        if (i + 1 < line.length() && line[i + 1] == '(') {
            ++i;
            state.type = SectionType::IMAGE_URL;
        } else {
            NodeList *parent = state_stack.top().target;
            parent->push_back(
                std::make_unique<TextNode>("![" + temp_text + "]"));
            popState();
        }
    } else {
        buffer += ch;
    }
}

void InlineParser::handleImageUrl(qsizetype &i, const QChar &ch) {
    auto commit = [&](const QString &src) {
        NodeList *parent = state_stack.top().target;
        parent->push_back(std::make_unique<ImageNode>(src, temp_text));
        buffer.clear();
        popState();
    };
    if (ch == ')') {
        commit(buffer);
    } else if (ch == ' ' &&
               (line.mid(i + 1, 6) == "&quot;" || line.mid(i + 1, 1) == "\"")) {
        QString src = buffer;
        buffer.clear();
        while (i < line.length() && line[i] != ')') ++i;
        commit(src);
    } else {
        buffer += ch;
    }
}
