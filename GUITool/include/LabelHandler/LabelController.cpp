/**
 * @file LabelController.cpp
 * @author Mes (mes900903@gmail.com) (Discord: Mes#0903)
 * @brief The implementation of the Label Controller
 * @version 0.1
 * @date 2023-01-24
 *
 * @copyright Copyright (c) 2023 Mes
 *
 */

#include "LabelController.h"
#include "make_feature.h"
#include "file_handler.h"
#include "json.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

/**
 * @brief Write the feature binary file. (matrix -> binary file)
 *
 * @param feature_index the index would be written
 * @param feature_matrix  the matrix would be written to the file.
 */
void LabelController::_write_bin_feature_data(const int feature_index, const Eigen::MatrixXd &feature_matrix)
{
  _feature_bin_file.seekp(feature_index, std::ios::beg);
  for (const auto &row : feature_matrix.rowwise()) {
    for (const double feature : row)
      _feature_bin_file.write(reinterpret_cast<const char *>(&feature), sizeof(double));
  }
  _feature_bin_file.flush();
}

/**
 * @brief Record the numbers of features.
 *
 * @param nums
 */
void LabelController::_write_bin_feature_num_data(const int nums)
{
  _feature_num_bin_file.seekp(frame * sizeof(double), std::ios::beg);
  _feature_num_bin_file.write(reinterpret_cast<const char *>(&nums), sizeof(double));
  _feature_num_bin_file.flush();
}

/**
 * @brief Write the label binary file. (label -> binary file)
 *
 * @param label_index the index would be written
 * @param segment_label the label of the section in that frame
 */
void LabelController::_write_bin_label_data(const int label_index, const std::vector<int> &segment_label)
{
  _label_bin_file.seekp(label_index, std::ios::beg);
  for (const int label : segment_label)
    _label_bin_file.write(reinterpret_cast<const char *>(&label), sizeof(int));

  _label_bin_file.flush();
}

/**
 * @brief Record the numbers of labels
 *
 * @param nums
 */
void LabelController::_write_bin_label_num_data(const int nums)
{
  _label_num_bin_file.seekp(frame * sizeof(int), std::ios::beg);
  _label_num_bin_file.write(reinterpret_cast<const char *>(&nums), sizeof(int));
  _label_num_bin_file.flush();
}

/**
 * @brief Output the feature of all segments to normal txt file
 */
void LabelController::output_feature_data()
{
  auto bk_p = _feature_bin_file.tellg();    // record the pointer position.

  // open the file would be written
  std::ofstream feature_outfile(feature_output_path, std::ios::out | std::ios::trunc);
  if (feature_outfile.fail()) {
    std::cerr << "Cannot open file" << feature_output_path << '\n';
    std::cin.get();
    return;
  }

  // reset the pointer of binary file to the beginning.
  _feature_bin_file.seekg(0, std::ios::beg);
  double buf[FEATURE_NUM];    // one line of feature.
  // read and write the feature line by line.
  while (_feature_bin_file.read(reinterpret_cast<char *>(buf), FEATURE_NUM * sizeof(double))) {
    for (int i{}; i < FEATURE_NUM; ++i)
      feature_outfile << buf[i] << " \n"[i == FEATURE_NUM - 1];
  }

  // set the pointer to the original place
  _feature_bin_file.seekg(bk_p, std::ios::beg);
  // clear the file flag
  _feature_bin_file.clear();
}

/**
 * @brief Output the label of all segments into json file
 */
void LabelController::output_json_label_data()
{
  using json = nlohmann::json;
  using ordered_json = nlohmann::ordered_json;

  ordered_json label_data;
  label_data["frames"] = ordered_json::array();

  auto bk_p = _label_bin_file.tellg();    // record the pointer position.

  for (int i = 0; i < max_frame; ++i) {
    if (label_size_vec[i] != 0) {
      const std::vector<Eigen::MatrixXd> &one_frame_segment_vec = total_frame_segment_vec[i];    // the i-th frame
      std::vector<int> one_frame_segment_label(one_frame_segment_vec.size());

      ordered_json one_frame_json = ordered_json::object();
      ordered_json segments_json = ordered_json::array();

      // read the label of all segment in the frame
      _label_bin_file.seekg(label_index_vec[i], std::ios::beg);
      _label_bin_file.read(reinterpret_cast<char *>(one_frame_segment_label.data()), label_size_vec[i]);

      // iterate through all the segment in the frame
      for (int j = 0; j < one_frame_segment_vec.size(); ++j) {
        const Eigen::MatrixXd &segment = one_frame_segment_vec[j];    // the j-th segment in the frame
        int segment_size = segment.rows();

        ordered_json points_json = ordered_json::object();
        points_json["x"] = ordered_json::array();
        points_json["y"] = ordered_json::array();

        // output the x and y and the corresponding label of the segment
        for (int k = 0; k < segment_size; ++k) {
          points_json["x"].push_back(segment(k, 0));
          points_json["y"].push_back(segment(k, 1));
        }

        segments_json.push_back(ordered_json::object({ { "segment_index", j }, { "label", one_frame_segment_label[j] }, { "points", points_json } }));
      }

      one_frame_json["frame_index"] = i;
      one_frame_json["segments"] = segments_json;
      label_data["frames"].push_back(one_frame_json);
    }
  }

  // open the file would be written
  std::ofstream label_outfile(label_output_path, std::ios::out | std::ios::trunc);
  if (label_outfile.fail()) {
    std::cerr << "Cannot open file" << label_output_path << '\n';
    std::cin.get();
    return;
  }

  label_outfile << label_data.dump(2);

  // set the pointer to the original place
  _label_bin_file.seekg(bk_p, std::ios::beg);
  // clear the file flag
  _label_bin_file.clear();
}

/**
 * @brief Output the label of all segments to normal txt file
 */
void LabelController::output_xy_label_data()
{
  // open the file would be written
  std::ofstream label_outfile(label_output_path, std::ios::out | std::ios::trunc);
  if (label_outfile.fail()) {
    std::cerr << "Cannot open file" << label_output_path << '\n';
    std::cin.get();
    return;
  }

  auto bk_p = _label_bin_file.tellg();    // record the pointer position.

  for (int i = 0; i < max_frame; ++i) {
    if (label_size_vec[i] != 0) {
      const std::vector<Eigen::MatrixXd> &one_frame_segment_vec = total_frame_segment_vec[i];    // the i-th frame
      std::vector<int> one_frame_segment_label(one_frame_segment_vec.size());

      // read the label of all segment in the frame
      _label_bin_file.seekg(label_index_vec[i], std::ios::beg);
      _label_bin_file.read(reinterpret_cast<char *>(one_frame_segment_label.data()), label_size_vec[i]);

      // iterate through all the segment in the frame
      for (int j = 0; j < one_frame_segment_vec.size(); ++j) {
        const Eigen::MatrixXd &segment = one_frame_segment_vec[j];    // the j-th segment in the frame
        int segment_size = segment.rows();

        // output the x and y and the corresponding label of the segment
        for (int k = 0; k < segment_size; ++k)
          label_outfile << segment(k, 0) << ' ' << segment(k, 1) << ' ' << one_frame_segment_label[j] << '\n';
      }
    }
  }

  // set the pointer to the original place
  _label_bin_file.seekg(bk_p, std::ios::beg);
  // clear the file flag
  _label_bin_file.clear();
}

/**
 * @brief clean the file had been wrote, and reset the corresponding member.
 */
void LabelController::check_clean_data()
{
  // clean data
  if (clean_data) {
    // clean feature binary file and feature vector
    _feature_bin_file.seekg(0, std::ios::beg);
    _feature_bin_file.seekp(0, std::ios::beg);
    std::filesystem::resize_file(_feature_bin_path, 0);
    std::fill(feature_size_vec.begin(), feature_size_vec.end(), 0);
    std::fill(feature_index_vec.begin(), feature_index_vec.end(), 0);

    // clean label binary file and label vector
    _label_bin_file.seekg(0, std::ios::beg);
    _label_bin_file.seekp(0, std::ios::beg);
    std::filesystem::resize_file(_label_bin_path, 0);
    std::fill(label_size_vec.begin(), label_size_vec.end(), 0);
    std::fill(label_index_vec.begin(), label_index_vec.end(), 0);
    std::fill(total_frame_segment_vec.begin(), total_frame_segment_vec.end(), std::vector<Eigen::MatrixXd>());

    // clean label number binary file
    _label_num_bin_file.seekg(0, std::ios::beg);
    _label_num_bin_file.seekp(0, std::ios::beg);
    std::filesystem::resize_file(_label_num_bin_path, 0);

    // clean feature number binary file
    _feature_num_bin_file.seekg(0, std::ios::beg);
    _feature_num_bin_file.seekp(0, std::ios::beg);
    std::filesystem::resize_file(_feature_num_bin_path, 0);

    // clean the output file if it exist
    if (std::filesystem::exists(feature_output_path)) std::filesystem::resize_file(feature_output_path, 0);
    if (std::filesystem::exists(label_output_path)) std::filesystem::resize_file(label_output_path, 0);

    // reset member
    update_frame = true;
    save_label = false;
    enable_enter_save = false;
    output_txt = false;
    show_rect = false;
    show_nearest = false;
    auto_label = false;
    clean_data = false;

    frame = 0;
    xy_data = Eigen::MatrixXd::Zero(HZ, 2);
    current_save_frame = -1;
    writed_max_frame = -1;
    writed_frame_numbers = 0;
  }
}

/**
 * @brief Check if it needs to load new data.
 */
void LabelController::check_load_data()
{
  // load data
  if (load_data) {
    load_data = false;
    transform_frame();

    // resize the information vector
    label_size_vec.resize(max_frame);
    label_size_vec.shrink_to_fit();
    label_index_vec.resize(max_frame);
    label_index_vec.shrink_to_fit();
    feature_size_vec.resize(max_frame);
    feature_size_vec.shrink_to_fit();
    feature_index_vec.resize(max_frame);
    feature_index_vec.shrink_to_fit();
    total_frame_segment_vec.resize(max_frame);
    total_frame_segment_vec.shrink_to_fit();
  }
}

/**
 * @brief Check if it needs to update the frame. If true, it would read a new frame into the xy_data matrix, otherwise won't do anything.
 *
 */
void LabelController::check_update_frame()
{
  // update frame
  if (update_frame) {
    update_frame = false;

    // read the new frame into the xy_data matrix
    read_frame();

    // transform the matrix into feature.
    std::tie(feature_matrix, segment_vec) = MakeFeatures::section_to_feature(xy_data);

    segment_label.clear();
    segment_label.resize(segment_vec.size());
    segment_label.shrink_to_fit();

    // if it had been labeled, update the information vector.
    if (label_size_vec[frame] != 0) {
      _label_bin_file.seekg(label_index_vec[frame], std::ios::beg);
      _label_bin_file.read(reinterpret_cast<char *>(segment_label.data()), label_size_vec[frame]);
    }
  }
}

/**
 * @brief Check if it needs to save the data, it would output the data to the binary file, but not notmal txt file
 */
void LabelController::check_save_data()
{
  if ((show_rect && auto_label) || (show_nearest && auto_label))
    save_label = true;

  // save label
  if (save_label) {
    save_label = false;

    // have bot been written
    const bool have_not_been_written = (label_size_vec[frame] == 0);
    if (have_not_been_written)
      ++writed_frame_numbers;

    // set the size of the frame
    feature_size_vec[frame] = feature_matrix.size() * sizeof(double);
    label_size_vec[frame] = segment_vec.size() * sizeof(int);
    total_frame_segment_vec[frame] = segment_vec;

    int feature_index = 0, label_index = 0;
    // calculate the index of this feature in the binary file
    for (int sec_i = 0; sec_i < frame; ++sec_i) {
      feature_index += feature_size_vec[sec_i];    // if the frame didn't be writed, the size will be zero
      label_index += label_size_vec[sec_i];    // calculate the index of the label in the binary file
    }

    feature_index_vec[frame] = feature_index;    // It means the nth frame will been writed at `feature_index`
    label_index_vec[frame] = label_index;    // It means the nth label will been writed at `label_index`

    // upload the max frame has been writed
    if (frame > writed_max_frame)
      writed_max_frame = frame;

    // if it's insert, not append, then move all the data after this frame
    /*                           | 1. origin       2. move the data        3. push the data        4. move back the old data
     *   _______                 |                                                                   _______
     *  |___C___|                |   _______   =>            (tmp file) =>   _______            =>  |___C___|
     *  |___B___| <- insert this |  |___C___|       _______   _______       |___B___| _______       |___B___|
     *  |___A___|                |  |___A___|      |___A___| |___C___|      |___A___||___C___|      |___A___|
     */
    if (frame < writed_max_frame && have_not_been_written) {
      std::fstream buf_feature_file(_buf_feature_path, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
      if (buf_feature_file.fail()) {
        std::cerr << "Cannot open file" << _buf_feature_path << '\n';
        std::cin.get();
        exit(1);
      }

      std::fstream buf_label_file(_buf_label_path, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
      if (buf_label_file.fail()) {
        std::cerr << "Cannot open file" << _buf_label_path << '\n';
        std::cin.get();
        exit(1);
      }

      std::vector<double> section_feature_buf;
      std::vector<int> section_label_buf;

      // copy all the data after this frame and move it
      _feature_bin_file.seekg(feature_index_vec[frame], std::ios::beg);    // copy from the position will begin written
      _label_bin_file.seekg(label_index_vec[frame], std::ios::beg);

      for (int sec_i = frame + 1; sec_i <= writed_max_frame; ++sec_i) {
        if (feature_size_vec[sec_i] != 0) {
          // copy the section(one frame) data
          section_feature_buf.resize(feature_size_vec[sec_i] / sizeof(double));
          section_feature_buf.shrink_to_fit();
          _feature_bin_file.read(reinterpret_cast<char *>(section_feature_buf.data()), feature_size_vec[sec_i]);    // copy
          buf_feature_file.write(reinterpret_cast<char *>(section_feature_buf.data()), feature_size_vec[sec_i]);    // and write

          section_label_buf.resize(label_size_vec[sec_i] / sizeof(int));
          section_label_buf.shrink_to_fit();
          _label_bin_file.read(reinterpret_cast<char *>(section_label_buf.data()), label_size_vec[sec_i]);    // copy
          buf_label_file.write(reinterpret_cast<char *>(section_label_buf.data()), label_size_vec[sec_i]);    // and write

          feature_index_vec[sec_i] += feature_size_vec[frame];    // add the offset of the data we will insert
          label_index_vec[sec_i] += label_size_vec[frame];    // add the offset of the data we will insert
        }
      }

      buf_feature_file.flush();
      buf_label_file.flush();
      buf_feature_file.seekg(0, std::ios::beg);
      buf_label_file.seekg(0, std::ios::beg);

      _write_bin_feature_data(feature_index, feature_matrix);
      _write_bin_label_data(label_index, segment_label);

      for (int sec_i = frame + 1; sec_i <= writed_max_frame; ++sec_i) {
        if (feature_size_vec[sec_i] != 0) {
          section_feature_buf.resize(feature_size_vec[sec_i] / sizeof(double));
          section_feature_buf.shrink_to_fit();
          buf_feature_file.read(reinterpret_cast<char *>(section_feature_buf.data()), feature_size_vec[sec_i]);    // copy
          _feature_bin_file.write(reinterpret_cast<char *>(section_feature_buf.data()), feature_size_vec[sec_i]);    // and write

          section_label_buf.resize(label_size_vec[sec_i] / sizeof(int));
          section_label_buf.shrink_to_fit();
          buf_label_file.read(reinterpret_cast<char *>(section_label_buf.data()), label_size_vec[sec_i]);    // copy
          _label_bin_file.write(reinterpret_cast<char *>(section_label_buf.data()), label_size_vec[sec_i]);    // and write
        }
      }

      _feature_bin_file.flush();
      _label_bin_file.flush();
    }
    else {
      // directly append the data
      _write_bin_feature_data(feature_index, feature_matrix);
      _write_bin_label_data(label_index, segment_label);
    }

    _write_bin_label_num_data(segment_vec.size() * sizeof(int));
    _write_bin_feature_num_data(feature_matrix.size() * sizeof(double));
  }
}

LabelController::LabelController() : AnimationController()
{
  save_label = false;
  enable_enter_save = false;
  output_txt = false;
  show_rect = false;
  show_nearest = false;
  auto_label = false;
  clean_data = false;
  load_data = false;

  current_save_time = std::chrono::system_clock::now();

  label_mouse_area = static_cast<float>(0.05);

  _tool_data_path = FileHandler::get_MRL_project_root() + "/dataset/binary_data/MesToolLabelController.dat";

  bool set_tool_data_file = false;
  // check if the tool data file exist
  if (!std::filesystem::exists(_tool_data_path) || std::filesystem::file_size(_tool_data_path) == 0)
    set_tool_data_file = true;
  else {
    // if the file exist, check if the path in the file exist
    std::ifstream infile(_tool_data_path);
    std::string line;
    std::getline(infile, line);

    // if the path in the file doesn't exist, set the tool data file
    if (!std::filesystem::exists(line))
      set_tool_data_file = true;

    // if it can't open the file, set the tool data file
    if (std::ifstream test_file(line); test_file.fail())
      set_tool_data_file = true;
  }

  if (set_tool_data_file) {
    // clear the file content and set the default path
    std::ofstream _tool_data_file(_tool_data_path, std::ios::out | std::ios::trunc);
    if (_tool_data_file.fail()) {
      std::cerr << "cant open " << _tool_data_path << '\n';
      std::cin.get();
      exit(1);
    }

    // write the default path to the file
    _tool_data_file << FileHandler::get_MRL_project_root() + "/dataset/raw_data/demo_train_xy.txt" << '\n'    // default raw_data_path
                    << FileHandler::get_MRL_project_root() + "/dataset/binary_data/label_using_raw_data_bin.txt" << '\n'    // default _raw_bin_path
                    << FileHandler::get_MRL_project_root() + "/dataset/default_data/default_feature_data.txt" << '\n'    // default feature_output_path
                    << FileHandler::get_MRL_project_root() + "/dataset/default_data/default_label_data.txt" << '\n'    // default label_output_path
                    << FileHandler::get_MRL_project_root() + "/dataset/binary_data/feature_bin.txt" << '\n'    // default _feature_bin_path
                    << FileHandler::get_MRL_project_root() + "/dataset/binary_data/feature_num_bin.txt" << '\n'    // default _feature_num_bin_path
                    << FileHandler::get_MRL_project_root() + "/dataset/binary_data/label_bin.txt" << '\n'    // default _label_bin_path
                    << FileHandler::get_MRL_project_root() + "/dataset/binary_data/label_num_bin.txt" << '\n'    // default _label_num_bin_path
                    << 360 << '\n'    // set default HZ to 360
                    << -1 << '\n'    // current_save_frame
                    << -1 << '\n'    // writed_max_frame
                    << 0;    // writed_frame_numbers
  }

  {
    // read the path from the tool file
    std::ifstream _tool_data_file(_tool_data_path);
    if (_tool_data_file.fail()) {
      std::cerr << "cant open " << _tool_data_path << '\n';
      std::cin.get();
      exit(1);
    }

    std::getline(_tool_data_file, raw_data_path);
    std::getline(_tool_data_file, _raw_bin_path);
    std::getline(_tool_data_file, feature_output_path);
    std::getline(_tool_data_file, label_output_path);
    std::getline(_tool_data_file, _feature_bin_path);
    std::getline(_tool_data_file, _feature_num_bin_path);
    std::getline(_tool_data_file, _label_bin_path);
    std::getline(_tool_data_file, _label_num_bin_path);
    _buf_feature_path = FileHandler::get_MRL_project_root() + "/dataset/binary_data/tmp_feature_buffer_data";
    _buf_label_path = FileHandler::get_MRL_project_root() + "/dataset/binary_data/tmp_lable_buffer_data";

    std::string line;
    std::getline(_tool_data_file, line);
    HZ = std::stoi(line);
    xy_data = Eigen::MatrixXd::Zero(HZ, 2);

    std::getline(_tool_data_file, line);
    current_save_frame = std::stoi(line);

    std::getline(_tool_data_file, line);
    writed_max_frame = std::stoi(line);

    std::getline(_tool_data_file, line);
    writed_frame_numbers = std::stoi(line);
  }

  if (!std::filesystem::exists(_feature_bin_path)) std::ofstream create_file(_feature_bin_path);    // just for creating file.
  if (!std::filesystem::exists(_feature_num_bin_path)) std::ofstream create_file(_feature_num_bin_path);    // just for creating file.
  if (!std::filesystem::exists(_label_bin_path)) std::ofstream create_file(_label_bin_path);    // just for creating file.
  if (!std::filesystem::exists(_label_num_bin_path)) std::ofstream create_file(_label_num_bin_path);    // just for creating file.

  _feature_bin_file.open(_feature_bin_path, std::ios::in | std::ios::out | std::ios::binary);
  if (_feature_bin_file.fail()) {
    std::cerr << "cant open " << _feature_bin_path << '\n';
    std::cin.get();
    exit(1);
  }

  _feature_num_bin_file.open(_feature_num_bin_path, std::ios::in | std::ios::out | std::ios::binary);
  if (_feature_num_bin_file.fail()) {
    std::cerr << "cant open " << _feature_num_bin_path << '\n';
    std::cin.get();
    exit(1);
  }

  _label_bin_file.open(_label_bin_path, std::ios::in | std::ios::out | std::ios::binary);
  if (_label_bin_file.fail()) {
    std::cerr << "cant open " << _label_bin_path << '\n';
    std::cin.get();
    exit(1);
  }

  _label_num_bin_file.open(_label_num_bin_path, std::ios::in | std::ios::out | std::ios::binary);
  if (_label_num_bin_file.fail()) {
    std::cerr << "cant open " << _label_num_bin_path << '\n';
    std::cin.get();
    exit(1);
  }

  transform_frame();
  label_size_vec.resize(max_frame);
  label_index_vec.resize(max_frame);
  feature_size_vec.resize(max_frame);
  feature_index_vec.resize(max_frame);
  total_frame_segment_vec.resize(max_frame);

  if (std::filesystem::file_size(_label_num_bin_path) == 0) {
    const int tmp_n = 0;
    const double tmp_d = 0.0;

    for (int i = 0; i < max_frame; ++i) {
      _label_num_bin_file.write(reinterpret_cast<const char *>(&tmp_n), sizeof(int));
      _feature_num_bin_file.write(reinterpret_cast<const char *>(&tmp_d), sizeof(double));
    }

    _label_num_bin_file.flush();
    _feature_num_bin_file.flush();
    _label_num_bin_file.seekp(0, std::ios::beg);
    _label_num_bin_file.seekg(0, std::ios::beg);
  }
  else {
    for (int i = 0; i < max_frame; ++i) {
      _label_num_bin_file.read(reinterpret_cast<char *>(&label_size_vec[i]), sizeof(int));
      _feature_num_bin_file.read(reinterpret_cast<char *>(&feature_size_vec[i]), sizeof(double));

      for (int j = 0; j < i; ++j) {
        label_index_vec[i] += label_size_vec[j];
        feature_index_vec[i] += feature_size_vec[j];
      }
    }
  }
}

LabelController::~LabelController()
{
  std::filesystem::remove(_buf_feature_path);
  std::filesystem::remove(_buf_label_path);

  std::ofstream _tool_data_file(_tool_data_path, std::ios::out | std::ios::trunc);
  if (_tool_data_file.fail()) {
    std::cerr << "cant open " << _tool_data_path << '\n';
    std::cin.get();
    exit(1);
  }

  _tool_data_file << raw_data_path << '\n'
                  << _raw_bin_path << '\n'
                  << feature_output_path << '\n'
                  << label_output_path << '\n'
                  << _feature_bin_path << '\n'
                  << _feature_num_bin_path << '\n'
                  << _label_bin_path << '\n'
                  << _label_num_bin_path << '\n'
                  << HZ << '\n'
                  << current_save_frame << '\n'
                  << writed_max_frame << '\n'
                  << writed_frame_numbers;
}