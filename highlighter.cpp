#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent, const QString &language)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;
    QFile file("SyntaxHighlighterSettings.xml");
    QDomDocument doc;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    if (!doc.setContent(&file)){
        file.close();
        return;
    }
    file.close();
    QDomElement docElem = doc.documentElement();
    QDomNode node = docElem.firstChild();
    while (node.toElement().tagName() == "Language" && !node.toElement().attribute("fileType").contains(language))
        node = node.nextSibling();
    while (node.toElement().tagName() != "KeywordLists")
         node = node.firstChild();
    QDomNode nodeStyles = node.nextSibling();
    node = node.firstChild();
    nodeStyles = nodeStyles.firstChild();
    while ((node.toElement().tagName() == "Keywords") && (nodeStyles.toElement().tagName() == "Style")) {
        if (node.toElement().attribute("name") == "startComment"){
            node = node.firstChild();
            multiLineCommentFormat.setForeground(QColor(nodeStyles.toElement().attribute("fontColor")));
            if (nodeStyles.toElement().attribute("isBold") == "true")
                multiLineCommentFormat.setFontWeight(QFont::Bold);
            commentStartExpression = QRegularExpression(node.toText().data());
            node = node.parentNode();
            nodeStyles = nodeStyles.nextSibling();
            node = node.nextSibling();
            continue;
        }
        if (node.toElement().attribute("name") == "endComment"){
            node = node.firstChild();
            multiLineCommentFormat.setForeground(QColor(nodeStyles.toElement().attribute("fontColor")));
            if (nodeStyles.toElement().attribute("isBold") == "true")
                multiLineCommentFormat.setFontWeight(QFont::Bold);
            commentEndExpression = QRegularExpression(node.toText().data());
            node = node.parentNode();
            nodeStyles = nodeStyles.nextSibling();
            node = node.nextSibling();
            continue;
        }
        if (node.toElement().attribute("isRegExp") == "false"){
            node = node.firstChild();
            QTextCharFormat format;
            QStringList words;
            words = node.toText().data().split(QRegExp(" "),QString::SkipEmptyParts);
            format.setForeground(QColor(nodeStyles.toElement().attribute("fontColor")));
            if (nodeStyles.toElement().attribute("isBold") == "true")
                format.setFontWeight(QFont::Bold);
            foreach (const QString &pattern, words) {
                rule.pattern = QRegularExpression(pattern + "\\b");
                rule.format = format;
                highlightingRules.append(rule);
            }
            node = node.parentNode();
            nodeStyles = nodeStyles.nextSibling();
            node = node.nextSibling();
        }
        else {
            node = node.firstChild();
            QTextCharFormat format;
            QString regExp;
            format.setForeground(QColor(nodeStyles.toElement().attribute("fontColor")));
            if (nodeStyles.toElement().attribute("isBold") == "true")
                format.setFontWeight(QFont::Bold);
            regExp = node.toText().data();
            rule.pattern = QRegularExpression(regExp);
            rule.format = format;
            highlightingRules.append(rule);
            node = node.parentNode();
            nodeStyles = nodeStyles.nextSibling();
            node = node.nextSibling();
        }

    }
}

void Highlighter::highlightBlock(const QString &text)
{

    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
