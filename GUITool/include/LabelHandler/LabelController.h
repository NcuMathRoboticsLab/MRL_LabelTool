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

#include "Controller.h"
#include "Eigen/Eigen"

#include <vector>

class LabelController : public AnimationController {
public:
  void output_feature_data();
  void output_label_data();

  void check_clean_data();
  void check_load_data();
  void check_update_frame() override;
  void check_save_data();

  LabelController();
  ~LabelController();

private:
  void _write_bin_feature_data(const int feature_index, const Eigen::MatrixXd &feature_matrix);
  void _write_bin_feature_num_data(const int nums);
  void _write_bin_label_data(const int label_index, const std::vector<int> &segment_label);
  void _write_bin_label_num_data(const int nums);

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
  std::vector<std::vector<Eigen::MatrixXd>> total_frame_segment_vec;

private:
  std::string _tool_data_path;
  std::string _feature_bin_path;
  std::string _feature_num_bin_path;
  std::string _label_bin_path;
  std::string _label_num_bin_path;
  std::string _buf_feature_path;
  std::string _buf_label_path;

  std::fstream _feature_bin_file;
  std::fstream _feature_num_bin_file;
  std::fstream _label_bin_file;
  std::fstream _label_num_bin_file;
};

#endif