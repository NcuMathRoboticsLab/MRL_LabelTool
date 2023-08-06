/**
 * @file file_handler.cpp
 * @author Mes (mes900903@gmail.com) (Discord: Mes#0903)
 * @brief Load the file and store weights to file, any functions related to file operations should be here.
 * @version 0.1
 * @date 2022-12-15
 */

#include "file_handler.h"
#include "normalize.h"
#include "make_feature.h"
#include "metric.h"
#include "Eigen/Eigen"

#include <filesystem>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <unordered_set>

#if __cplusplus >= 202002L
#include <string_view>
#include <ranges>
#else
#include <type_traits>
#endif

#define CLEAN_STREAM \
  stream.str("");    \
  stream.clear()

namespace LoadMatrix {
  /**
   * @brief Read the data from the filepath to the matrix.
   *
   * @param filepath The file which would be loaded to matrix.
   * @param ROWS The lines number of the file.
   * @param COLS The cols number of the file.
   * @return Eigen::MatrixXd The matrix which have completed loading.
   */
  Eigen::MatrixXd readDataSet(const std::string filepath, const int ROWS, const int COLS)
  {
    std::ifstream infile(filepath);
    if (infile.fail()) {
      std::cerr << "cant found " << filepath << '\n';
      std::cin.get();
      exit(1);
    }

    Eigen::MatrixXd result(ROWS, COLS);
    std::string line;
    std::stringstream stream;
    int row = 0;
    for (int cnt = 0; cnt < ROWS; ++cnt) {
      double buff;
      getline(infile, line);    // read every line of the file
      stream << line;
      for (int col = 0; col < COLS; ++col) {
        stream >> buff;
        result(row, col) = buff;
      }
      ++row;

      CLEAN_STREAM;
    }

    infile.close();

    return result;
  };

  /**
   * @brief Read the Labeling data from the file and load it to the matrix
   *
   * @param filepath The file which would be loaded to matrix.
   * @param SIZE The lines number of the file.
   * @return Eigen::VectorXd  The vector which have completed loading.
   */
  Eigen::VectorXd readLabel(const std::string filepath, const int SIZE)
  {
    std::ifstream infile(filepath);
    if (infile.fail()) {
      std::cerr << "cant found " << filepath << '\n';
      std::cin.get();
      exit(1);
    }

    Eigen::VectorXd result(SIZE);
    std::string line;
    std::stringstream stream;

    int row = 0;
    for (int cnt = 0; cnt < SIZE; ++cnt) {
      double buff;
      getline(infile, line);    // read every line of the file
      stream << line;
      stream >> buff;
      result(row) = buff;
      ++row;

      CLEAN_STREAM;
    }

    infile.close();
    return result;
  }
}    // namespace LoadMatrix


namespace FileHandler {

  /**
   * @brief Return the project directory path.
   *
   * @return std::string The project directory path
   */
  std::string get_MRL_project_root()
  {
    namespace fs = std::filesystem;

    fs::path current = fs::current_path();

    while (current.filename().string() != "MRL_LabelTool")
      current = current.parent_path();

    return current.string();
  }
}    // namespace FileHandler