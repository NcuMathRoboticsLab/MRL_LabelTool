#ifndef MAKE_FEATURES__
#define MAKE_FEATURES__

/**
 * @file make_feature.h
 * @author Mes (mes900903@gmail.com) (Discord: Mes#0903)
 * @brief Calculate the feature of the matrix, I used it to transform the laser data from segment to feature matrix.
 * @version 0.1
 * @date 2022-11-17
 */

#include "Eigen/Eigen"

#include <vector>
#include <utility>
#include <cmath>

constexpr int FEATURE_NUM = 10;

namespace MakeFeatures {

  /**
   * @brief Append a row to the matrix.
   *
   * @param A The target matrix wanna append a row.
   * @param B The vector would be appended.
   * @return Eigen::MatrixXd The result matrix after complete appending.
   */
  Eigen::MatrixXd AppendRow(const Eigen::MatrixXd &A, const Eigen::VectorXd &B);

  /**
   * @brief Calculate the point of the segment.
   *
   * @param Seg The segment data matrix. It's an Sn*2 matrix, Sn is the number of segments.
   * @return int The points number of the segment, which is the Sn above.
   */
  int cal_point(const Eigen::MatrixXd &Seg);

  /**
   * @brief Calculate the standard deviation of the segment.
   *
   * @param Seg The segment data matrix. It's an Sn*2 matrix, Sn is the number of segments.
   * @return double The standard deviation.
   */
  double cal_std(const Eigen::MatrixXd &Seg);

  /**
   * @brief Calculate the width of the segment, which is the distance of first point and the last point.
   *
   * @param Seg The segment data matrix. It's an Sn*2 matrix, Sn is the number of segments.
   * @return double The width
   */
  double cal_width(const Eigen::MatrixXd &Seg);

  /**
   * @brief Calculate the circularity and the radius of the segment.
   *
   * @param Seg The segment data matrix. It's an Sn*2 matrix, Sn is the number of segments.
   * @return std::tuple<double, double, double>
   */
  std::tuple<double, double, double> cal_cr(const Eigen::MatrixXd &Seg);

  std::tuple<double, double, double, double> cal_linearity(Eigen::MatrixXd Seg);

  /**
   * @brief Making the feature matrix by the segment data.
   *
   * @param Seg The segment data matrix. It's an Sn*2 matrix, Sn is the number of segments.
   * @return Eigen::VectorXd
   */
  Eigen::ArrayXd make_feature(const Eigen::MatrixXd &Seg);

  Eigen::MatrixXd segment_to_feature(const std::vector<Eigen::MatrixXd> &section_seg_vec);

  /**
   * @brief Transform the section xy data to the feature matrix.
   *
   * @param xy_data The section xy data. On my minibots, the matrix is 720*2
   * @return std::pair<Eigen::MatrixXd, std::vector<Eigen::MatrixXd>> feature matrix and a vector containing all the segments in one second.
   */
  std::pair<Eigen::MatrixXd, std::vector<Eigen::MatrixXd>> section_to_feature(const Eigen::MatrixXd &xy_data);    // xy_data is 720*2
}    // namespace MakeFeatures

#endif