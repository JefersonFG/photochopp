#pragma once

#include <QImage>

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

} // namespace image_op
