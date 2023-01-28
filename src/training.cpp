/**
 * @file training_ball.cpp
 * @author Mes (mes900903@gmail.com) (Discord: Mes#0903)
 * @brief Traning the Adaboost to classified if an object is an ball, then stored the weighting.
 *        Execute it by command `rosrun mes_detect_ball Training_Ball` if you use ROS to build it.
 * @version 0.1
 * @date 2022-11-17
 */

#include "adaboost.h"
#include "logistic.h"
#include "make_feature.h"
#include "normalize.h"
#include "file_handler.h"
#include "metric.h"
#include "Eigen/Dense"

#include <iostream>
#include <limits>

int main([[maybe_unused]] int argc, char **argv)
{
  const std::string filepath = FileHandler::get_filepath();

  int case_num = 0;
  int sample = 0;

  std::cout << "Input 1 if training, others if loading\n>";
  std::cin >> case_num;

  if (case_num == 1) {
    std::cout << "input sample numbers\n>";
    std::cin >> sample;
  }
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  if (case_num == 1) {
    /* fitting */
    puts("read training data...");
    Eigen::MatrixXd train_X = LoadMatrix::readDataSet(filepath + "/include/dataset/ball_train_x.txt", 17000, FEATURE_NUM);    // file, row, col

    puts("reading training label...");
    Eigen::VectorXd train_Y = LoadMatrix::readLabel(filepath + "/include/dataset/ball_train_y.txt", 17000);    // file, segment num(row)

    puts("reading testing data...");
    Eigen::MatrixXd test_X = LoadMatrix::readDataSet(filepath + "/include/dataset/ball_test_x.txt", 19133, FEATURE_NUM);

    puts("reading testing label...");
    Eigen::VectorXd test_Y = LoadMatrix::readLabel(filepath + "/include/dataset/ball_test_y.txt", 19133);

    Normalizer normalizer;
    normalizer.fit(train_X);
    train_X = normalizer.transform(train_X);
    test_X = normalizer.transform(test_X);

    for (int i = 0; i < sample; ++i) {
      std::cout << "training sample " << i + 1 << "...\n";
      puts("start tranning");
      Adaboost<logistic> A(100);
      A.fit(train_X, train_Y);

      // prediction
      puts("make prediction");
      Eigen::VectorXd pred_Y = A.predict(test_X);

      puts("cal confusion matrix");
      Eigen::MatrixXd confusion_matrix = metric::cal_confusion_matrix(test_Y, pred_Y);
      A.set_confusion_matrix(confusion_matrix);
      A.print_confusion_matrix();

      FileHandler::store_weight(confusion_matrix, filepath + "/include/weight_data/adaboost_ball_weight.txt", A, normalizer);
    }
  }
  else {
    puts("reading testing data...");
    Eigen::MatrixXd test_X = LoadMatrix::readDataSet(filepath + "/include/dataset/ball_test_x.txt", 19133, FEATURE_NUM);

    puts("reading testing label...");
    Eigen::VectorXd test_Y = LoadMatrix::readLabel(filepath + "/include/dataset/ball_test_y.txt", 19133);

    Normalizer normalizer;
    Adaboost<logistic> A;

    puts("Load Weighting...");
    FileHandler::load_weight(filepath + "/include/weight_data/adaboost_ball_weight.txt", A, normalizer);

    puts("Transforming test data...");
    test_X = normalizer.transform(test_X);

    puts("make prediction");
    Eigen::VectorXd pred_Y = A.predict(test_X);

    puts("cal confusion matrix");
    A.set_confusion_matrix(metric::cal_confusion_matrix(test_Y, pred_Y));
    A.print_confusion_matrix();
  }
}