/**
 * @file metric.cpp
 * @author Mes (mes900903@gmail.com) (Discord: Mes#0903)
 * @brief The metric related functions.
 * @version 0.1
 * @date 2022-12-15
 */

#include "metric.h"
#include "Eigen/Eigen"

#include <tuple>
#include <cmath>

#if _WIN32
#define _USE_MATH_DEFINES
#include <math.h>
#endif

namespace metric {
  /**
   * @brief Calculate the confusion table.
   *
   * @param y The label of the data.
   * @param pred_Y The predicted output of the data.
   * @return Eigen::MatrixXd The confusion table.
   */
  Eigen::MatrixXd cal_confusion_matrix(const Eigen::VectorXd &y, const Eigen::VectorXd &pred_Y)
  {
    int R = y.size();
    int TP{}, FP{}, FN{}, TN{};

    for (int i = 0; i < R; ++i) {
      if (y(i) == 0 && pred_Y(i) == 0)
        ++TN;
      else if (y(i) == 0 && pred_Y(i) == 1)
        ++FP;
      else if (y(i) == 1 && pred_Y(i) == 1)
        ++TP;
      else if (y(i) == 1 && pred_Y(i) == 0)
        ++FN;
    }

    Eigen::MatrixXd confusion(2, 2);
    confusion << TP, FP, FN, TN;
    return confusion;
  }

  /**
   * @brief Transforming the matrix from [theta, r] data to [x, y] data.
   *
   * @param data The [thera, r] matrix.
   * @param ROWS The rows number of the matrix.
   */
  void rtheta_to_xy(Eigen::MatrixXd &data, const int ROWS)
  {
    for (int i = 0; i < ROWS; i++) {
      const double theta = M_PI * data(i, 0) / 180;    // transform the radian to angle
      const double r = data(i, 1);    // radius

      data(i, 0) = r * std::cos(theta);    // x
      data(i, 1) = r * std::sin(theta);    // y
    }
  }

  /**
   * @brief Transform the xy data to segments data.
   *
   * @param section A section of the xy data, in my case, laser data each seconds is a section, thus the section is a 720*2 matrix.
   * @return std::vector<Eigen::MatrixXd> The std::vector of the segments,
   *         i.e., vec[0] is the first segment, vec[1] is the second segment.
   */
  std::vector<Eigen::MatrixXd> section_to_segment(const Eigen::MatrixXd &section)    // section is 720*2
  {
    const int ROWS = section.rows();
    const auto &x = section.col(0);
    const auto &y = section.col(1);
    const double threshold = 0.1;
    std::vector<Eigen::MatrixXd> seg_vec;    // a seg is a n*2 matrix.

    std::vector<int> valid_index;    // valid point index
    for (int i = 0; i < ROWS; ++i) {
      if ((x(i) != 0 || y(i) != 0) && (std::isnormal(x(i)) && std::isnormal(y(i))))    // if the xy is [0,0], it's not valid; if the x or y is sth like nan, it's not valid too
        valid_index.emplace_back(i);
    }
    int validsize = valid_index.size();    // the number of valid point in the section.
    bool first_end = std::sqrt(std::pow(x(valid_index[0]) - x(valid_index[validsize - 1]), 2) + std::pow(y(valid_index[0]) - y(valid_index[validsize - 1]), 2)) < threshold;

    std::vector<int> single_seg;    // The valid xy point index list of one segment.

    single_seg.emplace_back(valid_index[0]);    // The first point
    for (int i = 1; i < validsize; ++i)    // traverse all valid point and devide it into segment
    {
      // if std::sqrt( (x1 - x0)^2 + (y1-y0)^2 ) >= threshold, it's means the previous points in the `single_seq` are belongs to same segment.
      // Thus make it as a matrix and push it into the seg_vec
      if (std::sqrt(std::pow(x(valid_index[i - 1]) - x(valid_index[i]), 2) + std::pow(y(valid_index[i - 1]) - y(valid_index[i]), 2)) >= threshold) {
        const int node_num = single_seg.size();    // the numbers of the point in the one segment
        Eigen::MatrixXd tmp_seg(node_num, 2);
        for (int j = 0; j < node_num; ++j) {
          tmp_seg(j, 0) = x(single_seg[j]);
          tmp_seg(j, 1) = y(single_seg[j]);
        }

        seg_vec.push_back(std::move(tmp_seg));    // push the matrix into the segment list, which represents the xy data of a segment
        single_seg.clear();
      }

      single_seg.emplace_back(valid_index[i]);    // The next segment start point.
    }

    const int node_num = single_seg.size();    // the numbers of the point in the one segment

    Eigen::MatrixXd tmp_seg(node_num, 2);
    for (int j = 0; j < node_num; ++j) {
      tmp_seg(j, 0) = x(single_seg[j]);
      tmp_seg(j, 1) = y(single_seg[j]);
    }

    seg_vec.push_back(std::move(tmp_seg));    // push the matrix into the segment list, which represents the xy data of a segment

    if (first_end) {
      Eigen::MatrixXd &first = seg_vec[0];
      Eigen::MatrixXd &end = seg_vec[seg_vec.size() - 1];

      Eigen::MatrixXd tmp(first.rows() + end.rows(), first.cols());
      tmp << end, first;

      seg_vec[0] = std::move(tmp);
      seg_vec.pop_back();
    }

    return seg_vec;
  }
}    // namespace metric
