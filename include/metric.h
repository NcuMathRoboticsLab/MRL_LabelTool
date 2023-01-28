#ifndef METRIC_H__
#define METRIC_H__

/**
 * @file metric.h
 * @author Mes (mes900903@gmail.com) (Discord: Mes#0903)
 * @brief The metric related functions.
 * @version 0.1
 * @date 2022-11-18
 */

#include "Eigen/Eigen"

#include <tuple>
#include <cmath>

namespace metric {
  /**
   * @brief Calculate the confusion table.
   *
   * @param y The label of the data.
   * @param pred_Y The predicted output of the data.
   * @return Eigen::MatrixXd The confusion table.
   */
  Eigen::MatrixXd cal_confusion_matrix(const Eigen::VectorXd &y, const Eigen::VectorXd &pred_Y);

  /**
   * @brief Transforming the matrix from [theta, r] data to [x, y] data.
   *
   * @param data The [thera, r] matrix.
   * @param ROWS The rows number of the matrix.
   */
  void rtheta_to_xy(Eigen::MatrixXd &data, const int ROWS);

  /**
   * @brief Transform the xy data to segments data.
   *
   * @param section A section of the xy data, in my case, laser data each seconds is a section, thus the section is a 720*2 matrix.
   * @return std::vector<Eigen::MatrixXd> The std::vector of the segments,
   *         i.e., vec[0] is the first segment, vec[1] is the second segment.
   */
  std::vector<Eigen::MatrixXd> section_to_segment(const Eigen::MatrixXd &section);    // section is 720*2
}    // namespace metric

#endif