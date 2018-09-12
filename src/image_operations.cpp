#include "include/image_operations.hpp"

namespace image_op {

QImage mirrorHorizontally(QImage image)
{
  int width = image.width();
  int height = image.height();

  for (int row_index = 0; row_index < height; row_index++) {
    QRgb* line = (QRgb*) image.scanLine(row_index);
    for (int column_index = 0; column_index < width / 2; column_index++) {
      std::swap(line[column_index], line[width - 1 - column_index]);
    }
  }

  return image;
}

QImage mirrorVertically(QImage image)
{
  int width = image.width();
  int height = image.height();
  QRgb* buffer = new QRgb[width * sizeof(QRgb)];

  try {
    for (int row_index = 0; row_index < height / 2; row_index++) {
      QRgb* first_line = (QRgb*) image.scanLine(row_index);
      QRgb* second_line = (QRgb*) image.scanLine(height - 1 - row_index);
      std::memcpy(buffer, first_line, width * sizeof(QRgb));
      std::memcpy(first_line, second_line, width * sizeof(QRgb));
      std::memcpy(second_line, buffer, width * sizeof(QRgb));
    }
  } catch (...) {
    delete[] buffer;
    throw;
  }

  delete[] buffer;
  return image;
}

QImage convertColoredToGrayscale(QImage image)
{
  int width = image.width();
  int height = image.height();

  for (int row_index = 0; row_index < height; row_index++) {
    QRgb* line = (QRgb*) image.scanLine(row_index);
    for (int column_index = 0; column_index < width; column_index++) {
      auto* pixel = &line[column_index];
      auto luminance = 0.299 * qRed(*pixel) + 0.587 * qGreen(*pixel) + 0.114 * qBlue(*pixel);
      *pixel = qRgb(luminance, luminance, luminance);
    }
  }

  return image;
}

QImage quantizeGrayscale(QImage image, int num_colors)
{
  int width = image.width();
  int height = image.height();

  int step = 0;
  if (num_colors > 1)
    step = 255 / (num_colors - 1);

  for (int row_index = 0; row_index < height; row_index++) {
    QRgb* line = (QRgb*) image.scanLine(row_index);
    for (int column_index = 0; column_index < width; column_index++) {
      auto* pixel = &line[column_index];
      auto luminance = 0.299 * qRed(*pixel) + 0.587 * qGreen(*pixel) + 0.114 * qBlue(*pixel);
      auto color = std::round(luminance / step) * step;
      *pixel = qRgb(color, color, color);
    }
  }

  return image;
}

}
