#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtPrintSupport>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QDomDocument>

class Highlighter : public QSyntaxHighlighter {

    Q_OBJECT

    public:
        Highlighter(QTextDocument *parent = 0, const QString &language = "C++");
    protected:
        void highlightBlock(const QString &text) override;
    private:
        struct HighlightingRule {
            QRegularExpression pattern;
            QTextCharFormat format;
        };
        QVector<HighlightingRule> highlightingRules;
        QRegularExpression commentStartExpression;
        QRegularExpression commentEndExpression;
        QTextCharFormat multiLineCommentFormat;
};


#endif // SYNTAXHIGHLIGHTER_H

