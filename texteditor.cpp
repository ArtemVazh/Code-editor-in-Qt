#include "texteditor.h"
#include "ui_texteditor.h"

TextEditor::TextEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TextEditor)
{
    ui->setupUi(this);
    this->setWindowTitle("Notepad#");
    this->setWindowIcon(QIcon("resourses/icon.png"));
    this->resize(QSize(600, 600));
    createMenu();
    mainWidget->setTabsClosable(true);
    mainWidget->setMovable(false);
    QVBoxLayout *MainLayout = new QVBoxLayout;
    MainLayout->addWidget(mainWidget);
    MainLayout->setContentsMargins(0, 0, 0, 0);
    ui->centralWidget->setLayout(MainLayout);
    connect(mainWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(mainWidget, SIGNAL(tabBarClicked(int)), this, SLOT(changeUR(int)));
    undo->setEnabled(false);
    redo->setEnabled(false);
    font->setEnabled(false);
    setMouseTracking(true);
    X = new QLabel(this);
    Y = new QLabel(this);
    X->setText("x = 0");
    Y->setText("y = 0");
    statusBar()->addWidget(X);
    statusBar()->addWidget(Y);
}

void TextEditor::mouseMoveEvent(QMouseEvent *event) {
    X->setText("x = " + QString().setNum(event->x()));
    Y->setText("y = " + QString().setNum(event->y()));
}

void TextEditor::closeDock(bool visibality) {
    isActive = visibality;
}

void TextEditor::changeUR(int index){
    if (tabs[index].second->document()->isRedoAvailable() == true)
        redo->setEnabled(true);
    else
        redo->setEnabled(false);
    if (tabs[index].second->document()->isUndoAvailable() == true)
        undo->setEnabled(true);
    else
        undo->setEnabled(false);
}

void TextEditor::undoClicked(){
    int index = mainWidget->currentIndex();
    tabs[index].second->undo();
}

void TextEditor::redoClicked(){
    int index = mainWidget->currentIndex();
    tabs[index].second->redo();
}

void TextEditor::changeFont(){
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("Times New Roman", 10), this, QString::fromUtf8("Выберите шрифт"));
    QTextCharFormat format;
    format.setFont(font);
    if (ok) {
        if (!tabs[mainWidget->currentIndex()].second->textCursor().selectedText().isEmpty())
            tabs[mainWidget->currentIndex()].second->textCursor().mergeCharFormat(format);
        else if (tabs[mainWidget->currentIndex()].second->textCursor().atBlockStart())
            tabs[mainWidget->currentIndex()].second->textCursor().mergeBlockCharFormat(format);
    }
}

void TextEditor::closeEvent(QCloseEvent *event){
    closeAll();
    event->accept();
}

void TextEditor::createMenu() {
    QMenu *file = new QMenu("Файл");
    QMenu *edit = new QMenu("Правка");
    QMenu *view = new QMenu("Вид");
    ui->menuBar->addMenu(file);
    ui->menuBar->addSeparator();
    ui->menuBar->addMenu(edit);
    ui->menuBar->addSeparator();
    ui->menuBar->addMenu(view);
    ui->menuBar->addSeparator();
    QAction *New = new QAction(QIcon("resourses/new.png"), QString("Новый файл"));
    New->setShortcut(QKeySequence::New);
    connect(New, SIGNAL(triggered()), this, SLOT(newTab()));
    file->addAction(New);
    New->setToolTip("Новый файл");
    ui->mainToolBar->addAction(New);
    ui->mainToolBar->addSeparator();

    QAction *open = new QAction(QIcon("resourses/open.png"), QString("Открыть"));
    open->setShortcut(QKeySequence::Open);
    file->addAction(open);
    open->setToolTip("Открыть");
    ui->mainToolBar->addAction(open);
    ui->mainToolBar->addSeparator();
    connect(open, SIGNAL(triggered()), this, SLOT(openFile()));

    QAction *save = new QAction(QIcon("resourses/save 1.png"), QString("Сохранить"));
    save->setToolTip("Сохранить");
    ui->mainToolBar->addAction(save);
    ui->mainToolBar->addSeparator();
    save->setShortcut(QKeySequence::Save);
    connect(save, SIGNAL(triggered()), this, SLOT(saveFile(QString, int)));
    file->addAction(save);

    QAction *SaveAs = new QAction("Сохранить как...", file);
    SaveAs->setShortcut(QKeySequence::SaveAs);
    connect(SaveAs, SIGNAL(triggered()), this, SLOT(saveFileAs()));
    file->addAction(SaveAs);

    QAction *saveall = new QAction(QIcon("resourses/saveall.png"), QString("Сохранить все"));
    saveall->setToolTip("Сохранить все");
    ui->mainToolBar->addAction(saveall);
    ui->mainToolBar->addSeparator();
    connect(saveall, SIGNAL(triggered()), this, SLOT(saveAll()));
    file->addAction(saveall);

    QAction *Close = new QAction("Закрыть", file);
    Close->setShortcut(QKeySequence::Close);
    connect(Close, SIGNAL(triggered()), this, SLOT(closeTab()));
    file->addAction(Close);

    QAction *CloseAll = new QAction("Закрыть все", file);
    connect(CloseAll, SIGNAL(triggered()), this, SLOT(closeAll()));
    file->addAction(CloseAll);

    QAction *Exit = new QAction("Выход", file);
    connect(Exit, SIGNAL(triggered()), this, SLOT(closeProgramm()));
    file->addAction(Exit);

    QAction *Cut = new QAction("Вырезать", edit);
    Cut->setShortcut(QKeySequence::Cut);
    connect(Cut, SIGNAL(triggered()), this, SLOT(cutText()));
    edit->addAction(Cut);

    QAction *Copy = new QAction("Копировать", edit);
    Copy->setShortcut(QKeySequence::Copy);
    connect(Copy, SIGNAL(triggered()), this, SLOT(copyText()));
    edit->addAction(Copy);

    QAction *Paste = new QAction("Вставить", edit);
    Paste->setShortcut(QKeySequence::Paste);
    connect(Paste, SIGNAL(triggered()), this, SLOT(pasteText()));
    edit->addAction(Paste);

    QAction *Delete = new QAction("Удалить", edit);
    Delete->setShortcut(QKeySequence::Delete);
    connect(Delete, SIGNAL(triggered()), this, SLOT(deleteText()));
    edit->addAction(Delete);

    QAction *SelectAll = new QAction("Выделить все", edit);
    SelectAll->setShortcut(QKeySequence::SelectAll);
    connect(SelectAll, SIGNAL(triggered()), this, SLOT(selectAllText()));
    edit->addAction(SelectAll);

    QAction *Reviewer = new QAction("Обозреватель", view);
    connect(Reviewer, SIGNAL(triggered()), this, SLOT(createView()));
    view->addAction(Reviewer);

    QAction *Explorer = new QAction("Проводник", view);
    connect(Explorer, SIGNAL(triggered()), this, SLOT(createExplorer()));
    view->addAction(Explorer);

    undo = new QAction(QIcon("resourses/Undo.png"), QString("Назад"));
    saveall->setToolTip("Назад");
    ui->mainToolBar->addAction(undo);
    ui->mainToolBar->addSeparator();
    connect(undo, SIGNAL(triggered()), this, SLOT(undoClicked()));

    redo = new QAction(QIcon("resourses/Redo.png"), QString("Вперёд"));
    saveall->setToolTip("Вперёд");
    ui->mainToolBar->addAction(redo);
    ui->mainToolBar->addSeparator();
    connect(redo, SIGNAL(triggered()), this, SLOT(redoClicked()));

    font = new QAction(QIcon("resourses/Font.png"), QString("Шрифт"));
    saveall->setToolTip("Изменить шрифт");
    ui->mainToolBar->addAction(font);
    ui->mainToolBar->addSeparator();
    connect(font, SIGNAL(triggered()), this, SLOT(changeFont()));

    ui->mainToolBar->setMovable(false);
}

void TextEditor::createView() {
    QStringListModel *model = new QStringListModel;
    QStringList fileNames;
    for (int i = 0; i < tabs.size(); ++i)
        fileNames.push_back(tabs[i].first);
    model->setStringList(fileNames);
    ListView = new QListView;
    ListView->setModel(model);
    QItemSelectionModel *selection = new QItemSelectionModel(model);
    ListView->setSelectionModel(selection);
    view = new QDockWidget("Проводник", this);
    view->setAllowedAreas(Qt::RightDockWidgetArea);
    view->setWidget(ListView);
    addDockWidget(Qt::RightDockWidgetArea, view);
    connect(ListView, SIGNAL(clicked(const QModelIndex)),this,SLOT(viewClicked(QModelIndex)));
    isActive = true;
    connect(view, SIGNAL(visibilityChanged(bool)), this, SLOT(closeDock(bool)));

}

void TextEditor::viewClicked(QModelIndex index){
    QString tabname = index.data().toString();
    int row;
    for (int i = 0; i < tabs.size(); ++i) {
        if (tabs[i].first == tabname) {
            row = i;
            break;
        }
    }
    changeUR(row);
    mainWidget->setCurrentIndex(row);
}

void TextEditor::copyText() {
    tabs[mainWidget->currentIndex()].second->copy();
}

void TextEditor::pasteText() {
    tabs[mainWidget->currentIndex()].second->paste();
}

void TextEditor::cutText(){
    tabs[mainWidget->currentIndex()].second->cut();
}

void TextEditor::selectAllText(){
    tabs[mainWidget->currentIndex()].second->selectAll();
}

void TextEditor::deleteText() {
    tabs[mainWidget->currentIndex()].second->textCursor().removeSelectedText();
}

void TextEditor::saveAll(){
    for (int i = 0; i < mainWidget->count(); ++i) {
       saveFile("", i);
    }
}

void TextEditor::closeProgramm(){
    for (int i = 0; i < mainWidget->count(); ++i) {
       closeTab();
    }
    this->close();
}

void TextEditor::openFile(){
    CodeEditor *edit = new CodeEditor;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Выберите файл"), "/Users/artem/Desktop/");
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    file.open(QIODevice::ReadWrite);
    edit->setPlainText(file.readAll());
    mainWidget->addTab(edit, QString::fromStdString(fileName.toStdString().substr(fileName.toStdString().find_last_of("/") + 1, fileName.size())));
    tabs.push_back(QPair<QString, CodeEditor*>(fileName, edit));
    tabsSaveType.push_back(true);
    isChanged.push_back(false);
    file.close();
    connect(edit, SIGNAL(textChanged()), this, SLOT(textIsChanged()));
    std::string fileType = fileName.toStdString().substr(fileName.toStdString().find_last_of("."), fileName.size());
    if (fileType != ".txt") {
        Highlighter *highlighter = new Highlighter(edit->document(), fileType.c_str());
        QFont font("Source Code Pro", 10);
        edit->setFont(font);
        edit->setStyleSheet("CodeEditor{background-color: #000000; color: rgb(190, 190, 190);}");
        m_highlighter.push_back(highlighter);
    }
    else {
        QFont font("Times New Roman", 10);
        edit->setFont(font);
    }
    if(isActive == true) {
        view->close();
        createView();
    }
    font->setEnabled(true);
    edit->setMouseTracking(true);
    connect(edit, SIGNAL(cursorPositionChanged()), this, SLOT(position()));
}

void TextEditor::position(){
    X->setText("x = " + QString().setNum(tabs[mainWidget->currentIndex()].second->cursor().pos().x()));
    Y->setText("y = " + QString().setNum(tabs[mainWidget->currentIndex()].second->cursor().pos().y()));
}

void TextEditor::saveFile(const QString &notCurrentfileName, int index) {
    int current;
    if (index == -1)
        current = mainWidget->currentIndex();
    else
        current = index;
    if(tabsSaveType[current] == false)
        saveFileAs();
    QString currentFileName = tabs[current].first;
    if (currentFileName.isEmpty()){
        QMessageBox::critical(this, "Ошибка", "Файл для сохранения не найден", QMessageBox::Ok);
        return;
    }
    if(!notCurrentfileName.isEmpty()) {
        currentFileName = notCurrentfileName;
    }
    isChanged[current] = false;
    QString fileType = QString::fromStdString(currentFileName.toStdString().substr(currentFileName.toStdString().find_last_of(".") + 1, currentFileName.size()));
    if(fileType == "txt") {
        QFile file(currentFileName);
        file.open(QIODevice::WriteOnly| QIODevice::Text);
        QTextStream out(&file);
        out << tabs[current].second->toPlainText();
    }
    if(fileType == "odf") {
        QTextDocumentWriter writer;
        writer.setFileName(currentFileName);
        writer.setFormat("odf");
        writer.write(tabs[current].second->document());
    }
    if(fileType == "pdf") {
        QPrinter printer(QPrinter::ScreenResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(currentFileName);
        tabs[current].second->document()->print(&printer);
    }
    if(fileType == "html") {
        QTextDocumentWriter writer;
        writer.setFileName(currentFileName);
        writer.setFormat("html");
        writer.write(tabs[current].second->document());
    }
}

void TextEditor::saveFileAs() {
    QString fileName = QFileDialog::getSaveFileName(this, QString::fromUtf8("Сохранить файл"), QDir::currentPath(), "*.txt;; *.odf;; *.pdf;; *.html");
    tabsSaveType[mainWidget->currentIndex()] = true;
    saveFile(fileName);
}

void TextEditor::closeTab(int index){
    if(isChanged[index] == true) {
        QString fileName = tabs[index].first;
        QMessageBox *save = new QMessageBox(QMessageBox::Information, "Notepad#" , "Вы хотите сохранить изменения в файле" + fileName + "?",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        int result = save->exec();
        if(result == QMessageBox::Yes)
        {
            saveFile();
        }
        if(result == QMessageBox::Cancel)
        {
            return;
        }
        delete save;
    }
    mainWidget->removeTab(index);
    tabs.removeAt(index);
    tabsSaveType.removeAt(index);
    if(isActive == true) {
        view->close();
        createView();
    }
    if(tabs.size() != 0)
        changeUR(mainWidget->currentIndex());
    isChanged.removeAt(index);
    if(tabs.size() == 0){
       undo->setEnabled(false);
       redo->setEnabled(false);
       font->setEnabled(false);
    }
}

void TextEditor::closeTab(){
    if(isChanged[mainWidget->currentIndex()] == true){
        QString fileName = tabs[mainWidget->currentIndex()].first;
        QMessageBox *save = new QMessageBox(QMessageBox::Information, "Notepad#" , "Вы хотите сохранить изменения в файле\n" + fileName + "?",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        int result = save->exec();
        if(result == QMessageBox::Yes)
        {
            saveFile();
        }
        if(result == QMessageBox::Cancel)
        {
            return;
        }
        delete save;
    }
    int index = mainWidget->currentIndex();
    mainWidget->removeTab(index);
    tabs.removeAt(index);
    if(tabs.size() != 0)
        changeUR(mainWidget->currentIndex());
    tabsSaveType.removeAt(index);
    if(isActive == true) {
        view->close();
        createView();
    }
    isChanged.removeAt(index);
    if(tabs.size() == 0){
       undo->setEnabled(false);
       redo->setEnabled(false);
       font->setEnabled(false);
    }
}

void TextEditor::newTab() {
    CodeEditor *edit = new CodeEditor;
    QString fileName = "Файл " + QString::number(countOfNewFiles++) + ".txt";
    QFont p_font("Times New Roman", 10);
    edit->setFont(p_font);
    mainWidget->addTab(edit, QString::fromStdString(fileName.toStdString().substr(fileName.toStdString().find_last_of("/") + 1, fileName.size())));
    tabs.push_back(QPair<QString, CodeEditor*>(fileName, edit));
    tabsSaveType.push_back(false);
    isChanged.push_back(false);
    connect(edit, SIGNAL(textChanged()), this, SLOT(textIsChanged()));
    if(isActive == true) {
        view->close();
        createView();
    }
    font->setEnabled(true);
    edit->setMouseTracking(true);
}

void TextEditor::closeAll(){
    int count = mainWidget->count();
    for (int i = 0; i < count; ++i) {
       closeTab();
    }
}

void TextEditor::textIsChanged() {
    isChanged[mainWidget->currentIndex()] = true;
    int index = mainWidget->currentIndex();
    if (tabs[index].second->document()->isRedoAvailable() == true)
        redo->setEnabled(true);
    else
        redo->setEnabled(false);
    if (tabs[index].second->document()->isUndoAvailable() == true)
        undo->setEnabled(true);
    else
        undo->setEnabled(false);
}

void TextEditor::createExplorer(){
    QDockWidget *dock = new QDockWidget("Проводник", this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea);
    QFileSystemModel *model = new QFileSystemModel;
    QTreeView *treeView = new QTreeView;
    model->setRootPath("C:/Users/artem/Desktop/");
    treeView->setModel(model);
    treeView->setRootIndex(model->index("C:/Users/artem/Desktop/"));
    dock->setWidget(treeView);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
}

TextEditor::~TextEditor()
{
    delete ui;
}
