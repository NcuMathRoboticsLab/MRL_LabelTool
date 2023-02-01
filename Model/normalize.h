#ifndef NORMALIZER__
#define NORMALIZER__

/**
 * @file normalize.h
 * @author Mes (mes900903@gmail.com) (Discord: Mes#0903)
 * @brief Normalizer, which can normalize the data by min-max normalization.
 *        min-max normalization: (x - x_min) / (x_max - x_min)
 * @version 0.1
 * @date 2022-11-17
 */

#include "Eigen/Eigen"

class Normalizer {
public:
  Eigen::VectorXd data_min;    // the minumum num of each column, my feature matrix have 5 column, thus the size of data_min is 5
  Eigen::VectorXd data_mm;    // the max-min num of each column, my feature matrix have 5 column, thus the size of data_mm is 5

public:
  void fit(const Eigen::MatrixXd &data);    // calculate the data_min and data_mm
  Eigen::MatrixXd transform(const Eigen::MatrixXd &data);    // do normalization for every column of the data
  void store_weight(std::ofstream &outfile);    // store the scale of the normalization
  void load_weight(std::ifstream &infile);    // load the scale of the normalization
};

#endif