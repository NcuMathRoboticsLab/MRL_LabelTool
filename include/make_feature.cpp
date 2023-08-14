/**
 * @file make_feature.cpp
 * @author Mes (mes900903@gmail.com) (Discord: Mes#0903)
 * @brief Calculate the feature of the matrix, I used it to transform the laser data from segment to feature matrix.
 * @version 0.1
 * @date 2022-12-15
 */

#include "make_feature.h"
#include "metric.h"
#include "Eigen/Eigen"

#include <vector>
#include <utility>
#include <cmath>

namespace MakeFeatures {

  /**
   * @brief Calculate the point of the segment.
   *
   * @param Seg The segment data matrix. It's an Sn*2 matrix, Sn is the number of segments.
   * @return int The points number of the segment, which is the Sn above.
   */
  int cal_point(const Eigen::MatrixXd &Seg)
  {
    return Seg.rows();
  }

  /**
   * @brief Calculate the standard deviation of the segment.
   *
   * @param Seg The segment data matrix. It's an Sn*2 matrix, Sn is the number of segments.
   * @return double The standard deviation.
   */
  double cal_std(const Eigen::MatrixXd &Seg)
  {
    int n = cal_point(Seg);
    if (n < 2)
      return 0.0;

    Eigen::Vector2d m = Seg.colwise().mean();    // the means vector of data
    const auto &x = Seg.col(0);
    const auto &y = Seg.col(1);

    // ( (1/n-1) * ( sum( (x-x_mean)^2 )+ sum( (y-y_mean)^2 ) ) ^ (1/2)
    double sigma = std::sqrt((1 / (n - 1)) * ((x.array() - m(0)).square().sum() + (y.array() - m(1)).square().sum()));

    return sigma;
  }

  /**
   * @brief Calculate the width of the segment, which is the distance of first point and the last point.
   *
   * @param Seg The segment data matrix. It's an Sn*2 matrix, Sn is the number of segments.
   * @return double The width
   */
  double cal_width(const Eigen::MatrixXd &Seg)
  {
    // ( (x0 - x_last)^2 + (y0 - y_last)^2 )^(1/2)
    double width = std::sqrt(std::pow(Seg(0, 0) - Seg(Seg.rows() - 1, 0), 2) + std::pow(Seg(0, 1) - Seg(Seg.rows() - 1, 1), 2));
    return width;
  }

  /**
   * @brief Calculate the circularity and the radius of the segment.
   *
   * @param Seg The segment data matrix. It's an Sn*2 matrix, Sn is the number of segments.
   * @return std::tuple<double, double, double> { radius, circularity, distance }
   */
  std::tuple<double, double, double> cal_cr(const Eigen::MatrixXd &Seg)
  {
    const auto &x = Seg.col(0);
    const auto &y = Seg.col(1);

    Eigen::MatrixXd A(Seg.rows(), 3);
    Eigen::MatrixXd b(Seg.rows(), 1);

    A << -2 * x, -2 * y, Eigen::MatrixXd::Ones(Seg.rows(), 1);
    b << (-1 * x.array().square() - y.array().square());

    Eigen::MatrixXd x_p = A.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);
    auto xc = x_p(0);
    auto yc = x_p(1);

    double radius = std::sqrt(std::pow(xc, 2) + std::pow(yc, 2) - x_p(2));
    double circularity = ((radius - ((xc - x.array()).square() + (yc - y.array()).square()).sqrt()).square()).sum();
    double distance = std::sqrt(std::pow(xc, 2) + std::pow(yc, 2));

    return { radius, circularity, distance };
  }

  /**
   * @brief Calculates the linearity and bounding of the segment.
   *
   * @param Seg The segment data matrix. It's an Sn*2 matrix, Sn is the number of segments.
   * @return std::tuple<double, double, double, double> { bounding_box_long, bounding_box_short, bounding_box_area, total_least_square };
   */
  std::tuple<double, double, double, double> cal_linearity(Eigen::MatrixXd Seg)
  {
    if (Seg.rows() < 2)
      return { 0, 0, 0, 0 };

    Eigen::Vector2d m = Seg.colwise().mean();
    Seg.col(0) = Seg.col(0).array() - m(0);
    Seg.col(1) = Seg.col(1).array() - m(1);

    Eigen::BDCSVD<Eigen::MatrixXd> Seg_svd = Seg.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::MatrixXd U = Seg_svd.matrixU();
    Eigen::MatrixXd V = Seg_svd.matrixV();
    Eigen::MatrixXd A = Seg_svd.singularValues();

    Eigen::ArrayXd P_long = Seg * V.col(0);
    Eigen::ArrayXd P_short = Seg * V.col(1);

    double bounding_box_long = P_long.maxCoeff() - P_long.minCoeff();
    double bounding_box_short = P_short.maxCoeff() - P_short.minCoeff();
    double bounding_box_area = bounding_box_short * bounding_box_long;
    double total_least_square = P_short.square().mean();

    return { bounding_box_long, bounding_box_short, bounding_box_area, total_least_square };
  }

  /**
   * @brief Making the feature matrix by the segment data.
   *
   * @param Seg The segment data matrix. It's an Sn*2 matrix, Sn is the number of segments.
   * @return Eigen::VectorXd
   */
  Eigen::ArrayXd make_feature(const Eigen::MatrixXd &Seg)
  {
    Eigen::ArrayXd feature(FEATURE_NUM);
    feature(0) = cal_point(Seg);
    feature(1) = cal_std(Seg);
    feature(2) = cal_width(Seg);

    const auto [r, cir, distance] = cal_cr(Seg);
    feature(3) = r;
    feature(4) = cir;
    feature(5) = distance;

    const auto [bounding_box_long, bounding_box_short, bounding_box_area, total_least_square] = cal_linearity(Seg);
    feature(6) = bounding_box_long;
    feature(7) = bounding_box_short;
    feature(8) = bounding_box_area;
    feature(9) = total_least_square;

    return feature;
  }

  /**
   * @brief Transform the segemnt to the feature
   *
   * @param section_seg_vec The segment vector
   * @return Eigen::MatrixXd The feature data
   */
  Eigen::MatrixXd segment_to_feature(const std::vector<Eigen::MatrixXd> &section_seg_vec)
  {
    Eigen::MatrixXd feature_data(section_seg_vec.size(), FEATURE_NUM);
    for (int i{}; i < static_cast<int>(section_seg_vec.size()); ++i)
      feature_data.row(i) = make_feature(section_seg_vec[i]);

    return feature_data;
  }

  /**
   * @brief Transform the section xy data to the feature matrix.
   *
   * @param xy_data The section xy data. On my minibots, the matrix is 720*2
   * @return std::pair<Eigen::MatrixXd, std::vector<Eigen::MatrixXd>> feature matrix and a vector containing all the segments in one second.
   */
  std::pair<Eigen::MatrixXd, std::vector<Eigen::MatrixXd>> section_to_feature(const Eigen::MatrixXd &xy_data)    // xy_data default is 720*2
  {
    std::vector<Eigen::MatrixXd> section_seg_vec = metric::section_to_segment(xy_data);    // The all segments in the seconds.
    Eigen::MatrixXd feature_data = MakeFeatures::segment_to_feature(section_seg_vec);

    return { feature_data, section_seg_vec };
  }

}    // namespace MakeFeatures