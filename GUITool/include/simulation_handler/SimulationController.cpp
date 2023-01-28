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
  Target_X = 0.0, Target_Y = 0.0;

  raw_data_path = FileHandler::get_filepath() + "\\dataset\\raw_data\\demo_test_xy.txt";
  raw_bin_path__ = FileHandler::get_filepath() + "\\dataset\\binary_data\\simulation_using_raw_data_bin.txt";

  weight_data_path = FileHandler::get_filepath() + "\\dataset\\weight_data\\adaboost_ball_weight.txt";

  transform_frame();
}