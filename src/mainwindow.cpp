#include "include/mainwindow.hpp"
#include "ui_mainwindow.h"

#include <cmath>
#include <vector>

#include <QImageReader>
#include <QMessageBox>
#include <QDir>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QInputDialog>
#include <QStandardPaths>
#include <QImageWriter>
#include <QScreen>
#include <QPainter>

#include "include/image_operations.hpp"

MainWindow::MainWindow(QWidget *parent):
  QMainWindow(parent),
  is_first_dialog_(false),
  horizontal_layout_(&central_widget_)
{
  // Left image
  image_title_left_ = new QLabel;
  image_title_left_->setText("Original image");

  image_label_left_ = new QLabel;
  image_label_left_->setBackgroundRole(QPalette::Dark);
  image_label_left_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  image_label_left_->setScaledContents(true);
  image_label_left_->setMinimumSize(1, 1);

  scroll_area_left_ = new QScrollArea;
  scroll_area_left_->setBackgroundRole(QPalette::Dark);
  scroll_area_left_->setWidget(image_label_left_);
  scroll_area_left_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

  // Right image
  image_title_right_ = new QLabel;
  image_title_right_->setText("Modified image");

  image_label_right_ = new QLabel;
  image_label_right_->setBackgroundRole(QPalette::Dark);
  image_label_right_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  image_label_right_->setScaledContents(true);
  image_label_right_->setMinimumSize(1, 1);

  scroll_area_right_ = new QScrollArea;
  scroll_area_right_->setBackgroundRole(QPalette::Dark);
  scroll_area_right_->setWidget(image_label_right_);
  scroll_area_right_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

  // Show images side by side with titles on top
  vertical_layout_left_.addWidget(image_title_left_);
  vertical_layout_left_.addWidget(scroll_area_left_);

  vertical_layout_right_.addWidget(image_title_right_);
  vertical_layout_right_.addWidget(scroll_area_right_);

  horizontal_layout_.addLayout(&vertical_layout_left_);
  horizontal_layout_.addLayout(&vertical_layout_right_);

  setCentralWidget(&central_widget_);

  createActions();

  resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
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

  QMenu *edit_menu = menuBar()->addMenu(tr("&Edit"));

  mirror_horizontally_action_ = edit_menu->addAction(tr("Mirror &Horizontally"), this, &MainWindow::mirrorHorizontally);
  mirror_horizontally_action_->setEnabled(false);

  mirror_vertically_action_ = edit_menu->addAction(tr("Mirror &Vertically"), this, &MainWindow::mirrorVertically);
  mirror_vertically_action_->setEnabled(false);

  convert_to_monochrome_action_ = edit_menu->addAction(tr("Convert to &Grayscale"), this, &MainWindow::convertToGrayscale);
  convert_to_monochrome_action_->setEnabled(false);

  quantize_image_action_ = edit_menu->addAction(tr("&Quantize Image"), this, &MainWindow::quantizeImage);
  quantize_image_action_->setEnabled(false);

  generate_histogram_action_ = edit_menu->addAction(tr("Generate H&istogram"), this, &MainWindow::generateHistogram);
  generate_histogram_action_->setEnabled(false);

  adjust_brightness_action_ = edit_menu->addAction(tr("Adjust &Brightness"), this, &MainWindow::adjustBrightness);
  adjust_brightness_action_->setEnabled(false);

  adjust_contrast_action_ = edit_menu->addAction(tr("Adjust &Contrast"), this, &MainWindow::adjustContrast);
  adjust_contrast_action_->setEnabled(false);

  get_negative_action_ = edit_menu->addAction(tr("Get &Negative Image"), this, &MainWindow::getNegative);
  get_negative_action_->setEnabled(false);

  equalize_histogram_action_ = edit_menu->addAction(tr("&Equalize Histogram"), this, &MainWindow::equalizeHistogram);
  equalize_histogram_action_->setEnabled(false);

  match_histogram_action_ = edit_menu->addAction(tr("&Match Histogram"), this, &MainWindow::matchHistogram);
  match_histogram_action_->setEnabled(false);

  zoom_out_action_ = edit_menu->addAction(tr("Zoom &Out"), this, &MainWindow::zoomOut);
  zoom_out_action_->setEnabled(false);

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
  fit_to_window_action_->setEnabled(!image_.isNull());
  mirror_horizontally_action_->setEnabled(!image_.isNull());
  mirror_vertically_action_->setEnabled(!image_.isNull());
  convert_to_monochrome_action_->setEnabled(!image_.isNull());
  quantize_image_action_->setEnabled(!image_.isNull() && image_.isGrayscale());
  generate_histogram_action_->setEnabled(!image_.isNull() && image_.isGrayscale());
  adjust_brightness_action_->setEnabled(!image_.isNull());
  adjust_contrast_action_->setEnabled(!image_.isNull());
  get_negative_action_->setEnabled(!image_.isNull());
  equalize_histogram_action_->setEnabled(!image_.isNull());
  match_histogram_action_->setEnabled(!image_.isNull() && image_.isGrayscale());
  zoom_out_action_->setEnabled(!image_.isNull());
}

void MainWindow::initializeImageFileDialog(QFileDialog& dialog, QFileDialog::AcceptMode accept_mode)
{
  if (is_first_dialog_) {
    is_first_dialog_ = false;
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

  const QString message = tr("Opened \"%1\", %2x%3")
      .arg(QDir::toNativeSeparators(file_name)).arg(image_.width()).arg(image_.height());
  statusBar()->showMessage(message);
  return true;
}

void MainWindow::setImage(const QImage& new_image)
{
  image_ = new_image;
  pixmap_left_ = QPixmap::fromImage(image_);
  image_label_left_->setPixmap(pixmap_left_);

  // Clear right image
  pixmap_right_ = QPixmap();
  image_label_right_->clear();
  image_label_right_->adjustSize();
  updateActions();

  fitToWindow();
}

void MainWindow::saveAs()
{
  QFileDialog dialog(this, tr("Save File As"));
  initializeImageFileDialog(dialog, QFileDialog::AcceptSave);

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

void MainWindow::mirrorHorizontally()
{
  image_ = image_op::mirrorHorizontally(image_);
  pixmap_right_ = QPixmap::fromImage(image_);
  fitToWindow();
  statusBar()->showMessage("Image mirrored horizontally");
}

void MainWindow::mirrorVertically()
{
  image_ = image_op::mirrorVertically(image_);
  pixmap_right_ = QPixmap::fromImage(image_);
  fitToWindow();
  statusBar()->showMessage("Image mirrored vertically");
}

void MainWindow::convertToGrayscale()
{
  image_ = image_op::convertColoredToGrayscale(image_);
  pixmap_right_ = QPixmap::fromImage(image_);
  fitToWindow();
  updateActions();
  statusBar()->showMessage("Image converted to grayscale");
}

void MainWindow::quantizeImage()
{
  bool ok;
  int num_colors = QInputDialog::getInt(this, tr("Convert to Monochrome"),
                                        tr("How many colors?"), 255, 1, 255, 1, &ok,
                                        Qt::MSWindowsFixedSizeDialogHint);
  if (!ok)
    return;

  image_ = image_op::quantizeGrayscale(image_, num_colors);
  pixmap_right_ = QPixmap::fromImage(image_);
  fitToWindow();
  updateActions();
  const QString message = tr("Quantized image with %1 color(s)").arg(num_colors);
  statusBar()->showMessage(message);
}

void MainWindow::generateHistogram()
{
  auto histogram_data = image_op::generateGrayscaleHistogramData(image_);
  auto histogram = image_op::generate2DHistogramPixmap(histogram_data);

  QPointer<QLabel> histogram_label = new QLabel();
  histogram_label->setWindowTitle("Histogram");
  histogram_label->setPixmap(histogram);
  histogram_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  histogram_label->adjustSize();
  histogram_label->show();
  // Adding a margin of 20 pixels to better visualize the edges of the histogram
  histogram_label->resize(276, 276);

  updateActions();
  statusBar()->showMessage("Histogram generated");
}

void MainWindow::adjustBrightness()
{
  bool ok;
  int brightness_value = QInputDialog::getInt(this, tr("Adjust brightness"),
                                              tr("Add which value?"), 0, -255, 255, 1, &ok,
                                              Qt::MSWindowsFixedSizeDialogHint);
  if (!ok)
    return;

  image_ = image_op::adjustBrightness(image_, brightness_value);
  pixmap_right_ = QPixmap::fromImage(image_);
  fitToWindow();
  updateActions();
  const QString message = tr("Adjusted image brightness by %1").arg(brightness_value);
  statusBar()->showMessage(message);
}

void MainWindow::adjustContrast()
{
  bool ok;
  int contrast_factor = QInputDialog::getInt(this, tr("Adjust contrast"),
                                             tr("By which factor?"), 1, 1, 255, 1, &ok,
                                             Qt::MSWindowsFixedSizeDialogHint);
  if (!ok)
    return;

  image_ = image_op::adjustContrast(image_, contrast_factor);
  pixmap_right_ = QPixmap::fromImage(image_);
  fitToWindow();
  updateActions();
  const QString message = tr("Adjusted image contrast by a factor of %1").arg(contrast_factor);
  statusBar()->showMessage(message);
}

void MainWindow::getNegative()
{
  image_ = image_op::getNegativeImage(image_);
  pixmap_right_ = QPixmap::fromImage(image_);
  fitToWindow();
  statusBar()->showMessage("Generated negative image");
}

void MainWindow::equalizeHistogram()
{
  // Update left image to show image before equalization
  pixmap_left_ = QPixmap::fromImage(image_);

  if (image_.isGrayscale()) {
    // Original image histogram
    auto histogram_data = image_op::generateGrayscaleHistogramData(image_);
    auto original_histogram = image_op::generate2DHistogramPixmap(histogram_data);

    // Histogram equalization
    image_ = image_op::equalizeHistogram(image_);
    pixmap_right_ = QPixmap::fromImage(image_);
    fitToWindow();

    // Modified image equalization
    histogram_data = image_op::generateGrayscaleHistogramData(image_);
    auto modified_histogram = image_op::generate2DHistogramPixmap(histogram_data);

    // Show original and modified histogram side by side
    QPointer<QWidget> histogram_window = new QWidget();
    histogram_window->setWindowTitle("Original histogram vs modified histogram");

    QPointer<QHBoxLayout> histogram_layout = new QHBoxLayout();

    QPointer<QLabel> original_histogram_label = new QLabel();
    original_histogram_label->setPixmap(original_histogram);
    original_histogram_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    original_histogram_label->adjustSize();

    QPointer<QLabel> modified_histogram_label = new QLabel();
    modified_histogram_label->setPixmap(modified_histogram);
    modified_histogram_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    modified_histogram_label->adjustSize();

    histogram_layout->addWidget(original_histogram_label);
    histogram_layout->addWidget(modified_histogram_label);
    histogram_layout->setSpacing(10);

    histogram_window->setLayout(histogram_layout);
    histogram_window->adjustSize();
    histogram_window->show();
  } else {
    // Histogram equalization
    image_ = image_op::equalizeHistogram(image_);
    pixmap_right_ = QPixmap::fromImage(image_);
    fitToWindow();
  }

  statusBar()->showMessage("Equalized image histogram");
}

bool MainWindow::loadGrayscaleImage(const QString& file_name, QImage& image)
{
  QImageReader reader(file_name);
  reader.setAutoTransform(true);
  image = reader.read();

  if (image.isNull()) {
    QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                             tr("Cannot load %1: %2")
                             .arg(QDir::toNativeSeparators(file_name), reader.errorString()));
    return false;
  } else if (!image.isGrayscale()) {
    QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                             tr("Cannot load %1: must be a grayscale image")
                             .arg(QDir::toNativeSeparators(file_name)));
    return false;
  }

  return true;
}

void MainWindow::matchHistogram()
{
  QFileDialog dialog(this, tr("Open File"));
  initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

  QImage target_image;

  while (dialog.exec() == QDialog::Accepted
         && !loadGrayscaleImage(dialog.selectedFiles().first(), target_image));

  if (target_image.isNull())
    return;

  image_ = image_op::matchGrayscaleHistogram(image_, target_image);
  pixmap_right_ = QPixmap::fromImage(image_);
  fitToWindow();

  statusBar()->showMessage("Matched image histogram");
}

void MainWindow::zoomOut()
{
  bool ok;
  int sx = QInputDialog::getInt(this, tr("Zoom out"),
                                      tr("Factor on x axis:"), 1, 1, image_.width(), 1, &ok,
                                      Qt::MSWindowsFixedSizeDialogHint);
  if (!ok)
    return;

  int sy = QInputDialog::getInt(this, tr("Zoom out"),
                                      tr("Factor on y axis:"), 1, 1, image_.height(), 1, &ok,
                                      Qt::MSWindowsFixedSizeDialogHint);
  if (!ok)
    return;

  image_ = image_op::zoomOutByFactors(image_, sx, sy);
  pixmap_right_ = QPixmap::fromImage(image_);
  fitToWindow();
  const QString message = tr("Zoomed out image by a factor of %1 and %2").arg(sx).arg(sy);
  statusBar()->showMessage(message);
}

void MainWindow::fitToWindow()
{
  if (fit_to_window_action_->isChecked()) {
    image_label_left_->setPixmap(pixmap_left_.scaled(scroll_area_left_->width(),
                                                     scroll_area_left_->height(),
                                                     Qt::KeepAspectRatio,
                                                     Qt::SmoothTransformation));
    image_label_left_->adjustSize();

    if (!pixmap_right_.isNull()) {
      image_label_right_->setPixmap(pixmap_right_.scaled(scroll_area_right_->width(),
                                                         scroll_area_right_->height(),
                                                         Qt::KeepAspectRatio,
                                                         Qt::SmoothTransformation));
      image_label_right_->adjustSize();
    }

    statusBar()->showMessage("Adjusted image to available space");
  } else {
    image_label_left_->setPixmap(pixmap_left_);
    image_label_left_->adjustSize();

    image_label_right_->setPixmap(pixmap_right_);
    image_label_right_->adjustSize();

    statusBar()->showMessage("Showing image in original size");
  }
}

void MainWindow::about()
{
  QMessageBox::about(this, tr("About PhotoChopp"),
                     tr("<p><b>PhotoChopp</b> is a class project for the image processing fundamentals class"
                        "on UFRGS 2018/2. This instance was developed by Jéferson Ferreira Guimarães.</p>"));
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
  QMainWindow::resizeEvent(event);

  if (!image_.isNull())
    fitToWindow();
}
