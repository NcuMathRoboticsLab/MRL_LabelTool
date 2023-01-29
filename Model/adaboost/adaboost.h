#ifndef ADABOOST_CLASSIFIER__
#define ADABOOST_CLASSIFIER__

/**
 * @file adaboost_classifier.h
 * @author Mes (mes900903@gmail.com) (Discord: Mes#0903)
 * @brief The declaration of Adaboost class
 * @version 0.1
 * @date 2022-11-17
 */

#include "normalize.h"
#include "Eigen/Eigen"

#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>


#if __cplusplus >= 202002L

/**
 * @brief Check if the weight in class can be stored.
 * @param ins The instance of the class.
 */
template <typename Model>
concept has_fit = requires(Model ins, const Eigen::MatrixXd &train_X, const Eigen::VectorXd &train_Y, int Iterations, const Eigen::MatrixXd &train_weight)
{
  {
    ins.store_weight(filepath, outfile)
    } -> void;    // ok WTF clang-format...
};

/**
 * @param ins The instance of the class.
 */
template <typename Model>
concept has_predict = requires(Model ins, const std::string &filepath, std::ifstream &infile)
{
  {
    ins.load_weight(filepath, infile)
    } -> void;
};

template <typename Module>
concept valid_Model = has_fir<Module> && has_predict<Module>;

#else


/**
 * @brief Check if the class has `fit` function
 */
template <typename, typename = void>
struct has_fit : std::false_type {
};

template <typename Model>
struct has_fit<Model, std::void_t<decltype(&Model::fit)> >
    : std::is_invocable_r<std::tuple<Eigen::VectorXd, double, bool>,
                          decltype(&Model::fit),
                          Model &,
                          const Eigen::MatrixXd &,
                          const Eigen::VectorXd &,
                          const Eigen::MatrixXd &,
                          int> {
};

/**
 * @brief Check if the class has `predict` function.
 */
template <typename, typename = void>
struct has_predict : std::false_type {
};

template <typename Model>
struct has_predict<Model, std::void_t<decltype(&Model::predict)> >
    : std::is_invocable_r<Eigen::VectorXd,
                          decltype(&Model::predict),
                          Model &,
                          const Eigen::MatrixXd &> {
};

template <typename Model>
struct valid_Model : std::conjunction<has_fit<Model>, has_predict<Model> > {
};

template <typename Model>
constexpr bool valid_Model_v = valid_Model<Model>::value;

#endif


/**
 * @brief The Adaboost class, have M weak classfiers, each weak classfiers is a logistic regression classifier.
 */
#if __cplusplus >= 202002L
template <valid_Model Model>
#else
template <typename Model>
#endif
class Adaboost {
#if __cplusplus < 202002L
  static_assert(valid_Model_v<Model>, "The Model didn't fit the requires of predict function or fit function");
#endif

public:
  int TN{}, TP{}, FN{}, FP{};
  int M = 0;    // the number of weak classfiers
  Eigen::VectorXd alpha;    // the vector of weights for weak classfiers

  std::vector<Model> vec;    // the vector of weak classfiers

public:
  Adaboost() = default;
  Adaboost(const int M)
      : M{ M }, vec(M) { alpha = Eigen::VectorXd::Zero(M); }

  void fit(const Eigen::MatrixXd &train_X, const Eigen::VectorXd &train_Y);    // training Adaboost
  Eigen::VectorXd predict(const Eigen::MatrixXd &test_X);    // make prediction

public:
  void store_weight(const std::string &filepath, std::ofstream &outfile);    // store the weights of Adaboost
  void load_weight(const std::string &filepath, std::ifstream &infile);    // load the weights before stored.
  void set_confusion_matrix(const Eigen::MatrixXd &confusion_matrix);    // set the confusion matrix of the Adaboost
  void print_confusion_matrix();    // print the confusion matrix of the Adaboost
};

/**
 * @brief Training Adaboost.
 *
 * @param train_X The training data, which is a feature matrix.
 * @param train_Y The training label.
 */
template <typename Model>
void Adaboost<Model>::fit(const Eigen::MatrixXd &train_X, const Eigen::VectorXd &train_Y)
{
  Eigen::VectorXd w = Eigen::VectorXd::Ones(train_X.rows());

  for (int i = 0; i < M; ++i) {
    std::cout << "Training Weak Learner: " << i + 1 << '\n';
    w /= w.sum();

    const auto [pred_Y, err, all_correct] = vec[i].fit(train_X, train_Y, w, 1000);    // pred_Y is the label it predict, err is the error rate.

    // if the accuracy is 100%, we can delete all the other weak learner in adaboost, just use this weak learner to judge data.
    if (all_correct) {
      auto tmp = vec[i];
      vec.clear();
      vec.push_back(std::move(tmp));
      M = 1;
      alpha = Eigen::VectorXd::Ones(M);
      break;
    }

    alpha(i) = std::log((1 + err) / (1 - err)) / 2;
    for (int r = 0; r < train_Y.size(); ++r) {
      if (train_Y(r) != pred_Y(r))
        w(r) *= std::exp(alpha(i));
      else
        w(r) *= std::exp(-alpha(i));
    }
  }
}

/**
 * @brief Make the prediction of the data.
 *
 * @param data The data need to be predicted, which is a feature matrix.
 * @return Eigen::VectorXd The output label vector.
 */
template <typename Model>
Eigen::VectorXd Adaboost<Model>::predict(const Eigen::MatrixXd &data)
{
  int R = data.rows();
  Eigen::ArrayXd C = Eigen::ArrayXd::Zero(R);
  for (int m = 0; m < M; ++m)
    C += alpha(m) * (2 * vec[m].get_label(data).array() - 1);

  return C.unaryExpr([](double x) { return double(x > 0); });
}

/**
 * @brief Store the weight vector of all weak learner in Adaboost.
 *
 * @param filepath For Debug using, maybe unused. The file path, where to store the weight.
 * @param outfile The file, where to store the weight, provided by the file handler.
 */
template <typename Model>
void Adaboost<Model>::store_weight([[maybe_unused]] const std::string &filepath, std::ofstream &outfile)
{
  double recall = static_cast<double>(TP) / (TP + FN);
  double precision = static_cast<double>(TP) / (TP + FP);
  double F1_Score = 2 * precision * recall / (precision + recall);

  outfile << F1_Score << ' ' << TN << ' ' << TP << ' ' << FN << ' ' << FP << '\n';
  outfile << M << '\n';
  for (int i = 0; i < M; ++i)
    outfile << alpha(i) << " \n"[i == M - 1];

  for (int i = 0; i < M; ++i)
    vec[i].store_weight(outfile);

  std::cout << "Successfly stored Adaboost weighting!\n";
}

/**
 * @brief Store the weight vector of all weak learner in Adaboost.
 *
 * @param filepath For Debug using, maybe unused. The file path, where to load the weight.
 * @param outfile The file, where to load the weight, provided by the file handler.
 */
template <typename Model>
void Adaboost<Model>::load_weight([[maybe_unused]] const std::string &filepath, std::ifstream &infile)
{
  std::string line;
  std::stringstream stream;
  double F1_Score;

  getline(infile, line);
  stream << line;
  stream >> F1_Score >> TN >> TP >> FN >> FP;
  stream.str("");
  stream.clear();

  getline(infile, line);
  stream << line;
  stream >> M;
  stream.str("");
  stream.clear();

  alpha = Eigen::VectorXd::Zero(M);
  getline(infile, line);
  stream << line;
  for (int i = 0; i < M; ++i)
    stream >> alpha(i);
  stream.str("");
  stream.clear();

  vec.resize(M);
  for (int i = 0; i < M; ++i) {
    vec[i].load_weight(infile, stream);
    stream.str("");
    stream.clear();
  }
}

/**
 * @brief Set the confusion matrix of the Adaboost.
 *
 * @param confusion_matrix The confusion matrix.
 */
template <typename Model>
void Adaboost<Model>::set_confusion_matrix(const Eigen::MatrixXd &confusion_matrix)
{
  TP = static_cast<int>(confusion_matrix(0, 0));
  FP = static_cast<int>(confusion_matrix(0, 1));
  FN = static_cast<int>(confusion_matrix(1, 0));
  TN = static_cast<int>(confusion_matrix(1, 1));
}

/**
 * @brief Print the confusion table.
 */
template <typename Model>
void Adaboost<Model>::print_confusion_matrix()
{
  Eigen::MatrixXd confusion_matrix(2, 2);
  confusion_matrix << TP, FP, FN, TN;

  std::cout << "The confusion matrix of the Adaboost is : \n"
            << confusion_matrix << '\n';
}

#endif