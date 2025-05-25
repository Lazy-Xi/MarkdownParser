#pragma once

#include "export.h"
#include "section/AbstractSection.h"
#include "SectionType.h"

#include <qmap.h>
#include <qregularexpression.h>
#include <qstring.h>

#include <functional>
#include <variant>

struct MARKDOWNPARSER_API RawSection {
    std::variant<AbstractSection1::LinesType, AbstractSection2::LinesType> lines = {};
    SectionType type = SectionType::UNKNOWN;
};

class MARKDOWNPARSER_API SectionFactory {
public:
    static AbstractSection* createSection(const RawSection& section);
    static SectionType getLineType(const QString& line);

    /**
    * @brief 创建用于匹配 Markdown 缩进代码块的正则表达式
    *
    * 捕获组说明：
    *  - capture 1: 代码内容
    **/
    static QRegularExpression createIndentedCodeBlockRegex();

    /**
    * @brief 创建用于匹配 Markdown 分割线的正则表达式
    *
    * 捕获组说明：
    *  - capture 1: 分割线标记
    **/
    static QRegularExpression createHorizontalRuleRegex();

    /**
    * @brief 创建用于匹配 Markdown 标题的正则表达式
    *
    * 捕获组说明：
    *  - capture 1: `#`序列
    *  - capture 2: 标题内容
    **/
    static QRegularExpression createTitleRegex();

    /**
    * @brief 创建用于匹配 Markdown 代码块标记的正则表达式
    *
    * 捕获组说明：
    *  - capture 1: 语言
    **/
    static QRegularExpression createCodeBlockRegex();

    /**
    * @brief 创建用于匹配 Markdown 有序列表的正则表达式
    *
    * 捕获组说明：
    *  - capture 1: 序号
    *  - capture 2: 文本
    **/
    static QRegularExpression createOrderedListRegex(bool ignore_start_space = false);

    /**
    * @brief 创建用于匹配 Markdown 引用的正则表达式
    *
    * 捕获组说明：
    *  - capture 1: 引用内容
    **/
    static QRegularExpression createQuoteRegex();

    static QRegularExpression createTableRegex();
    static QRegularExpression createTableHeaderSeparatorRegex();

    /**
    * @brief 创建用于匹配 Markdown 有序列表的正则表达式
    *
    * 捕获组说明：
    *  - capture 1: 列表标记
    *  - capture 2: 文本
    **/
    static QRegularExpression createUnorderedListRegex(bool ignore_start_space = false);

    static bool isHorizontalRule(const QString& line);
    static bool isTitle(const QString& line);

    static bool isCodeBlock(const QString& line);
    static bool isIndentedCodeBlock(const QString& line);
    static bool isOrderedList(const QString& line);
    static bool isQuote(const QString& line);
    static bool isTable(const QString& line);
    static bool isTableHeaderSeparator(const QString& line);
    static bool isUnorderedList(const QString& line);

private:
    static const QMap<SectionType, std::function<bool(const QString&)>> checks;
    static const QMap<SectionType, std::function<AbstractSection* (const RawSection&)>> creators;
};