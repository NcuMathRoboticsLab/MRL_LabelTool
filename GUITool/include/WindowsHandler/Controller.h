/**
 * @file Controller.cpp
 * @author Mes (mes900903@gmail.com) (Discord: Mes#0903)
 * @brief The base of window controller
 * @version 0.1
 * @date 2023-01-24
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef CONTROLLER_H__
#define CONTROLLER_H__

#include "Eigen/Eigen"

#include <chrono>
#include <string>
#include <fstream>
#include <vector>

class AnimationController {
public:
  void transform_frame();
  void read_frame();

  virtual void check_auto_play();
  virtual void check_update_frame() = 0;

  AnimationController();

public:
  int fps;
  int HZ;
  int frame, max_frame;
  int window_size;

  bool update_frame;

  bool auto_play;
  bool replay;

  Eigen::MatrixXd xy_data;
  Eigen::MatrixXd feature_matrix;
  std::vector<Eigen::MatrixXd> segment_vec;

  std::string raw_data_path;

protected:
  bool is_xydata;

  std::chrono::system_clock::time_point current_time__;
  std::string raw_bin_path__;
  std::ifstream raw_bin_file__;
  bool raw_bin_open__;
};

#endif