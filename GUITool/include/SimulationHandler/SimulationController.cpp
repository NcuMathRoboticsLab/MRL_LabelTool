/**
 * @file SimulationController.cpp
 * @author Mes (mes900903@gmail.com) (Discord: Mes#0903)
 * @brief The implementation of the Simulation Controller
 * @version 0.1
 * @date 2023-01-24
 *
 * @copyright Copyright (c) 2023 Mes
 *
 */

#include "SimulationController.h"
#include "file_handler.h"
#include "make_feature.h"

#include <filesystem>

/**
 * @brief Check if it needs to update the frame. If true, it would read a new frame into the xy_data matrix, otherwise won't do anything.
 */
void SimulationController::check_update_frame()
{
  if (update_frame) {
    update_frame = false;

    read_frame();
    std::tie(feature_matrix, segment_vec) = MakeFeatures::section_to_feature(xy_data);
  }
}

SimulationController::SimulationController()
{
  _tool_data_path = FileHandler::get_MRL_project_root() + "/dataset/binary_data/MesToolSimulationController.dat";
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
    _tool_data_file << FileHandler::get_MRL_project_root() + "/dataset/raw_data/demo_test_xy.txt" << '\n'    // default raw_data_path
                    << FileHandler::get_MRL_project_root() + "/dataset/binary_data/simulation_using_raw_data_bin.txt" << '\n'    // default _raw_bin_path
                    << FileHandler::get_MRL_project_root() + "/dataset/weight_data/adaboost_ball_weight.txt" << '\n'    // default weight_data_path
                    << 360;    // set default HZ to 360
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
    std::getline(_tool_data_file, weight_data_path);

    std::string HZ_str;
    std::getline(_tool_data_file, HZ_str);
    HZ = std::stoi(HZ_str);
    xy_data = Eigen::MatrixXd::Zero(HZ, 2);
  }

  Target_X = 0.0, Target_Y = 0.0;

  transform_frame();
}

SimulationController::~SimulationController()
{
  // write the path to the tool file
  std::ofstream _tool_data_file(_tool_data_path, std::ios::out | std::ios::trunc);
  if (_tool_data_file.fail()) {
    std::cerr << "cant open " << _tool_data_path << '\n';
    std::cin.get();
    exit(1);
  }

  _tool_data_file << raw_data_path << '\n'
                  << _raw_bin_path << '\n'
                  << weight_data_path << '\n'
                  << HZ;
}