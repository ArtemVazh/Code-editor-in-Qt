#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtPrintSupport>
#include <QUndoGroup>
#include "highlighter.h"
#include "linenumberarea.h"
#include "codeeditor.h"

namespace Ui {
class TextEditor;
}

class TextEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit TextEditor(QWidget *parent = 0);
    ~TextEditor();
    void closeEvent(QCloseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    QLabel *X;
    QLabel *Y;
private:
    QAction *font;
    QAction *undo;
    QAction *redo;
    QList<Highlighter*> m_highlighter;
    QDockWidget *view;
    bool isActive = false;
    QListView *ListView;
    QList<QPair<QString, CodeEditor*>> tabs;
    QList<bool> tabsSaveType;
    QList<bool> isChanged;
    QTabWidget *mainWidget = new QTabWidget;
    void createMenu();
    Ui::TextEditor *ui;
    int countOfNewFiles = 0;

 private slots:
    void position();
    void closeDock(bool);
    void changeUR(int index);
    void changeFont();
    void redoClicked();
    void undoClicked();
    void viewClicked(QModelIndex);
    void createView();
    void createExplorer();
    void copyText();
    void pasteText();
    void cutText();
    void selectAllText();
    void deleteText();
    void closeProgramm();
    void saveAll();
    void textIsChanged();
    void closeAll();
    void newTab();
    void closeTab(int index);
    void closeTab();
    void saveFileAs();
    void openFile();
    void saveFile(const QString &fileName = QString(), int index = -1);
};

#endif // TEXTEDITOR_H
