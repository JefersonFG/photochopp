#pragma once

#include <QMainWindow>
#include <QAction>
#include <QImage>
#include <QLabel>
#include <QScrollArea>
#include <QFileDialog>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

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
  void scaleImage(double factor);
  void adjustScrollBar(QScrollBar* scroll_bar, double factor);
  void initializeImageFileDialog(QFileDialog& dialog, QFileDialog::AcceptMode accept_mode);

  Ui::MainWindow* ui;

  bool first_dialog_;
  QImage image_;
  QLabel* image_label_left_;
  QLabel* image_label_right_;
  QScrollArea* scroll_area_left_;
  QScrollArea* scroll_area_right_;

  QAction* save_as_action_;
  QAction* fit_to_window_action_;
};
