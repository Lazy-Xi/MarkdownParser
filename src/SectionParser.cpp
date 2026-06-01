#include "SectionParser.h"

#include "InlineParser.h"
#include "SectionFactory.h"
#include "SectionType.h"
#include "node/BlockNodes.h"
#include "node/InlineNodes.h"
#include "utils/RegexUtils.h"
#include "utils/TypeUtils.h"

#include <qmap.h>
#include <qregularexpression.h>
#include <qset.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtypes.h>

static void appendInlines(Node &parent, const QString &raw) {
    for (auto &n : InlineParser(raw).parse())
        parent.appendChild(std::move(n));
}

static QStringList parseHtmlBlockLines(QStringListIterator &it) {
    QStringList lines;
    while (it.hasNext()) {
        const QString &line = it.peekNext();
        if (line.trimmed().isEmpty()) break;
        SectionType t = SectionFactory::getLineType(line);
        if (!lines.isEmpty() && t != SectionType::HTML && t != SectionType::TEXT)
            break;
        lines.append(it.next());
    }
    return lines;
}

static QStringList parseTableLines(QStringListIterator &it) {
    QStringList lines;
    while (it.hasNext()) {
        if (!TypeUtils::isTable(it.peekNext())) break;
        lines.append(it.next());
    }
    return lines;
}

static QStringList tableCells(const QString &line) {
    QRegularExpression re(R"(\|(?= *\S) *(.*?)(?= *(?<!\\)\|| *$))");
    QRegularExpressionMatchIterator mi = re.globalMatch(line);
    QStringList cells;
    while (mi.hasNext()) cells.append(mi.next().captured(1));
    return cells;
}

static std::unique_ptr<Node> buildTableNodeFromLines(const QStringList &lines) {
    if (lines.size() < 2) return std::make_unique<TableNode>();

    QStringList headers = tableCells(lines[0]);
    const QStringList seps = tableCells(lines[1]);
    QList<Align> aligns;
    for (const auto &s : seps) {
        bool l = s.startsWith(":"), r = s.endsWith(":");
        aligns.append((l && r) ? Align::CENTER : r ? Align::RIGHT : Align::LEFT);
    }
    qsizetype cols = qMax(headers.size(), aligns.size());
    while (headers.size() < cols) headers.append("");
    while (aligns.size() < cols) aligns.append(Align::LEFT);

    auto table = std::make_unique<TableNode>();
    auto head_row = std::make_unique<TableRowNode>(true);
    for (qsizetype i = 0; i < cols; ++i) {
        auto cell = std::make_unique<TableCellNode>(aligns[i], true);
        appendInlines(*cell, headers[i]);
        head_row->appendChild(std::move(cell));
    }
    table->appendChild(std::move(head_row));

    for (qsizetype r = 2; r < lines.size(); ++r) {
        QStringList cells = tableCells(lines[r]);
        auto row = std::make_unique<TableRowNode>(false);
        for (qsizetype i = 0; i < qMin(cols, cells.size()); ++i) {
            auto cell = std::make_unique<TableCellNode>(aligns[i], false);
            appendInlines(*cell, cells[i]);
            row->appendChild(std::move(cell));
        }
        table->appendChild(std::move(row));
    }
    return table;
}

// ── public API ────────────────────────────────────────────────────────────────

std::unique_ptr<Node> SectionParser::nextNode() {
    while (it.hasNext() && it.peekNext().isEmpty()) it.next();
    if (!it.hasNext()) return nullptr;

    const QString &line = it.peekNext();
    if (line == "<!--TOC-->") {
        // Skip the marker and any hand-maintained TOC body up to the closing
        // marker; emit a TocNode placeholder for the renderer to fill.
        it.next();
        while (it.hasNext() && it.peekNext() != "<!--/TOC-->") it.next();
        if (it.hasNext()) it.next(); // consume <!--/TOC-->
        return std::make_unique<TocNode>();
    }
    if (line == "<!--/TOC-->") {
        it.next();
        return nextNode();
    }

    switch (SectionFactory::getLineType(line)) {
    case SectionType::CODE_BLOCK:          return buildCodeBlockNode();
    case SectionType::INDENTED_CODE_BLOCK: return buildIndentedCodeBlockNode();
    case SectionType::HORIZONTAL_RULE:     return buildThematicBreakNode();
    case SectionType::HTML:                return buildHtmlBlockNode();
    case SectionType::TITLE:               return buildHeadingNode();
    case SectionType::TEXT:                return buildParagraphNode();
    case SectionType::TABLE:               return tryBuildTableNode();
    case SectionType::ORDERED_LIST:        return buildListNode(SectionType::ORDERED_LIST);
    case SectionType::UNORDERED_LIST:      return buildListNode(SectionType::UNORDERED_LIST);
    case SectionType::QUOTE:               return buildQuoteNode();
    default:                               return buildParagraphNode();
    }
}

// ── block builders ────────────────────────────────────────────────────────────

std::unique_ptr<Node> SectionParser::buildCodeBlockNode() {
    QStringList lines;
    it.next(); // consume opening ```
    while (it.hasNext()) {
        const QString &l = it.next();
        if (TypeUtils::isCodeBlock(l)) break;
        lines.append(l);
    }
    return std::make_unique<CodeBlockNode>(std::move(lines));
}

std::unique_ptr<Node> SectionParser::buildIndentedCodeBlockNode() {
    QStringList lines;
    lines.append(it.next().mid(4));
    bool last_empty = false;
    while (it.hasNext()) {
        if (last_empty) { lines.append(""); last_empty = false; }
        const QString &l = it.peekNext();
        SectionType t = SectionFactory::getLineType(l);
        if (t == SectionType::EMPTY) { it.next(); last_empty = true; }
        else if (t != SectionType::INDENTED_CODE_BLOCK) break;
        else { lines.append(l.mid(4)); it.next(); }
    }
    if (!lines.isEmpty() && lines.last().isEmpty()) lines.pop_back();
    return std::make_unique<CodeBlockNode>(std::move(lines));
}

std::unique_ptr<Node> SectionParser::buildHtmlBlockNode() {
    return std::make_unique<HtmlBlockNode>(parseHtmlBlockLines(it));
}

std::unique_ptr<Node> SectionParser::buildThematicBreakNode() {
    it.next();
    return std::make_unique<ThematicBreakNode>();
}

std::unique_ptr<Node> SectionParser::buildHeadingNode() {
    const QString raw = it.next();
    QRegularExpressionMatch m = RegexUtils::title().match(raw);
    int level = m.captured(1).length();
    QString content = m.captured(2);
    auto node = std::make_unique<HeadingNode>(level, content);
    appendInlines(*node, content);
    return node;
}

std::unique_ptr<Node> SectionParser::buildParagraphNode() {
    QString content;
    while (it.hasNext()) {
        if (SectionFactory::getLineType(it.peekNext()) != SectionType::TEXT) break;
        const QString &l = it.next();
        content += l;
        if (!l.endsWith("\n")) content += " ";
    }
    auto node = std::make_unique<ParagraphNode>();
    appendInlines(*node, content.trimmed());
    return node;
}

std::unique_ptr<Node> SectionParser::tryBuildTableNode() {
    QStringList lines;
    bool sep_break = false;
    lines.append(it.next());
    while (it.hasNext()) {
        const QString &l = it.peekNext();
        SectionType t = SectionFactory::getLineType(l);
        if (t != SectionType::TABLE) break;
        if (TypeUtils::isTableHeaderSeparator(l)) { sep_break = true; break; }
        lines.append(l); it.next();
    }
    if (lines.size() != 1 || !sep_break) {
        auto node = std::make_unique<ParagraphNode>();
        for (const auto &l : lines) appendInlines(*node, l.trimmed());
        return node;
    }
    it.previous();
    return buildTableNodeFromLines(parseTableLines(it));
}

std::unique_ptr<Node> SectionParser::buildListNode(SectionType type) {
    static const QMap<SectionType, SectionType> other_type {
        {SectionType::ORDERED_LIST, SectionType::UNORDERED_LIST},
        {SectionType::UNORDERED_LIST, SectionType::ORDERED_LIST}};
    static const auto lineType = [](const QString &l) {
        return SectionFactory::getLineType(l.trimmed());
    };
    static const auto indentOf = [](const QString &l) -> qsizetype {
        qsizetype n = 0;
        for (QChar c : l) { if (c == ' ') ++n; else break; }
        return n >> 1;
    };

    const QRegularExpression &re = (type == SectionType::ORDERED_LIST)
        ? RegexUtils::orderedList(true) : RegexUtils::unorderedList(true);

    auto list = std::make_unique<ListNode>(type == SectionType::ORDERED_LIST);
    std::unique_ptr<ListItemNode> cur;
    qsizetype base = -1;

    while (it.hasNext()) {
        const QString &line = it.peekNext();
        SectionType lt = lineType(line);
        qsizetype ind = indentOf(line);
        bool allowed = lt != SectionType::TITLE && lt != SectionType::EMPTY;

        if (base == -1 && lt == type) base = ind;
        if (!allowed || ind < base || (ind == base && lt == other_type[type])) break;

        if (ind > base) {
            QStringList sub;
            sub.append(it.next().mid(ind << 1));
            while (it.hasNext()) {
                SectionType st = lineType(it.peekNext());
                if (st == SectionType::TITLE || st == SectionType::EMPTY ||
                    indentOf(it.peekNext()) < ind) break;
                sub.append(it.next().mid(ind << 1));
            }
            QStringListIterator si(sub);
            SectionParser sp(si);
            if (!cur) cur = std::make_unique<ListItemNode>();
            for (auto n = sp.nextNode(); n; n = sp.nextNode())
                cur->appendChild(std::move(n));
        } else if (lt == type) {
            if (cur) list->appendChild(std::move(cur));
            cur = std::make_unique<ListItemNode>();
            QString content(it.next());
            while (it.hasNext()) {
                const QString &nl = it.peekNext().trimmed();
                if (lineType(nl) != SectionType::TEXT ||
                    indentOf(it.peekNext()) != ind) break;
                content += " " + nl;
                it.next();
            }
            appendInlines(*cur, re.match(content).captured(2));
        } else if (lt == SectionType::TABLE) {
            if (!cur) cur = std::make_unique<ListItemNode>();
            cur->appendChild(tryBuildTableNode());
        } else {
            QStringList sub;
            sub.append(it.next().trimmed());
            while (it.hasNext()) {
                SectionType st = lineType(it.peekNext().trimmed());
                if (st == SectionType::EMPTY) { it.next(); continue; }
                if (st != SectionType::TEXT || indentOf(it.peekNext()) < ind) break;
                sub.append(it.next().trimmed());
            }
            QStringListIterator si(sub);
            SectionParser sp(si);
            if (!cur) cur = std::make_unique<ListItemNode>();
            for (auto n = sp.nextNode(); n; n = sp.nextNode())
                cur->appendChild(std::move(n));
        }
    }
    if (cur) list->appendChild(std::move(cur));
    return list;
}

std::unique_ptr<Node> SectionParser::buildQuoteNode() {
    auto quote = std::make_unique<BlockQuoteNode>();
    static const QSet<SectionType> stop {SectionType::EMPTY, SectionType::TITLE};
    static const auto lineType = [](const QString &l) {
        return SectionFactory::getLineType(l.trimmed());
    };
    QStringList sub;
    QRegularExpression re(RegexUtils::quote());
    while (it.hasNext()) {
        SectionType lt = lineType(it.peekNext());
        if (stop.contains(lt)) break;
        if (lt == SectionType::QUOTE)
            sub.append(re.match(it.next()).captured(1));
        else break;
    }
    QStringListIterator si(sub);
    SectionParser sp(si);
    for (auto n = sp.nextNode(); n; n = sp.nextNode())
        quote->appendChild(std::move(n));
    return quote;
}
