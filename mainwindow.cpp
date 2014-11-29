#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSplitter>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QSettings>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    readSettings();
    setupMainSplitter();
    loadNoteDirectoryList();

    ui->centralWidget->layout()->addWidget( this->mainSplitter );
}

MainWindow::~MainWindow()
{
    delete ui;
}


/*!
 * Methods
 */

void MainWindow::setupMainSplitter()
{
    this->mainSplitter = new QSplitter;
    connect( this->mainSplitter, SIGNAL( splitterMoved( int, int ) ), this, SLOT( on_mainSplitter_splitterMoved( int, int ) ) );

    this->mainSplitter->addWidget(ui->notesListWidget);
    this->mainSplitter->addWidget(ui->noteTextEdit);

    // restore splitter sizes
    QSettings settings("PBE", "QNotes");
    QByteArray state = settings.value( "mainSplitterSizes" ).toByteArray();
    this->mainSplitter->restoreState( state );
}

void MainWindow::loadNoteDirectoryList()
{
    // TODO: make notes path selectable
    this->notesPath = QDir::homePath() + QDir::separator() + "ownCloud" + QDir::separator() + "Notes";
    QDir notesDir( this->notesPath );

    // only show text files
    QStringList filters;
    filters << "*.txt";

    // show newest entry first
    QStringList files = notesDir.entryList( filters, QDir::Files, QDir::Time );

    this->ui->notesListWidget->addItems( files );
}

void MainWindow::loadNote( QString &fileName )
{
    QFile file( fileName );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in( &file );

    // qDebug() << file.size() << in.readAll();
    QString noteText = in.readAll();
    file.close();

    // we don't want a "textChange()" fired up
    this->ui->noteTextEdit->blockSignals( true );
    this->ui->noteTextEdit->setText( noteText );
    this->ui->noteTextEdit->blockSignals( false );
}

void MainWindow::storeNote( QString &fileName, QString &text )
{
    QFile file( fileName );
    file.open( QIODevice::WriteOnly | QIODevice::Text );
    QTextStream out( &file );
    out << text;
    file.flush();
    file.close();
}

void MainWindow::readSettings()
{
    QSettings settings("PBE", "QNotes");
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/windowState").toByteArray());
}


/*!
 * Internal events
 */

void MainWindow::closeEvent(QCloseEvent *event)
 {
    QSettings settings("PBE", "QNotes");
    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/windowState", saveState());
    QMainWindow::closeEvent(event);
 }



 /*!
  * Slots implementation
  */

void MainWindow::on_notesListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    qDebug() << "currentItemChanged " << current->text();
    QString fileName = this->notesPath + QDir::separator() + current->text();
    loadNote( fileName );
}

void MainWindow::on_noteTextEdit_textChanged()
{
    // TODO: don't save after every text change, use a delay of one second to save
    qDebug() << "textChanged";
    QString fileName = this->notesPath + QDir::separator() + this->ui->notesListWidget->currentItem()->text();
    QString text = this->ui->noteTextEdit->toPlainText();
    storeNote( fileName, text );
}

void MainWindow::on_mainSplitter_splitterMoved ( int pos, int index )
{
    qDebug() << "on_mainSplitter_splitterMoved";

    QSettings settings("PBE", "QNotes");
    settings.setValue( "mainSplitterSizes", this->mainSplitter->saveState() );
}
