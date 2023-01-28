/**
 * @file normalize.cpp
 * @author Mes (mes900903@gmail.com) (Discord: Mes#0903)
 * @brief This is the implementation of class Normalizer
 * @version 0.1
 * @date 2022-11-17
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "normalize.h"
#include "Eigen/Eigen"

#include <iostream>
#include <fstream>

#define CLEAN_STREAM \
  stream.str("");    \
  stream.clear()

/**
 * @brief Calculate the data_min and data_mm in Normalizer,
 *        which is the minimum num and max-min num of each column.
 *
 * @param data The matrix will determine the data_min and data_mm, which means the scale of normalization.
 */
void Normalizer::fit(const Eigen::MatrixXd &data)
{
  const int COLS = data.cols();
  data_min = Eigen::VectorXd::Zero(COLS);
  data_mm = Eigen::VectorXd::Zero(COLS);

  for (int i = 0; i < COLS; ++i) {
    data_min(i) = data.col(i).minCoeff();
    data_mm(i) = data.col(i).maxCoeff() - data_min(i);    // max - min
    if (data_mm(i) == 0)    // if max-min is 0, it can't be division, thus assign it to 1
      data_mm(i) = 1;
  }
}

/**
 * @brief Do normalization for every column of the data.
 *
 * @param data The matrix will be normalized by min-max normalization.
 * @return Eigen::MatrixXd The result of the normalization matrix.
 */
Eigen::MatrixXd Normalizer::transform(const Eigen::MatrixXd &data)
{
  Eigen::MatrixXd tf_matrix(data.rows(), data.cols());

  const int COLS = data.cols();
  for (int i = 0; i < COLS; ++i)
    tf_matrix.col(i) = (data.col(i).array() - data_min(i)) / data_mm(i);

  return tf_matrix;
}

/**
 * @brief Store the scale of the normalization matrix.
 *
 * @param filepath For debug using, maybe unused.
 * @param outfile The file will store the result of the normalization.
 */
void Normalizer::store_weight([[maybe_unused]] const std::string &filepath, std::ofstream &outfile)
{
  puts("Storing Normalizer data...");

  outfile << data_min.size() << ' ' << data_mm.size() << '\n';
  for (int i = 0; i < data_min.size(); ++i)
    outfile << data_min(i) << " \n"[i == data_min.size() - 1];    // if the element is the last element, store '\n', otherwise a space ' '

  for (int i = 0; i < data_mm.size(); ++i)
    outfile << data_mm(i) << " \n"[i == data_mm.size() - 1];    // if the element is the last element, store '\n', otherwise a space ' '

  std::cout << "Successfly stored normalizer!\n";
}

/**
 * @brief Load the scale of the normalization matrix.
 *
 * @param filepath For debug using, maybe unused.
 * @param infile The file will store the result of the normalization.
 */
void Normalizer::load_weight([[maybe_unused]] const std::string &filepath, std::ifstream &infile)
{
  std::string line;
  std::stringstream stream;
  int min_size, mm_size;
  getline(infile, line);
  stream << line;
  stream >> min_size >> mm_size;
  CLEAN_STREAM;

  data_min = Eigen::VectorXd::Zero(min_size);    // resize
  data_mm = Eigen::VectorXd::Zero(mm_size);    // resize
  getline(infile, line);
  stream << line;
  for (int i = 0; i < min_size; ++i)
    stream >> data_min(i);

  CLEAN_STREAM;

  getline(infile, line);
  stream << line;
  for (int i = 0; i < mm_size; ++i)
    stream >> data_mm(i);
}