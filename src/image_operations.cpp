#include "include/image_operations.hpp"

#include <QPainter>

namespace image_op {

QImage mirrorHorizontally(QImage image)
{
  int width = image.width();
  int height = image.height();

  for (int row_index = 0; row_index < height; row_index++) {
    QRgb* line = reinterpret_cast<QRgb*>(image.scanLine(row_index));
    for (int column_index = 0; column_index < width / 2; column_index++) {
      std::swap(line[column_index], line[width - 1 - column_index]);
    }
  }

  return image;
}

QImage mirrorVertically(QImage image)
{
  auto width = static_cast<size_t>(image.width());
  auto height = image.height();
  QRgb* buffer = new QRgb[width * sizeof(QRgb)];

  try {
    for (int row_index = 0; row_index < height / 2; row_index++) {
      QRgb* first_line = reinterpret_cast<QRgb*>(image.scanLine(row_index));
      QRgb* second_line = reinterpret_cast<QRgb*>(image.scanLine(height - 1 - row_index));
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
  if (image.isGrayscale())
    return image;

  int width = image.width();
  int height = image.height();

  for (int row_index = 0; row_index < height; row_index++) {
    QRgb* line = reinterpret_cast<QRgb*>(image.scanLine(row_index));
    for (int column_index = 0; column_index < width; column_index++) {
      auto* pixel = &line[column_index];
      auto luminance = static_cast<int>(0.299 * qRed(*pixel) + 0.587 * qGreen(*pixel) + 0.114 * qBlue(*pixel));
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
    QRgb* line = reinterpret_cast<QRgb*>(image.scanLine(row_index));
    for (int column_index = 0; column_index < width; column_index++) {
      auto* pixel = &line[column_index];
      auto luminance = 0.299 * qRed(*pixel) + 0.587 * qGreen(*pixel) + 0.114 * qBlue(*pixel);
      auto color = static_cast<int>(std::round(luminance / step) * step);
      *pixel = qRgb(color, color, color);
    }
  }

  return image;
}

std::vector<int> generateGrayscaleHistogramData(QImage image)
{
  std::vector<int> histogram(256);
  int width = image.width();
  int height = image.height();

  for (int row_index = 0; row_index < height; row_index++) {
    QRgb* line = reinterpret_cast<QRgb*>(image.scanLine(row_index));
    for (int column_index = 0; column_index < width; column_index++) {
      auto* pixel = &line[column_index];
      // Since it is a grayscale image each channel has the same value
      histogram[static_cast<size_t>(qRed(*pixel))]++;
    }
  }

  return histogram;
}

QPixmap generate2DHistogramPixmap(std::vector<int> histogram_data)
{
  auto max_histogram = std::max_element(histogram_data.begin(), histogram_data.end());

  QPixmap histogram(256, 256);
  histogram.fill();

  QPainter painter(&histogram);
  painter.setPen(Qt::black);

  for (int i = 0; i < 256; i++) {
    auto line_height = std::round(histogram_data[static_cast<size_t>(i)] * 1.0 / *max_histogram * 255);
    painter.drawLine(i, 255, i, 255 - static_cast<int>(line_height));
  }

  return histogram;
}

QImage adjustBrightness(QImage image, int brightness_value)
{
  int width = image.width();
  int height = image.height();

  for (int row_index = 0; row_index < height; row_index++) {
    QRgb* line = reinterpret_cast<QRgb*>(image.scanLine(row_index));
    for (int column_index = 0; column_index < width; column_index++) {
      auto* pixel = &line[column_index];
      auto red = qRed(*pixel) + brightness_value;
      red = red > 255 ? 255 : red < 0 ? 0 : red;
      auto green = qGreen(*pixel) + brightness_value;
      green = green > 255 ? 255 : green < 0 ? 0 : green;
      auto blue = qBlue(*pixel) + brightness_value;
      blue = blue > 255 ? 255 : blue < 0 ? 0 : blue;
      *pixel = qRgb(red, green, blue);
    }
  }

  return image;
}

QImage adjustContrast(QImage image, int contrast_factor)
{
  int width = image.width();
  int height = image.height();

  for (int row_index = 0; row_index < height; row_index++) {
    QRgb* line = reinterpret_cast<QRgb*>(image.scanLine(row_index));
    for (int column_index = 0; column_index < width; column_index++) {
      auto* pixel = &line[column_index];
      auto red = qRed(*pixel) * contrast_factor;
      red = red > 255 ? 255 : red < 0 ? 0 : red;
      auto green = qGreen(*pixel) * contrast_factor;
      green = green > 255 ? 255 : green < 0 ? 0 : green;
      auto blue = qBlue(*pixel) * contrast_factor;
      blue = blue > 255 ? 255 : blue < 0 ? 0 : blue;
      *pixel = qRgb(red, green, blue);
    }
  }

  return image;
}

QImage getNegativeImage(QImage image)
{
  int width = image.width();
  int height = image.height();

  for (int row_index = 0; row_index < height; row_index++) {
    QRgb* line = reinterpret_cast<QRgb*>(image.scanLine(row_index));
    for (int column_index = 0; column_index < width; column_index++) {
      auto* pixel = &line[column_index];
      auto red = 255 - qRed(*pixel);
      auto green = 255 - qGreen(*pixel);
      auto blue = 255 - qBlue(*pixel);
      *pixel = qRgb(red, green, blue);
    }
  }

  return image;
}

}
