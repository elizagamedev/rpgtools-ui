#include "mainwindow.h"

#include <QDir>
#include <QStringList>
#include <QProcess>

#include <QPushButton>
#include <QVBoxLayout>

#include <QFileDialog>
#include <QInputDialog>
#include <QProgressDialog>
#include <QMessageBox>

#ifdef Q_OS_WIN32
#define EXE ".exe"
#else
#define EXE ""
#endif

MainWindow::MainWindow(QDir &tooldir) :
    QWidget(0)
{
    //Get tools
    QStringList files = tooldir.entryList(QDir::Files);
    foreach (const QString &file, files) {
        if (QString::compare(file, "mapdump" EXE, Qt::CaseInsensitive) == 0) {
            mapdumpPath = tooldir.absoluteFilePath(file);
        } else if (QString::compare(file, "rpgconv" EXE, Qt::CaseInsensitive) == 0) {
            rpgconvPath = tooldir.absoluteFilePath(file);
        } else if (QString::compare(file, "xyz" EXE, Qt::CaseInsensitive) == 0) {
            xyzPath = tooldir.absoluteFilePath(file);
        } else if (QString::compare(file, "2k2xp" EXE, Qt::CaseInsensitive) == 0) {
            _2k2xpPath = tooldir.absoluteFilePath(file);
        }
    }

    //Construct buttons
    mapdumpBtn = new QPushButton(tr("Map Dump (RPG Maker 2000/2003)"));
    rpgconvBtn = new QPushButton(tr("RPG Project/Release Converter"));
    xyzBtn = new QPushButton(tr("XYZ Image Converter"));
    _2k2xpBtn = new QPushButton(tr("RPG Maker 2000/2003 to XP Resource Converter"));
    mapdumpBtn->setEnabled(!mapdumpPath.isEmpty());
    rpgconvBtn->setEnabled(!rpgconvPath.isEmpty());
    xyzBtn->setEnabled(!xyzPath.isEmpty());
    _2k2xpBtn->setEnabled(!_2k2xpPath.isEmpty());
    connect(mapdumpBtn, SIGNAL(clicked()), this, SLOT(mapdumpClicked()));
    connect(rpgconvBtn, SIGNAL(clicked()), this, SLOT(rpgconvClicked()));
    connect(xyzBtn, SIGNAL(clicked()), this, SLOT(xyzClicked()));
    connect(_2k2xpBtn, SIGNAL(clicked()), this, SLOT(_2k2xpClicked()));

    //Create layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(mapdumpBtn);
    layout->addWidget(rpgconvBtn);
    layout->addWidget(xyzBtn);
    layout->addWidget(_2k2xpBtn);
    setLayout(layout);

    //Adjust window
    adjustSize();
    setFixedSize(size());
    setWindowTitle("rpgtools");
}

void MainWindow::startProcess(const QString &name, const QStringList &args)
{
    //Create progress dialog
    progress = new QProgressDialog(tr("Performing operation..."), tr("Cancel"), 0, 0, this);
    connect(progress, SIGNAL(canceled()), this, SLOT(processCanceled()));
    progress->setWindowModality(Qt::WindowModal);
    progress->show();

    //Create process
    processWasCanceled = false;
    process = new QProcess(this);
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
    process->start(name, args);
}

void MainWindow::processCanceled()
{
    processWasCanceled = true;
    process->terminate();
}

void MainWindow::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    progress->hide();
    progress->deleteLater();
    QString msg = QString::fromUtf8(process->readAllStandardError());
    if (exitCode == 0 || exitStatus != QProcess::NormalExit) {
        if (msg.isEmpty()) {
            if (processWasCanceled)
                msg = tr("The process was canceled.");
            else
                msg = tr("The operation completed successfully.");
        }
        QMessageBox(QMessageBox::Information, tr("rpgtools"), msg, QMessageBox::Ok).exec();
    } else {
        if (msg.isEmpty())
            msg = tr("The operation failed.");
        QMessageBox(QMessageBox::Critical, tr("rpgtools"), msg, QMessageBox::Ok).exec();
    }
    process->deleteLater();
}

void MainWindow::mapdumpClicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Select Game Folder"));
    if (path.isEmpty())
        return;
    bool ok;
    QString encoding = QInputDialog::getText(this, tr("rpgtools"), tr("What is the game encoding? (Leave blank to auto-detect)"), QLineEdit::Normal, QString(), &ok);
    if (!ok)
        return;

    //Start process
    QStringList args;
    args << path;
    if (!encoding.isEmpty())
        args << encoding;
    startProcess(mapdumpPath, args);
}

void MainWindow::rpgconvClicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Select Game Folder"));
    if (path.isEmpty())
        return;

    //Start process
    QStringList args;
    args << path;
    startProcess(rpgconvPath, args);
}

void MainWindow::xyzClicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Image Files"), QString(), tr("Images (*.xyz *.png *.bmp)"));
    if (files.isEmpty())
        return;
    startProcess(xyzPath, files);
}

void MainWindow::_2k2xpClicked()
{
    QString srcPath = QFileDialog::getExistingDirectory(this, tr("Select 2000/2003 Game Folder"));
    if (srcPath.isEmpty())
        return;
    QString dstPath = QFileDialog::getExistingDirectory(this, tr("Select XP Game Folder"));
    if (dstPath.isEmpty())
        return;
    QStringList args;
    args << srcPath << dstPath;
    startProcess(_2k2xpPath, args);
}
