#include "include/mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QHBoxLayout>
#include <QImageReader>
#include <QMessageBox>
#include <QDir>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QStandardPaths>
#include <QImageWriter>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent):
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  first_dialog_ = false;

  // Left image
  image_label_left_ = new QLabel;
  image_label_left_->setBackgroundRole(QPalette::Dark);
  image_label_left_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  image_label_left_->setScaledContents(true);

  scroll_area_left_ = new QScrollArea;
  scroll_area_left_->setBackgroundRole(QPalette::Dark);
  scroll_area_left_->setWidget(image_label_left_);
  scroll_area_left_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

  // Right image
  image_label_right_ = new QLabel;
  image_label_right_->setBackgroundRole(QPalette::Dark);
  image_label_right_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  image_label_right_->setScaledContents(true);

  scroll_area_right_ = new QScrollArea;
  scroll_area_right_->setBackgroundRole(QPalette::Dark);
  scroll_area_right_->setWidget(image_label_right_);
  scroll_area_right_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

  // Show images side by side
  QWidget *central_widget = new QWidget;
  QHBoxLayout *layout = new QHBoxLayout(central_widget);
  layout->addWidget(scroll_area_left_);
  layout->addWidget(scroll_area_right_);

  setCentralWidget(central_widget);

  createActions();

  resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::createActions()
{
  QMenu *file_menu = menuBar()->addMenu(tr("&File"));

  QAction *open_action = file_menu->addAction(tr("&Open..."), this, &MainWindow::open);
  open_action->setShortcut(QKeySequence::Open);

  save_as_action_ = file_menu->addAction(tr("&Save As..."), this, &MainWindow::saveAs);
  save_as_action_->setShortcut(QKeySequence::Save);
  save_as_action_->setEnabled(false);

  file_menu->addSeparator();

  QAction *exit_action = file_menu->addAction(tr("E&xit"), this, &QWidget::close);
  exit_action->setShortcut(tr("Ctrl+Q"));

  QMenu *view_menu = menuBar()->addMenu(tr("&View"));

  fit_to_window_action_ = view_menu->addAction(tr("&Fit to Window"), this, &MainWindow::fitToWindow);
  fit_to_window_action_->setEnabled(false);
  fit_to_window_action_->setCheckable(true);

  QMenu *help_menu = menuBar()->addMenu(tr("&Help"));

  help_menu->addAction(tr("&About"), this, &MainWindow::about);
  help_menu->addAction(tr("About &Qt"), &QApplication::aboutQt);
}

void MainWindow::updateActions()
{
    save_as_action_->setEnabled(!image_.isNull());
}

void MainWindow::initializeImageFileDialog(QFileDialog& dialog, QFileDialog::AcceptMode accept_mode)
{
    if (first_dialog_) {
      first_dialog_ = false;
      const QStringList pictures_locations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
      dialog.setDirectory(pictures_locations.isEmpty() ? QDir::currentPath() : pictures_locations.last());
    }

    QStringList mime_type_filters;
    const QByteArrayList supported_mime_types = accept_mode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mime_type_name, supported_mime_types)
        mime_type_filters.append(mime_type_name);
    mime_type_filters.sort();
    dialog.setMimeTypeFilters(mime_type_filters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (accept_mode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

void MainWindow::open()
{
  QFileDialog dialog(this, tr("Open File"));
  initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

  // TODO(jfguimaraes) Review this loop
  while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first()));
}

bool MainWindow::loadFile(const QString& file_name)
{
  QImageReader reader(file_name);
  reader.setAutoTransform(true);
  const QImage new_image = reader.read();

  if (new_image.isNull()) {
    QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                             tr("Cannot load %1: %2")
                             .arg(QDir::toNativeSeparators(file_name), reader.errorString()));
    return false;
  }

  setImage(new_image);
  setWindowFilePath(file_name);

  const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
      .arg(QDir::toNativeSeparators(file_name)).arg(image_.width()).arg(image_.height()).arg(image_.depth());
  statusBar()->showMessage(message);
  return true;
}

void MainWindow::setImage(const QImage& new_image)
{
    image_ = new_image;
    image_label_left_->setPixmap(QPixmap::fromImage(image_));

    fit_to_window_action_->setEnabled(true);
    updateActions();

    if (!fit_to_window_action_->isChecked())
      image_label_left_->adjustSize();
}

void MainWindow::saveAs()
{
  QFileDialog dialog(this, tr("Save File As"));
  initializeImageFileDialog(dialog, QFileDialog::AcceptSave);

  // TODO(jfguimaraes) Review this loop
  while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first()));
}

bool MainWindow::saveFile(const QString& file_name)
{
    QImageWriter writer(file_name);

    if (!writer.write(image_)) {
      QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                               tr("Cannot write %1: %2")
                               .arg(QDir::toNativeSeparators(file_name)), writer.errorString());
      return false;
    }

    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(file_name));
    statusBar()->showMessage(message);
    return true;
}

void MainWindow::fitToWindow()
{
    bool fit_to_window = fit_to_window_action_->isChecked();
    scroll_area_left_->setWidgetResizable(fit_to_window);
    //scroll_area_right_->setWidgetResizable(fit_to_window);

    if (!fit_to_window) {
      image_label_left_->adjustSize();
      //image_label_right_->adjustSize();
    }
}

void MainWindow::about()
{
  QMessageBox::about(this, tr("About PhotoChopp"),
                     tr("<p><b>PhotoChopp</b> is a class project for the image processing fundamentals class"
                        "on UFRGS 2018/2. This instance was developed by Jéferson Ferreira Guimarães.</p>"));
}
