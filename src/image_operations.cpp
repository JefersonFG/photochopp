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

QImage equalizeHistogram(QImage image)
{
  // TODO(jfguimaraes) Implement L*a*b color space
  int width = image.width();
  int height = image.height();
  std::vector<int> histogram_data;

  if (image.isGrayscale())
    histogram_data = generateGrayscaleHistogramData(image);
  else
    histogram_data = generateGrayscaleHistogramData(convertColoredToGrayscale(image));

  std::vector<int> cumulative_histogram(256);
  double alpha = 255.0 / (width * height);

  // Generate cumulative histogram of luminance channel
  cumulative_histogram[0] = static_cast<int>(std::round(alpha * histogram_data[0]));

  for (size_t i = 1; i < 256; i++)
    cumulative_histogram[i] = cumulative_histogram[i-1] + static_cast<int>(std::round(alpha * histogram_data[i]));

  // Update pixel values
  for (int row_index = 0; row_index < height; row_index++) {
    QRgb* line = reinterpret_cast<QRgb*>(image.scanLine(row_index));
    for (int column_index = 0; column_index < width; column_index++) {
      auto* pixel = &line[column_index];
      auto red = cumulative_histogram[static_cast<size_t>(qRed(*pixel))];
      auto green = cumulative_histogram[static_cast<size_t>(qGreen(*pixel))];
      auto blue = cumulative_histogram[static_cast<size_t>(qBlue(*pixel))];
      *pixel = qRgb(red, green, blue);
    }
  }

  return image;
}

QImage matchGrayscaleHistogram(QImage original_image, QImage target_image)
{
  int width = original_image.width();
  int height = original_image.height();

  // Get histograms
  auto original_histogram_data = generateGrayscaleHistogramData(original_image);
  auto target_histogram_data = generateGrayscaleHistogramData(target_image);

  // Get cumulative histograms
  // TODO(jfguimaraes) Function for cumulative histograms
  std::vector<int> original_cumulative_histogram(256);
  double original_alpha = 255.0 / (width * height);

  original_cumulative_histogram[0] = static_cast<int>(std::round(original_alpha * original_histogram_data[0]));

  for (size_t i = 1; i < 256; i++) {
    original_cumulative_histogram[i] = original_cumulative_histogram[i-1]
        + static_cast<int>(std::round(original_alpha * original_histogram_data[i]));
    original_cumulative_histogram[i] = original_cumulative_histogram[i] > 255 ?
          255 : original_cumulative_histogram[i];
  }

  std::vector<int> target_cumulative_histogram(256);
  double target_alpha = 255.0 / (target_image.width() * target_image.height());

  target_cumulative_histogram[0] = static_cast<int>(std::round(target_alpha * target_histogram_data[0]));

  for (size_t i = 1; i < 256; i++) {
    target_cumulative_histogram[i] = target_cumulative_histogram[i-1]
        + static_cast<int>(std::round(target_alpha * target_histogram_data[i]));
    target_cumulative_histogram[i] = target_cumulative_histogram[i] > 255 ?
          255 : target_cumulative_histogram[i];
  }

  // For each shade map the closest on the target image to the map function
  std::vector<int> map_function(256);

  // TODO(jfguimaraes) Optimize this
  for (int i = 0; i < 256; i++) {
    int original_shade = original_cumulative_histogram[static_cast<size_t>(i)];
    int lowest_difference = 300;
    int nearest_shade_index = 0;

    for (int j = 0; j < 256; j++) {
      int target_shade = target_cumulative_histogram[static_cast<size_t>(j)];

      if (abs(target_shade - original_shade) < lowest_difference) {
        lowest_difference = abs(target_shade - original_shade);
        nearest_shade_index = j;
      }

      if (lowest_difference == 0)
        break;
    }

    map_function[static_cast<size_t>(i)] = nearest_shade_index;
  }

  // Match histogram using the map function
  for (int row_index = 0; row_index < height; row_index++) {
    QRgb* line = reinterpret_cast<QRgb*>(original_image.scanLine(row_index));
    for (int column_index = 0; column_index < width; column_index++) {
      auto* pixel = &line[column_index];
      // Since it is a grayscale image each channel has the same value
      auto color = map_function[static_cast<size_t>(qRed(*pixel))];
      *pixel = qRgb(color, color, color);
    }
  }

  return original_image;
}

QImage zoomOutByFactors(QImage image, int sx, int sy)
{
  int original_width = image.width();
  int original_height = image.height();

  int target_width = static_cast<int>(ceil(original_width * 1.0 / sx));
  int target_height = static_cast<int>(ceil(original_height * 1.0 / sy));

  QImage target_image(target_width, target_height, QImage::Format_RGB32);

  QVector<QRgb*> lines(sy);

  for (int row_index = 0, target_row = 0; row_index < original_height; row_index += sy, target_row++) {
    int rows_read = 0;

    for (int row = row_index, i = 0; row < row_index + sy && row < original_height; row++, i++) {
      lines[i] = reinterpret_cast<QRgb*>(image.scanLine(row));
      rows_read++;
    }

    auto target_line = reinterpret_cast<QRgb*>(target_image.scanLine(target_row));

    for (int column_index = 0, target_column = 0; column_index < original_width; column_index += sx, target_column++) {
      int red = 0;
      int green = 0;
      int blue = 0;
      int num_pixels = 0;

      for (int column = column_index; column < column_index + sx && column < original_width; column++) {
        for (int row = 0; row < rows_read; row++) {
          num_pixels++;
          red += qRed(lines[row][column]);
          green += qGreen(lines[row][column]);
          blue += qBlue(lines[row][column]);
        }
      }

      red /= num_pixels;
      green /= num_pixels;
      blue /= num_pixels;

      auto* pixel = &target_line[target_column];
      *pixel = qRgb(red, green, blue);
    }
  }

  return target_image;
}

QImage zoomIn2x2(QImage image)
{
  int original_width = image.width();
  int original_height = image.height();

  int target_width = 2 * original_width;
  int target_height = 2 * original_height;

  QImage target_image(target_width, target_height, QImage::Format_RGB32);

  // Average of two colors
  auto average = [](QRgb* a, QRgb* b){
    auto red = (qRed(*a) + qRed(*b)) / 2;
    auto green = (qGreen(*a) + qGreen(*b)) / 2;
    auto blue = (qBlue(*a) + qBlue(*b)) / 2;
    return qRgb(red, green, blue);
  };

  // Between columns
  for (int row_index = 0; row_index < original_height; row_index++) {
    int target_row = row_index * 2;
    auto original_line = reinterpret_cast<QRgb*>(image.scanLine(row_index));
    auto target_line = reinterpret_cast<QRgb*>(target_image.scanLine(target_row));

    for (int column_index = 0; column_index < original_width; column_index++) {
      int target_column = column_index * 2;

      // First pixel is equal
      auto* original_pixel = &original_line[column_index];
      auto* target_pixel = &target_line[target_column];
      *target_pixel = *original_pixel;

      // Second pixel is the medium between the current and the next (if it exists)
      target_pixel = &target_line[target_column + 1];

      if (column_index + 2 < original_width) {
        auto* next_pixel = &original_line[column_index + 2];
        *target_pixel = average(original_pixel, next_pixel);
      } else {
        *target_pixel = *original_pixel;
      }
    }
  }

  // Between lines
  for (int row_index = 0; row_index < target_height; row_index += 2) {
    auto current_line = reinterpret_cast<QRgb*>(target_image.scanLine(row_index));
    auto target_line = reinterpret_cast<QRgb*>(target_image.scanLine(row_index + 1));

    if (row_index + 2 >= target_height) {
      std::memcpy(target_line, current_line, static_cast<size_t>(target_width) * sizeof(QRgb));
      break;
    }

    auto next_line = reinterpret_cast<QRgb*>(target_image.scanLine(row_index + 2));

    for (int column_index = 0; column_index < target_width; column_index++) {
      auto* current_pixel = &current_line[column_index];
      auto* target_pixel = &target_line[column_index];
      auto* next_pixel = &next_line[column_index];

      *target_pixel = average(current_pixel, next_pixel);
    }
  }

  return target_image;
}

QImage rotate90DegreesClockwise(QImage image)
{
  int width = image.width();
  int height = image.height();

  // Target image has inverted dimensions
  QImage target_image(height, width, QImage::Format_RGB32);

  std::vector<QRgb*> original_image_lines;
  std::vector<QRgb*> target_image_lines;

  for (int i = 0; i < height; i++)
    original_image_lines.emplace_back(reinterpret_cast<QRgb*>(image.scanLine(i)));

  for (int i = 0; i < width; i++)
    target_image_lines.emplace_back(reinterpret_cast<QRgb*>(target_image.scanLine(i)));

  for (int row_index = 0; row_index < height; row_index++) {
    for (int column_index = 0; column_index < width; column_index++) {
      target_image_lines[static_cast<size_t>(column_index)][height - row_index - 1] =
          original_image_lines[static_cast<size_t>(row_index)][static_cast<size_t>(column_index)];
    }
  }

  return target_image;
}

QImage rotate90DegreesCounterClockwise(QImage image)
{
  int width = image.width();
  int height = image.height();

  // Target image has inverted dimensions
  QImage target_image(height, width, QImage::Format_RGB32);

  std::vector<QRgb*> original_image_lines;
  std::vector<QRgb*> target_image_lines;

  for (int i = 0; i < height; i++)
    original_image_lines.emplace_back(reinterpret_cast<QRgb*>(image.scanLine(i)));

  for (int i = 0; i < width; i++)
    target_image_lines.emplace_back(reinterpret_cast<QRgb*>(target_image.scanLine(i)));

  for (int row_index = 0; row_index < height; row_index++) {
    for (int column_index = 0; column_index < width; column_index++) {
      target_image_lines[static_cast<size_t>(width - column_index - 1)][static_cast<size_t>(row_index)] =
          original_image_lines[static_cast<size_t>(row_index)][static_cast<size_t>(column_index)];
    }
  }

  return target_image;
}

QImage applyConvolutionWith3x3Kernel(QImage image, QVector<QVector<double>> kernel, bool add_bias)
{
  int width = image.width();
  int height = image.height();

  QImage target_image(width, height, QImage::Format_RGB32);
  target_image.fill(Qt::black);

  double sum;
  QVector<QRgb*> original_image_lines;
  QVector<QRgb*> target_image_lines;

  for (int i = 0; i < height; i++) {
    original_image_lines.append(reinterpret_cast<QRgb*>(image.scanLine(i)));
    target_image_lines.append(reinterpret_cast<QRgb*>(target_image.scanLine(i)));
  }

  for (int row = 1; row <= height - 2; row++) {
    for (int column = 1; column <= width - 2; column++) {
      sum = 0.0;

      for (int k = -1; k <= 1; k++) {
        for (int j = -1; j <= 1; j++) {
          // Since it is a grayscale image each channel has the same value
          sum += kernel[1+j][1+k] * qRed(original_image_lines[row-j][column-k]);
        }
      }

      if (add_bias)
        sum += 127;

      sum = sum > 255 ? 255 : sum < 0 ? 0 : sum;
      auto color = static_cast<int>(sum);

      auto& pixel = target_image_lines[row][column];
      pixel = qRgb(color, color, color);
    }
  }

  return target_image;
}

}
