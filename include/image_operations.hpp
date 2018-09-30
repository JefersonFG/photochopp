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

/**
 * Adjusts the brightness of an image by adding the desired value to
 * all the pixels on all channels separatedly and then combining them
 */
QImage adjustBrightness(QImage image, int brightness_value);

/**
 * Adjusts the contrast of an image by multiplying the desired value to
 * all the pixels on all channels separatedly and then combining them
 */
QImage adjustContrast(QImage image, int contrast_factor);

/**
 * Returns the negative of the image by inverting the value of each pixel
 * on each channel separatedly
 * Assumes 8-bit image
 */
QImage getNegativeImage(QImage image);

/**
 * Equalizes the image histogram using the cumulative luminance histogram
 */
QImage equalizeHistogram(QImage image);

/**
 * Matches the histogram of the original image with the target image,
 * assuming both grayscale 8-bit images
 */
QImage matchGrayscaleHistogram(QImage original_image, QImage target_image);

} // namespace image_op
