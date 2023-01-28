/**
 * @file show_simulation_window.cpp
 * @author Mes (mes900903@gmail.com) (Discord: Mes#0903)
 * @brief The implementation of simulation GUI.
 * @version 0.1
 * @date 2023-01-24
 *
 * @copyright Copyright (c) 2023 Mes
 *
 */

#include "Eigen/Eigen"
#include "imgui_header.h"
#include "show_simulation_window.h"
#include "SimulationController.h"

#include "make_feature.h"
#include "adaboost.h"
#include "logistic.h"
#include "segment.h"
#include "normalize.h"
#include "file_handler.h"

#include <chrono>
#include <thread>

static Adaboost<logistic> Model;
static Normalizer normalizer;

static SimulationController SC;    // simulation animation info

static ImVec4 color_arr[] = { ImVec4(192 / 255.0, 238 / 255.0, 228 / 255.0, 1),
                              ImVec4(248 / 255.0, 249 / 255.0, 136 / 255.0, 1),
                              ImVec4(255 / 255.0, 202 / 255.0, 200 / 255.0, 1),
                              ImVec4(255 / 255.0, 158 / 255.0, 158 / 255.0, 1),
                              ImVec4(250 / 255.0, 248 / 255.0, 241 / 255.0, 1),
                              ImVec4(250 / 255.0, 234 / 255.0, 177 / 255.0, 1),
                              ImVec4(229 / 255.0, 186 / 255.0, 115 / 255.0, 1),
                              ImVec4(197 / 255.0, 137 / 255.0, 64 / 255.0, 1),
                              ImVec4(204 / 255.0, 214 / 255.0, 166 / 255.0, 1) };

/**
 * @brief The window controled detail member in Simulation Controller
 */
void ShowSimulationInformation()
{
  if (ImGui::TreeNodeEx("Simulation Information")) {
    /*----------Load raw data----------*/
    if (ImGui::Button("Load raw data"))
      ImGuiFileDialog::Instance()->OpenDialog("LoadSimulationRawData", "Choose your raw data", ".*", FileHandler::get_filepath() + "\\");

    ImGui::SameLine();
    ImGui::Text("path: %s", SC.raw_data_path.c_str());
    // display
    if (ImGuiFileDialog::Instance()->Display("LoadSimulationRawData", ImGuiWindowFlags_NoCollapse, ImVec2(600, 500))) {
      // action if OK
      if (ImGuiFileDialog::Instance()->IsOk()) {
        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
        std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

        if (filePath != SC.raw_data_path) {
          SC.raw_data_path = filePathName;
          SC.transform_frame();
          SC.frame = 0;
          SC.update_frame = true;
        }
      }

      ImGuiFileDialog::Instance()->Close();
    }

    /*----------Load trained weight data----------*/
    if (ImGui::Button("Load trained weight data"))
      ImGuiFileDialog::Instance()->OpenDialog("LoadSimulationWeightData", "Choose your weight data", ".*", FileHandler::get_filepath() + "\\");

    ImGui::SameLine();
    ImGui::Text("path: %s", SC.weight_data_path.c_str());
    // display
    if (ImGuiFileDialog::Instance()->Display("LoadSimulationWeightData", ImGuiWindowFlags_NoCollapse, ImVec2(600, 500))) {
      // action if OK
      if (ImGuiFileDialog::Instance()->IsOk()) {
        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
        std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

        if (filePath != SC.weight_data_path) {
          SC.weight_data_path = filePathName;
          FileHandler::load_weight(SC.weight_data_path, Model, normalizer);

          SC.update_frame = true;
        }
      }

      ImGuiFileDialog::Instance()->Close();
    }

    /*----------Print target position----------*/
    float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    ImGui::Text("Target is at: [%f, %f]", SC.Target_X, SC.Target_Y);

    /*----------Robot HZ----------*/
    ImVec2 current_windows_size = ImGui::GetWindowSize();
    static int current_HZ = SC.HZ;

    if (ImGui::Button("720")) {
      SC.HZ = 720;

      if (current_HZ != SC.HZ) {
        current_HZ = SC.HZ;
        SC.max_frame = static_cast<int>(SC.max_frame / 2.0);    // 360->720
      }

      SC.xy_data = Eigen::MatrixXd::Zero(SC.HZ, 2);
      SC.frame = 0;
      SC.update_frame = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("360")) {
      SC.HZ = 360;

      if (current_HZ != SC.HZ) {
        current_HZ = SC.HZ;
        SC.max_frame *= 2;    // 720->360
      }

      SC.xy_data = Eigen::MatrixXd::Zero(SC.HZ, 2);
      SC.frame = 0;
      SC.update_frame = true;
    }

    ImGui::SameLine();
    ImGui::Text("HZ:%d", SC.HZ);

    /*----------Simulation Window Size----------*/
    ImGui::Text("Simulation Window Size Control:");
    ImGui::SameLine();
    ImGui::PushButtonRepeat(true);

    if (ImGui::ArrowButton("simulation_window_size_left", ImGuiDir_Left) && SC.window_size > 100)
      --SC.window_size;

    ImGui::SameLine(0.0f, spacing);
    if (ImGui::ArrowButton("simulation_window_size_right", ImGuiDir_Right) && SC.window_size < 2000)
      ++SC.window_size;

    ImGui::PopButtonRepeat();
    ImGui::SameLine(0.0f, spacing);
    ImGui::PushItemWidth(current_windows_size.x / 3.0f);
    ImGui::SliderInt("Simulation Window size", &SC.window_size, 100, 2000, "%d");
    ImGui::SameLine();
    ImGui::Text(":%d", SC.window_size);

    /*----------Frame Control----------*/
    ImGui::Text("Max Frame: %d", SC.max_frame);
    ImGui::PushButtonRepeat(true);

    ImGui::Text("Frame Control:");
    ImGui::SameLine();

    if (ImGui::ArrowButton("frame_left", ImGuiDir_Left) && !SC.auto_play && SC.frame > 0) {
      SC.update_frame = true;
      --SC.frame;
    }

    ImGui::SameLine(0.0f, spacing);
    if (ImGui::ArrowButton("frame_right", ImGuiDir_Right) && !SC.auto_play && SC.frame < SC.max_frame) {
      SC.update_frame = true;
      ++SC.frame;
    }

    ImGui::PopButtonRepeat();
    ImGui::SameLine(0.0f, spacing);
    ImGui::PushItemWidth(current_windows_size.x / 3.0f);
    if (ImGui::SliderInt("Frame", &SC.frame, 0, SC.max_frame, "%d"))
      SC.update_frame = true;

    ImGui::SameLine();
    ImGui::Text(": %d", SC.frame);

    /*----------FPS Control----------*/
    ImGui::Text("FPS Control:");
    ImGui::SameLine();
    ImGui::PushButtonRepeat(true);

    if (ImGui::ArrowButton("fps_left", ImGuiDir_Left) && SC.fps > 1)
      --SC.fps;

    ImGui::SameLine(0.0f, spacing);
    if (ImGui::ArrowButton("fps_right", ImGuiDir_Right) && SC.fps < 200)
      ++SC.fps;

    ImGui::SameLine(0.0f, spacing);
    ImGui::PushItemWidth(current_windows_size.x / 3.0f);
    ImGui::SliderInt("FPS", &SC.fps, 1, 200, "%d");

    ImGui::PopButtonRepeat();
    ImGui::SameLine();
    ImGui::Text(": %d", SC.fps);

    /*----------Auto play and Replay----------*/
    ImGui::Checkbox("Auto Play", &SC.auto_play);
    if (SC.auto_play && SC.update_frame && SC.frame < SC.max_frame) {
      SC.update_frame = true;
      ++SC.frame;
    }

    ImGui::SameLine();

    ImGui::Checkbox("Replay", &SC.replay);
    if (SC.replay && SC.update_frame && SC.frame >= SC.max_frame) {
      SC.update_frame = true;
      SC.frame = 0;
    }

    ImGui::TreePop();
  }
}

void ShowSimulation()
{
  FileHandler::load_weight(SC.weight_data_path, Model, normalizer);

  ImGui::SetNextWindowPos(ImVec2(550, 50), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);
  ImGui::Begin("Simulation Window");

  SC.check_auto_play();
  ShowSimulationInformation();
  SC.check_update_frame();

  if (ImGui::TreeNodeEx("Simulation window")) {
    if (ImPlot::BeginPlot("Simulation", ImVec2(static_cast<float>(SC.window_size), static_cast<float>(SC.window_size)))) {
      ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 1);
      ImPlot::SetupAxes("x", "y");
      ImPlot::SetupAxisLimits(ImAxis_X1, -5.0, 5.0);
      ImPlot::SetupAxisLimits(ImAxis_Y1, -10, 10);

      double order_using_xy = 0;
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 0, ImVec4(0, 0.7f, 0, 1), IMPLOT_AUTO, ImVec4(0, 0.7f, 0, 1));
      ImPlot::PlotScatter("Normal Point", &order_using_xy, &order_using_xy, 1);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 0, ImVec4(1, 0, 0, 1), IMPLOT_AUTO, ImVec4(1, 0, 0, 1));
      ImPlot::PlotScatter("Target Segment", &order_using_xy, &order_using_xy, 1);

      Eigen::MatrixXd target_feature_matrix = normalizer.transform(SC.feature_matrix);
      Eigen::VectorXd pred_Y = Model.predict(target_feature_matrix);

      for (int i = 0; i < SC.segment_vec.size(); ++i) {
        Eigen::ArrayXd segment_x_data = SC.segment_vec[i].col(0).array();
        Eigen::ArrayXd segment_y_data = SC.segment_vec[i].col(1).array();

        double *segment_x = segment_x_data.data();
        double *segment_y = segment_y_data.data();

        if (pred_Y(i) == 1) {
          ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 1, ImVec4(1, 0, 0, 1), IMPLOT_AUTO, ImVec4(1, 0, 0, 1));
          ImPlot::PlotScatter("Target Section", segment_x, segment_y, segment_x_data.size());

          SC.Target_X = segment_x_data.mean();
          SC.Target_Y = segment_y_data.mean();
        }
        else {
          ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 1, color_arr[i % 9], IMPLOT_AUTO, color_arr[i % 9]);
          ImPlot::PlotScatter("Normal Point", segment_x, segment_y, segment_x_data.size());
        }
      }

      ImPlot::PopStyleVar();
      ImPlot::EndPlot();
    }    // end ImPlot::BeginPlot("Simulation", ...)

    ImGui::TreePop();
  }

  ImGui::End();
}