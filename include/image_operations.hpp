#pragma once

#include <vector>

#include <QImage>
#include <QPixmap>

namespace image_op {

/**
 * Mirrors image horizontally swapping
 * the values on each bit
 */
QImage mirrorHorizontally(QImage image);

/**
 * Mirrors image vertically swapping
 * entire lines at once
 */
QImage mirrorVertically(QImage image);

/**
 * Convert colored image to grayscale calculating
 * the luminance for each pixel then making
 * L = R = G = B
 */
QImage convertColoredToGrayscale(QImage image);

/**
 * Quantize a grayscale image by defining num_colors - 1
 * intervals and aligning each pixel color to the closest
 * separator of the intervals
 * Assumes 8-bit image
 */
QImage quantizeGrayscale(QImage image, int num_colors);

/**
 * Generates the histogram data of a grayscale 8-bit image
 * @return 256 position vector with density of tones
 */
std::vector<int> generateGrayscaleHistogramData(QImage image);

/**
 * Generates the 2D histogram bitmap of an image based on its data
 * @return Pixmap with a 2D histogram
 */
QPixmap generate2DHistogramPixmap(std::vector<int> histogram_data);

} // namespace image_op
