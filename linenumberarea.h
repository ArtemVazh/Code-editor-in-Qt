#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QtWidgets>
#include "codeeditor.h"

class LineNumberArea : public QWidget
{
private:
    CodeEditor *codeEditor;
public:
    LineNumberArea (CodeEditor *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) {
        codeEditor->lineNumberAreaPaintEvent(event);
    }
};

#endif // LINENUMBERAREA_H
