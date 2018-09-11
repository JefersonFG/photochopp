#pragma once

#include <QMainWindow>
#include <QAction>
#include <QImage>
#include <QLabel>
#include <QPixMap>
#include <QScrollArea>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPointer>

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);

private slots:
  void open();
  void saveAs();
  void fitToWindow();
  void about();

private:
  void createActions();
  void updateActions();
  bool loadFile(const QString& file_name);
  bool saveFile(const QString& file_name);
  void setImage(const QImage& new_image);
  void initializeImageFileDialog(QFileDialog& dialog, QFileDialog::AcceptMode accept_mode);
  void mirrorHorizontally();
  void mirrorVertically();
  void convertToMonochrome();
  void quantizeImage();

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
  QAction* fit_to_window_action_;
  QAction* mirror_horizontally_action_;
  QAction* mirror_vertically_action_;
  QAction* convert_to_monochrome_action_;
  QAction* quantize_image_action_;
};
