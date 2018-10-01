#pragma once

#include <QMainWindow>
#include <QAction>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QScrollArea>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPointer>

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

private slots:
  /**
   * Shows a file open dialog to the user
   */
  void open();

  /**
   * Shows a file save dialog to the user
   */
  void saveAs();

  /**
   * Updates the image on screen, changing to/from fit to space available to/from original size
   */
  void fitToWindow();

  /**
   * Shows information about the software and its implementation
   */
  void about();

private:
  /**
   * Creates menu actions
   */
  void createActions();

  /**
   * Updates menu actions, enabling/disabling options based on current image state
   */
  void updateActions();

  /**
   * Loads image to memory
   */
  bool loadFile(const QString& file_name);

  /**
   * Saves modified image to disk
   */
  bool saveFile(const QString& file_name);

  /**
   * Set loaded image to display on left panel and reset right panel
   */
  void setImage(const QImage& new_image);

  /**
   * Configures file dialogs to default to jpeg images
   */
  void initializeImageFileDialog(QFileDialog& dialog, QFileDialog::AcceptMode accept_mode);

  /**
   * Applies horizontal mirroring operation on the current image
   */
  void mirrorHorizontally();

  /**
   * Applies vertical mirroring operation on the current image
   */
  void mirrorVertically();

  /**
   * Applies grayscale conversion operation on the current image
   */
  void convertToGrayscale();

  /**
   * Applies quantization operation on the current image
   */
  void quantizeImage();

  /**
   * Generates and shows the histogram of the current image
   */
  void generateHistogram();

  /**
   * Adjusts the brightness of the current image
   */
  void adjustBrightness();

  /**
   * Adjusts the constrast of the current image
   */
  void adjustContrast();

  /**
   * Gets the negative of the image
   */
  void getNegative();

  /**
   * Equalizes the image histogram, showing the original
   * and posterior histogram for grayscale images
   */
  void equalizeHistogram();

  /**
   * Loads grayscale image, fails if is colored
   */
  bool loadGrayscaleImage(const QString& file_name, QImage& image);

  /**
   * Matches the current grayscale image histogram with
   * the histogram of a selected grayscale image
   */
  void matchHistogram();

  /**
   * Applies zoom out to the image with parameters input by the user
   */
  void zoomOut();

  /**
   * Window resize event, updates image size
   */
  void resizeEvent(QResizeEvent* event);

  bool is_first_dialog_;

  QImage image_;
  QPixmap pixmap_left_;
  QPixmap pixmap_right_;

  QPointer<QLabel> image_title_left_;
  QPointer<QLabel> image_title_right_;
  QPointer<QLabel> image_label_left_;
  QPointer<QLabel> image_label_right_;
  QPointer<QScrollArea> scroll_area_left_;
  QPointer<QScrollArea> scroll_area_right_;

  QWidget central_widget_;
  QHBoxLayout horizontal_layout_;
  QVBoxLayout vertical_layout_left_;
  QVBoxLayout vertical_layout_right_;

  QAction* save_as_action_;
  QAction* mirror_horizontally_action_;
  QAction* mirror_vertically_action_;
  QAction* convert_to_monochrome_action_;
  QAction* quantize_image_action_;
  QAction* generate_histogram_action_;
  QAction* adjust_brightness_action_;
  QAction* adjust_contrast_action_;
  QAction* get_negative_action_;
  QAction* equalize_histogram_action_;
  QAction* match_histogram_action_;
  QAction* zoom_out_action_;
  QAction* fit_to_window_action_;
};
