#ifndef FILE_HANDLER_H__
#define FILE_HANDLER_H__

/**
 * @file FileHandler.h
 * @author Mes (mes900903@gmail.com) (Discord: Mes#0903)
 * @brief Load the file and store weights to file, any functions related to file operations should be here.
 * @version 0.1
 * @date 2022-11-18
 */

#include "Eigen/Eigen"

#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>


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
  Eigen::MatrixXd readDataSet(const std::string filepath, const int ROWS, const int COLS);

  /**
   * @brief Read the Labeling data from the file and load it to the matrix
   *
   * @param filepath The file which would be loaded to matrix.
   * @param SIZE The lines number of the file.
   * @return Eigen::VectorXd  The vector which have completed loading.
   */
  Eigen::VectorXd readLabel(const std::string filepath, const int SIZE);

}    // namespace LoadMatrix

namespace FileHandler {
  /**
   * @brief Return the project directory path.
   *
   * @param filepath the executable file path, which is argv[0].
   * @return std::string The project directory path
   */
  std::string get_MRL_project_root();

  namespace detail {
#if __cplusplus >= 202002L
    /**
     * @brief Check if the weight in class can be stored.
     * @param ins The instance of the class.
     */
    template <typename T>
    concept can_store = requires(T &ins, std::ofstream &outfile) {
      {
        ins.store_weight(outfile)
      } -> std::same_as<void>;    // ok WTF clang-format...
    };

    /**
     * @param ins The instance of the class.
     */
    template <typename T>
    concept can_load = requires(T &ins, std::ifstream &infile) {
      {
        ins.load_weight(infile)
      } -> std::same_as<void>;
    };

    /**
     * @brief The implementation for loading data.
     *
     * @param filepath The file which would be written.
     * @param ins The class instance.
     * @param first The head of parameter pack, a class instance.
     */
    template <typename T>
    void store_weight_impl(std::ofstream &outfile, T &ins)
      requires can_store<T>    // Check if the instance implemented the `store_weight` method by Detection Idioms(Concept requires)
    {
      ins.store_weight(outfile);
    }

    /**
     * @brief The implementation for storing data.
     *
     * @param filepath The file which would be stored.
     * @param ins The class instance.
     * @param first The head of parameter pack, a class instance.
     */
    template <typename T>
    void load_weight_impl(std::ifstream &infile, T &ins)
      requires can_load<T>    // Check if the instance implemented the `load_weight` method by Detection Idioms(Concept requires)
    {
      ins.load_weight(infile);
    }

#else

    /**
     * @brief Check if the weight in class can be stored.
     */
    template <typename, typename = void>
    struct can_store : std::false_type {};

    template <typename T>
    struct can_store<T, std::void_t<decltype(&T::store_weight)> >
        : std::is_invocable_r<void,
                              decltype(&T::store_weight),
                              T &,
                              std::ofstream &> {};


    /**
     * @brief Check if the weight in class can be loaded.
     */
    template <typename, typename = void>
    struct can_load : std::false_type {};

    template <typename T>
    struct can_load<T, std::void_t<decltype(&T::load_weight)> >
        : std::is_invocable_r<void,
                              decltype(&T::load_weight),
                              T &,
                              std::ifstream &> {};


    /**
     * @brief The implementation for loading data.
     *
     * @param filepath The file which would be written.
     * @param outfile The file stream which will be written.
     * @param ins The class instance.
     */
    template <typename T,
              typename std::enable_if<can_store<T>::value>::type * = nullptr>
    void store_weight_impl(std::ofstream &outfile, T &ins)
    {
      ins.store_weight(outfile);
    }

    /**
     * @brief The implementation for storing data.
     *
     * @param filepath The file which would be stored.
     * @param outfile The file stream which will be stored.
     * @param ins The class instance.
     */
    template <typename T,
              typename std::enable_if<can_load<T>::value>::type * = nullptr>
    void load_weight_impl(std::ifstream &infile, T &ins)
    {
      ins.load_weight(infile);
    }

#endif

    /**
     * @brief Calling the implementation of the class instance
     *
     * @param filepath The file which would be stored.
     * @param outfile The file stream which will be stored.
     * @param first The target class instance.
     * @param instances Class instances pack.
     */
    template <typename T, typename... A>
    void store_weight_impl(std::ofstream &outfile, T &first, A &...instances)
    {
      store_weight_impl(outfile, first);
      store_weight_impl(outfile, instances...);
    }

    template <typename T, typename... A>
    void load_weight_impl(std::ifstream &infile, T &first, A &...instances)
    {
      load_weight_impl(infile, first);
      load_weight_impl(infile, instances...);
    }
  }    // namespace detail

  /**
   * @brief the API for storing data
   *
   * @param filepath the file which would be stored
   * @param instances the parameter pack, class instances
   */
  template <typename... T>
  void store_weight(const Eigen::MatrixXd &confusion, const std::string filepath, T &...instances)
  {
    // compact the correct rate between storing file and current training data
    std::ifstream infile(filepath);
    if (infile.fail()) {
      std::cerr << "cant read " << filepath << '\n';
      std::cin.get();
      exit(1);
    }

    std::string line;
    std::stringstream stream;
    getline(infile, line);

    double old_F1_Score;
    int store_TN, store_TP, store_FN, store_FP;
    stream << line;
    stream >> old_F1_Score >> store_TN >> store_TP >> store_FN >> store_FP;
    stream.str("");
    stream.clear();
    infile.close();

    // compact the correct rate between storing file and current training data
    int TP = static_cast<int>(confusion(0, 0)), FP = static_cast<int>(confusion(0, 1)), FN = static_cast<int>(confusion(1, 0)), TN = static_cast<int>(confusion(1, 1));

    double accuracy = static_cast<double>(TP + TN) / (TP + TN + FP + FN);
    double recall = static_cast<double>(TP) / (TP + FN);
    double precision = static_cast<double>(TP) / (TP + FP);
    double F1_Score = 2 * precision * recall / (precision + recall);

    std::cout << "Calculated Accuracy : " << accuracy << '\n'
              << "Calculated recall : " << recall << '\n'
              << "Calculated precision: " << precision << '\n'
              << "Calculated F1 Score: " << F1_Score << '\n'
              << "Best F1 Score: " << old_F1_Score << '\n';

    // compact F1 Score
    if (F1_Score <= old_F1_Score) {
      std::cout << "This weight won't be saved since its F1 Score is not better than the original one\n";
      return;
    }

    std::ofstream outfile(filepath);
    if (outfile.fail()) {
      std::cerr << "cant found " << filepath << '\n';
      std::cin.get();
      exit(1);
    }

    detail::store_weight_impl(outfile, instances...);

    outfile.close();
  }

  /**
   * @brief the API for loading data
   *
   * @param filepath the file which would be stored
   * @param instances the parameter pack, class instances
   */
  template <typename... T>
  void load_weight(const std::string filepath, T &...instances)
  {
    std::ifstream infile(filepath);
    if (infile.fail()) {
      std::cerr << "cant found " << filepath << '\n';
      std::cin.get();
      exit(1);
    }

    detail::load_weight_impl(infile, instances...);

    infile.close();
  }
}    // namespace FileHandler

#endif