/**
 * @file logistic.cpp
 * @author Mes (mes900903@gmail.com) (Discord: Mes#0903)
 * @brief The implementation of the weak learner class in Adaboost, I use the logistic as the weak learner.
 * @version 0.1
 * @date 2022-11-17
 */

#include "logistic.h"

#include <Eigen/Eigen>
#include <vector>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <random>
#include <fstream>
#include <sstream>

constexpr uint32_t FEATURE_NUM = 10;

/**
 * @brief Training the weight in weak learner
 *
 * @param train_X The training data, which is a feature matrix.
 * @param train_Y The training label.
 * @param train_weight The training weight in adaboost.
 * @param Iterations The training iterations.
 * @return std::tuple<Eigen::VectorXd, double, bool> The first element of the pair is the label it predict,
 *                                            the second one is the error rate,
 *                                            the third one is a flag for 100% accuracy, if the accuracy is 100%, we can delete all the other weak learner in adaboost.
 */
std::tuple<Eigen::VectorXd, double, bool>
logistic::fit(const Eigen::MatrixXd &train_X, const Eigen::VectorXd &train_Y, const Eigen::MatrixXd &train_weight, uint32_t Iterations)
{
  uint32_t D = FEATURE_NUM;    // dimention is the column of training data, which is 5 in my case, since there is 5 features.

  // use random initialize weight generated by normal distribution
  static std::random_device rd;
  static std::default_random_engine gen(rd());
  std::normal_distribution<> dis(0, std::sqrt(D + 1));
  w = Eigen::VectorXd::NullaryExpr(D, [&]() { return dis(gen); });
  w0 = dis(gen);

  Eigen::VectorXd w_momentum = Eigen::VectorXd::Zero(D);
  double w0_momentum = 0.0;
  double alpha = 0.1;

  for (uint32_t i = 0; i < Iterations; ++i) {
    double lr = alpha / (1 + i / 10);

    Eigen::ArrayXd hx = (train_X * w).array() + w0;
    hx = cal_logistic(hx);
    Eigen::VectorXd tmp = train_weight.array() * (train_Y.array() - hx);

    Eigen::VectorXd w_grad = train_X.transpose() * tmp;
    double w0_grad = tmp.sum();

    w_momentum = (w_momentum + lr * w_grad) * 0.9;
    w0_momentum = (w0_momentum + lr * w0_grad) * 0.9;
    w += w_momentum + lr * w_grad;
    w0 += w0_momentum + lr * w0_grad;
  }

  Eigen::VectorXd pred_Y = get_label(train_X);

  double err = 0.0;
  bool all_correct = true;
  for (int i = 0; i < pred_Y.size(); ++i) {
    if (pred_Y(i) != train_Y(i)) {
      all_correct = false;
      err -= train_weight(i);
    }
    else {
      err += train_weight(i);
    }
  }

  return { pred_Y, err, all_correct };
}

/**
 * @brief Calculate the logistic function 1 / (1 + e^(-x)), which is equivalent to (tanh(x / 2) + 1)/2.
 *
 * @param x The input array.
 * @return Eigen::ArrayXd
 */
Eigen::ArrayXd logistic::cal_logistic(const Eigen::ArrayXd &x)
{
  return ((x / 2).tanh() + 1) / 2;
}

/**
 * @brief Make prediction of the data, this function is for debug using, it's output is not an label but an probability.
 *
 * @param data The feature matrix of all section, the size is Sn*5, Sn is the total number of the data, 5 means the number of the feature.
 * @return Eigen::VectorXd The probability of the data get from the logistic function.
 */
Eigen::VectorXd logistic::predict(const Eigen::MatrixXd &data)
{
  Eigen::ArrayXd hx = (data * w).array() + w0;

  return cal_logistic(hx);
}

/**
 * @brief Predict the label of the data.
 *
 * @param data The feature matrix of all section, the size is Sn*5, Sn is the total number of the data, 5 means the number of the feature.
 * @return Eigen::VectorXd The label of the data. If the probability get from the logistic function >= 0.5, output 1, otherwise 0.
 */
Eigen::VectorXd logistic::get_label(const Eigen::MatrixXd &data)
{
  Eigen::ArrayXd hx = (data * w).array() + w0;

  return cal_logistic(hx).round();
}

/**
 * @brief Store the weight of the weak learner.
 *
 * @param outfile The file path, where to store the weight.
 */
void logistic::store_weight(std::ofstream &outfile)
{
  uint32_t N = FEATURE_NUM;

  outfile << w0 << '\n';
  for (uint32_t i = 0; i < N; ++i)
    outfile << w(i) << " \n"[i == N - 1];
}

/**
 * @brief Load the weight of the weak learner.
 *
 * @param infile The file path, where to load the weight.
 * @param stream For avoiding copying std::stringstream, pass it by reference into function.
 */
void logistic::load_weight(std::ifstream &infile, std::stringstream &stream)
{
  int N = FEATURE_NUM;    // The number of weighting
  w = Eigen::VectorXd::Zero(N);
  std::string line;

  getline(infile, line);
  stream << line;
  stream >> w0;
  stream.str("");
  stream.clear();

  getline(infile, line);
  stream << line;
  for (int i = 0; i < N; ++i) {
    double buff;
    stream >> buff;
    w(i) = buff;
  }
}