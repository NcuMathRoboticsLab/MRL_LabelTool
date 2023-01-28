/**
 * @file LabelController.h
 * @author Mes (mes900903@gmail.com) (Discord: Mes#0903)
 * @brief The declaration of Label Controller
 * @version 0.1
 * @date 2023-01-24
 *
 * @copyright Copyright (c) 2023 Mes
 *
 */

#ifndef LABEL_CONTROLLER_H__
#define LABEL_CONTROLLER_H__

#include "Eigen/Eigen"
#include "Controller.h"

#include <vector>

struct LabelController : public AnimationController {
public:
  void write_bin_feature_data__(const int feature_index, const Eigen::MatrixXd &feature_matrix);
  void write_bin_feature_num_data__(const int nums);
  void write_bin_label_data__(const int label_index, const std::vector<int> &segment_label);
  void write_bin_label_num_data__(const int nums);

  void output_feature_data();
  void output_label_data();

  void check_clean_data();
  void check_load_data();
  void check_update_frame() override;
  void check_save_data();

  LabelController();
  ~LabelController();

public:
  bool save_label;
  bool enable_enter_save;
  bool output_txt;
  bool show_rect;
  bool show_nearest;
  bool auto_label;
  bool clean_data;
  bool load_data;

  int writed_max_frame;
  int writed_frame_numbers;

  float label_mouse_area;

  int current_save_frame;
  std::chrono::system_clock::time_point current_save_time;

  std::string feature_output_path;
  std::string label_output_path;
  std::string tmp_feature_filepath;
  std::string tmp_label_filepath;

  std::vector<int> segment_label;
  std::vector<int> label_size_vec;
  std::vector<int> label_index_vec;
  std::vector<int> feature_size_vec;
  std::vector<int> feature_index_vec;

private:
  std::string tool_data_path__;
  std::string feature_bin_path__;
  std::string feature_num_bin_path__;
  std::string label_bin_path__;
  std::string label_num_bin_path__;
  std::string buf_feature_path__;
  std::string buf_label_path__;

  std::fstream feature_bin_file__;
  std::fstream feature_num_bin_file__;
  std::fstream label_bin_file__;
  std::fstream label_num_bin_file__;
};

#endif