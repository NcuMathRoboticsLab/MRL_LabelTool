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
void LabelController::write_bin_feature_data__(const int feature_index, const Eigen::MatrixXd &feature_matrix)
{
  feature_bin_file__.seekp(feature_index, std::ios::beg);
  for (const auto &row : feature_matrix.rowwise()) {
    for (const double feature : row)
      feature_bin_file__.write(reinterpret_cast<const char *>(&feature), sizeof(double));
  }
}

/**
 * @brief Record the numbers of features.
 *
 * @param nums
 */
void LabelController::write_bin_feature_num_data__(const int nums)
{
  feature_num_bin_file__.seekp(frame * sizeof(double), std::ios::beg);
  feature_num_bin_file__.write(reinterpret_cast<const char *>(&nums), sizeof(double));
}

/**
 * @brief Write the label binary file. (label -> binary file)
 *
 * @param label_index the index would be written
 * @param segment_label the label of the section in that frame
 */
void LabelController::write_bin_label_data__(const int label_index, const std::vector<int> &segment_label)
{
  label_bin_file__.seekp(label_index, std::ios::beg);
  for (const int label : segment_label)
    label_bin_file__.write(reinterpret_cast<const char *>(&label), sizeof(int));
}

/**
 * @brief Record the numbers of labels
 *
 * @param nums
 */
void LabelController::write_bin_label_num_data__(const int nums)
{
  label_num_bin_file__.seekp(frame * sizeof(int), std::ios::beg);
  label_num_bin_file__.write(reinterpret_cast<const char *>(&nums), sizeof(int));
}

/**
 * @brief Output the feature of all segments to normal txt file
 */
void LabelController::output_feature_data()
{
  auto bk_p = feature_bin_file__.tellg();    // record the pointer position.

  // open the file would be written
  std::ofstream feature_outfile(feature_output_path, std::ios::out | std::ios::trunc);
  if (feature_outfile.fail()) {
    std::cerr << "Cannot open file" << feature_output_path << '\n';
    std::cin.get();
    return;
  }

  // reset the pointer of binary file to the beginning.
  feature_bin_file__.seekg(0, std::ios::beg);
  double buf[FEATURE_NUM];    // one line of feature.
  // read and write the feature line by line.
  while (feature_bin_file__.read(reinterpret_cast<char *>(buf), FEATURE_NUM * sizeof(double))) {
    for (int i{}; i < FEATURE_NUM; ++i)
      feature_outfile << buf[i] << " \n"[i == FEATURE_NUM - 1];
  }

  // set the pointer to the original place
  feature_bin_file__.seekg(bk_p, std::ios::beg);
  // clear the file flag
  feature_bin_file__.clear();
}

/**
 * @brief Output the label of all segments to normal txt file
 */
void LabelController::output_label_data()
{
  auto bk_p = label_bin_file__.tellg();    // record the pointer position.

  // open the file would be written
  std::ofstream label_outfile(label_output_path, std::ios::out | std::ios::trunc);
  if (label_outfile.fail()) {
    std::cerr << "Cannot open file" << label_output_path << '\n';
    std::cin.get();
    return;
  }

  // reset the pointer of binary file to the beginning.
  label_bin_file__.seekg(0, std::ios::beg);
  int buf;    // the buffer of label
  // read and write all label
  while (label_bin_file__.read(reinterpret_cast<char *>(&buf), sizeof(int)))
    label_outfile << buf << '\n';

  // set the pointer to the original place
  label_bin_file__.seekg(bk_p, std::ios::beg);
  // clear the file flag
  label_bin_file__.clear();
}

/**
 * @brief clean the file had been wrote, and reset the corresponding member.
 */
void LabelController::check_clean_data()
{
  // clean data
  if (clean_data) {
    // clean feature binary file and feature vector
    feature_bin_file__.seekg(0, std::ios::beg);
    feature_bin_file__.seekp(0, std::ios::beg);
    std::filesystem::resize_file(feature_bin_path__, 0);
    std::fill(feature_size_vec.begin(), feature_size_vec.end(), 0);
    std::fill(feature_index_vec.begin(), feature_index_vec.end(), 0);

    // clean label binary file and label vector
    label_bin_file__.seekg(0, std::ios::beg);
    label_bin_file__.seekp(0, std::ios::beg);
    std::filesystem::resize_file(label_bin_path__, 0);
    std::fill(label_size_vec.begin(), label_size_vec.end(), 0);
    std::fill(label_index_vec.begin(), label_index_vec.end(), 0);

    // clean label number binary file
    label_num_bin_file__.seekg(0, std::ios::beg);
    label_num_bin_file__.seekp(0, std::ios::beg);
    std::filesystem::resize_file(label_num_bin_path__, 0);

    // clean feature number binary file
    feature_num_bin_file__.seekg(0, std::ios::beg);
    feature_num_bin_file__.seekp(0, std::ios::beg);
    std::filesystem::resize_file(feature_num_bin_path__, 0);

    // clean the output file if it exist
    if (std::filesystem::exists(feature_output_path)) std::filesystem::resize_file(feature_output_path, 0);
    if (std::filesystem::exists(label_output_path)) std::filesystem::resize_file(label_output_path, 0);

    // reset member
    clean_data = false;
    update_frame = true;
    save_label = false;
    enable_enter_save = false;
    output_txt = false;
    show_rect = false;
    show_nearest = false;
    auto_label = false;
    clean_data = false;
    load_data = false;

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
    clean_data = true;
    check_clean_data();
    transform_frame();

    // resize the information vector
    label_size_vec.resize(max_frame + 1);
    label_size_vec.shrink_to_fit();
    label_index_vec.resize(max_frame + 1);
    label_index_vec.shrink_to_fit();
    feature_size_vec.resize(max_frame + 1);
    feature_size_vec.shrink_to_fit();
    feature_index_vec.resize(max_frame + 1);
    feature_index_vec.shrink_to_fit();
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
      label_bin_file__.seekg(label_index_vec[frame], std::ios::beg);
      label_bin_file__.read(reinterpret_cast<char *>(segment_label.data()), label_size_vec[frame]);
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
    bool have_not_been_written = (label_size_vec[frame] == 0);
    if (have_not_been_written)
      ++writed_frame_numbers;

    // set the size of the frame
    feature_size_vec[frame] = feature_matrix.size() * sizeof(double);
    label_size_vec[frame] = segment_vec.size() * sizeof(int);

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
      std::fstream buf_feature_file(buf_feature_path__, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
      if (buf_feature_file.fail()) {
        std::cerr << "Cannot open file" << buf_feature_path__ << '\n';
        std::cin.get();
        exit(1);
      }

      std::fstream buf_label_file(buf_label_path__, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
      if (buf_label_file.fail()) {
        std::cerr << "Cannot open file" << buf_label_path__ << '\n';
        std::cin.get();
        exit(1);
      }

      std::vector<double> section_feature_buf;
      std::vector<int> section_label_buf;

      // copy all the data after this frame and move it
      feature_bin_file__.seekg(feature_index_vec[frame], std::ios::beg);    // copy from the position will begin written
      label_bin_file__.seekg(label_index_vec[frame], std::ios::beg);

      for (int sec_i = frame + 1; sec_i <= writed_max_frame; ++sec_i) {
        if (feature_size_vec[sec_i] != 0) {
          // copy the section(one frame) data
          section_feature_buf.resize(feature_size_vec[sec_i] / sizeof(double));
          section_feature_buf.shrink_to_fit();
          feature_bin_file__.read(reinterpret_cast<char *>(section_feature_buf.data()), feature_size_vec[sec_i]);    // copy
          buf_feature_file.write(reinterpret_cast<char *>(section_feature_buf.data()), feature_size_vec[sec_i]);    // and write

          section_label_buf.resize(label_size_vec[sec_i] / sizeof(int));
          section_label_buf.shrink_to_fit();
          label_bin_file__.read(reinterpret_cast<char *>(section_label_buf.data()), label_size_vec[sec_i]);    // copy
          buf_label_file.write(reinterpret_cast<char *>(section_label_buf.data()), label_size_vec[sec_i]);    // and write

          feature_index_vec[sec_i] += feature_size_vec[frame];    // add the offset of the data we will insert
          label_index_vec[sec_i] += label_size_vec[frame];    // add the offset of the data we will insert
        }
      }

      buf_feature_file.flush();
      buf_label_file.flush();
      buf_feature_file.seekg(0, std::ios::beg);
      buf_label_file.seekg(0, std::ios::beg);

      write_bin_feature_data__(feature_index, feature_matrix);
      write_bin_label_data__(label_index, segment_label);

      for (int sec_i = frame + 1; sec_i <= writed_max_frame; ++sec_i) {
        if (feature_size_vec[sec_i] != 0) {
          section_feature_buf.resize(feature_size_vec[sec_i] / sizeof(double));
          section_feature_buf.shrink_to_fit();
          buf_feature_file.read(reinterpret_cast<char *>(section_feature_buf.data()), feature_size_vec[sec_i]);    // copy
          feature_bin_file__.write(reinterpret_cast<char *>(section_feature_buf.data()), feature_size_vec[sec_i]);    // and write

          section_label_buf.resize(label_size_vec[sec_i] / sizeof(int));
          section_label_buf.shrink_to_fit();
          buf_label_file.read(reinterpret_cast<char *>(section_label_buf.data()), label_size_vec[sec_i]);    // copy
          label_bin_file__.write(reinterpret_cast<char *>(section_label_buf.data()), label_size_vec[sec_i]);    // and write
        }
      }

      feature_bin_file__.flush();
      label_bin_file__.flush();
    }
    else {
      // directly append the data
      write_bin_feature_data__(feature_index, feature_matrix);
      write_bin_label_data__(label_index, segment_label);
    }

    write_bin_label_num_data__(segment_vec.size() * sizeof(int));
    write_bin_feature_num_data__(feature_matrix.size() * sizeof(double));
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

  current_save_frame = -1;
  writed_max_frame = -1;
  writed_frame_numbers = 0;
  current_save_time = std::chrono::system_clock::now();

  label_mouse_area = static_cast<float>(0.05);

  tool_data_path__ = FileHandler::get_filepath() + "\\dataset\\binary_data\\MesTool.dat";

  if (!std::filesystem::exists(tool_data_path__) || std::filesystem::file_size(tool_data_path__) == 0) {
    std::ofstream tool_data_file__(tool_data_path__);    // just for creating file.
    if (tool_data_file__.fail()) {
      std::cerr << "cant open " << tool_data_path__ << '\n';
      std::cin.get();
      exit(1);
    }

    tool_data_file__ << FileHandler::get_filepath() + "\\dataset\\raw_data\\demo_train_xy.txt" << '\n'    // default raw_data_path
                     << FileHandler::get_filepath() + "\\dataset\\binary_data\\label_using_raw_data_bin.txt" << '\n'    // default raw_bin_path__
                     << FileHandler::get_filepath() + "\\dataset\\default_data\\default_feature_data.txt" << '\n'    // default feature_output_path
                     << FileHandler::get_filepath() + "\\dataset\\default_data\\default_label_data.txt" << '\n'    // default label_output_path
                     << FileHandler::get_filepath() + "\\dataset\\binary_data\\feature_bin.txt" << '\n'    // default feature_bin_path__
                     << FileHandler::get_filepath() + "\\dataset\\binary_data\\feature_num_bin.txt" << '\n'    // default feature_num_bin_path__
                     << FileHandler::get_filepath() + "\\dataset\\binary_data\\label_bin.txt" << '\n'    // default label_bin_path__
                     << FileHandler::get_filepath() + "\\dataset\\binary_data\\label_num_bin.txt";    // default label_num_bin_path__
  }

  {
    std::ifstream tool_data_file__(tool_data_path__);
    if (tool_data_file__.fail()) {
      std::cerr << "cant open " << tool_data_path__ << '\n';
      std::cin.get();
      exit(1);
    }

    std::getline(tool_data_file__, raw_data_path);
    std::getline(tool_data_file__, raw_bin_path__);
    std::getline(tool_data_file__, feature_output_path);
    std::getline(tool_data_file__, label_output_path);
    std::getline(tool_data_file__, feature_bin_path__);
    std::getline(tool_data_file__, feature_num_bin_path__);
    std::getline(tool_data_file__, label_bin_path__);
    std::getline(tool_data_file__, label_num_bin_path__);
    buf_feature_path__ = FileHandler::get_filepath() + "/dataset/binary_data/tmp_feature_buffer_data";
    buf_label_path__ = FileHandler::get_filepath() + "/dataset/binary_data/tmp_lable_buffer_data";
  }

  if (!std::filesystem::exists(feature_bin_path__)) std::ofstream create_file(feature_bin_path__);    // just for creating file.
  if (!std::filesystem::exists(feature_num_bin_path__)) std::ofstream create_file(feature_num_bin_path__);    // just for creating file.
  if (!std::filesystem::exists(label_bin_path__)) std::ofstream create_file(label_bin_path__);    // just for creating file.
  if (!std::filesystem::exists(label_num_bin_path__)) std::ofstream create_file(label_num_bin_path__);    // just for creating file.

  feature_bin_file__.open(feature_bin_path__, std::ios::in | std::ios::out | std::ios::binary);
  if (feature_bin_file__.fail()) {
    std::cerr << "cant open " << feature_bin_path__ << '\n';
    std::cin.get();
    exit(1);
  }

  feature_num_bin_file__.open(feature_num_bin_path__, std::ios::in | std::ios::out | std::ios::binary);
  if (feature_num_bin_file__.fail()) {
    std::cerr << "cant open " << feature_num_bin_path__ << '\n';
    std::cin.get();
    exit(1);
  }

  label_bin_file__.open(label_bin_path__, std::ios::in | std::ios::out | std::ios::binary);
  if (label_bin_file__.fail()) {
    std::cerr << "cant open " << label_bin_path__ << '\n';
    std::cin.get();
    exit(1);
  }

  label_num_bin_file__.open(label_num_bin_path__, std::ios::in | std::ios::out | std::ios::binary);
  if (label_num_bin_file__.fail()) {
    std::cerr << "cant open " << label_num_bin_path__ << '\n';
    std::cin.get();
    exit(1);
  }
  transform_frame();
  label_size_vec.resize(max_frame + 1);
  label_index_vec.resize(max_frame + 1);
  feature_size_vec.resize(max_frame + 1);
  feature_index_vec.resize(max_frame + 1);

  if (std::filesystem::file_size(label_num_bin_path__) == 0) {
    const int tmp_n = 0;
    const double tmp_d = 0.0;

    for (int i = 0; i < max_frame + 1; ++i) {
      label_num_bin_file__.write(reinterpret_cast<const char *>(&tmp_n), sizeof(int));
      feature_num_bin_file__.write(reinterpret_cast<const char *>(&tmp_d), sizeof(double));
    }

    label_num_bin_file__.seekp(0, std::ios::beg);
  }
  else {
    for (int i = 0; i < max_frame + 1; ++i) {
      label_num_bin_file__.read(reinterpret_cast<char *>(&label_size_vec[i]), sizeof(int));
      feature_num_bin_file__.read(reinterpret_cast<char *>(&feature_size_vec[i]), sizeof(double));

      for (int j = 0; j < i; ++j) {
        label_index_vec[i] += label_size_vec[j];
        feature_index_vec[i] += feature_size_vec[j];
      }
    }
  }
}

LabelController::~LabelController()
{
  std::filesystem::remove(buf_feature_path__);
  std::filesystem::remove(buf_label_path__);

  std::ofstream tool_data_file__(tool_data_path__);
  if (tool_data_file__.fail()) {
    std::cerr << "cant open " << tool_data_path__ << '\n';
    std::cin.get();
    exit(1);
  }

  tool_data_file__ << raw_data_path << '\n'
                   << raw_bin_path__ << '\n'
                   << feature_output_path << '\n'
                   << label_output_path << '\n'
                   << feature_bin_path__ << '\n'
                   << feature_num_bin_path__ << '\n'
                   << label_bin_path__ << '\n'
                   << label_num_bin_path__;
}