/**
 * @file show_label_window.cpp
 * @author Mes (mes900903@gmail.com) (Discord: Mes#0903)
 * @brief The implementation of Label GUI.
 * @version 0.1
 * @date 2023-01-24
 *
 * @copyright Copyright (c) 2023 Mes
 *
 */

#include "imgui_header.h"
#include "show_label_window.h"
#include "LabelController.h"

#include "make_feature.h"
#include "adaboost.h"
#include "logistic.h"
#include "normalize.h"
#include "file_handler.h"

#include <Eigen/Eigen>
#include <chrono>
#include <vector>
#include <algorithm>
#include <fstream>
#include <filesystem>

static LabelController LC;

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
 * @brief The window controled detail member in Label Controller
 */
void ShowLabelInformation()
{
  if (ImGui::TreeNodeEx("Label Information")) {
    /*----------Clearing Label Data----------*/
    if (ImGui::Button("Clearing Data"))
      ImGui::OpenPopup("Clean?");

    ImGui::SameLine();
    ImGui::Text("This will clear the data you have wrote!");

    if (ImGui::BeginPopupModal("Clean?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::Text("Are you sure to clean up the data?\nAll of the data you wrote will be cleaned.\n\n");
      ImGui::Separator();

      if (ImGui::Button("OK", ImVec2(200, 0))) {
        LC.clean_data = true;
        ImGui::CloseCurrentPopup();
      }

      ImGui::SetItemDefaultFocus();
      ImGui::SameLine();
      if (ImGui::Button("Cancel", ImVec2(200, 0))) { ImGui::CloseCurrentPopup(); }
      ImGui::EndPopup();
    }

    /*----------Load raw data----------*/
    if (ImGui::Button("Load raw data"))
      ImGuiFileDialog::Instance()->OpenDialog("LoadLabelRawData", "Choose your raw data", ".*", FileHandler::get_filepath() + "\\");

    ImGui::SameLine();
    ImGui::Text("path: %s", LC.raw_data_path.c_str());
    // display
    if (ImGuiFileDialog::Instance()->Display("LoadLabelRawData", ImGuiWindowFlags_NoCollapse, ImVec2(600, 500))) {
      // action if OK
      if (ImGuiFileDialog::Instance()->IsOk()) {
        LC.auto_play = false;
        LC.replay = false;
        LC.load_data = true;

        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
        std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

        if (filePath != LC.raw_data_path)
          LC.raw_data_path = filePathName;
      }

      ImGuiFileDialog::Instance()->Close();
    }

    /*----------Load feature output data----------*/
    if (ImGui::Button("Load feature output data"))
      ImGuiFileDialog::Instance()->OpenDialog("LoadLabelFeatureData", "Choose the feature data you wanna write to", ".*", FileHandler::get_filepath() + "\\");

    ImGui::SameLine();
    ImGui::Text("path: %s", LC.feature_output_path.c_str());

    // display
    if (ImGuiFileDialog::Instance()->Display("LoadLabelFeatureData", ImGuiWindowFlags_NoCollapse, ImVec2(600, 500))) {
      // action if OK
      if (ImGuiFileDialog::Instance()->IsOk()) {
        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
        std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
        LC.feature_output_path = filePathName;
      }

      ImGuiFileDialog::Instance()->Close();
    }

    /*----------Load label output data----------*/
    if (ImGui::Button("Load label output data"))
      ImGuiFileDialog::Instance()->OpenDialog("LoadLabelLabelData", "Choose the label data you wanna write to", ".*", FileHandler::get_filepath() + "\\");

    ImGui::SameLine();
    ImGui::Text("path: %s", LC.label_output_path.c_str());

    // display
    if (ImGuiFileDialog::Instance()->Display("LoadLabelLabelData", ImGuiWindowFlags_NoCollapse, ImVec2(600, 500))) {
      // action if OK
      if (ImGuiFileDialog::Instance()->IsOk()) {
        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
        std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
        LC.label_output_path = filePathName;
      }

      ImGuiFileDialog::Instance()->Close();
    }

    /*----------Robot HZ----------*/
    ImVec2 current_windows_size = ImGui::GetWindowSize();
    static int current_HZ = LC.HZ;

    if (ImGui::Button("720")) {
      LC.HZ = 720;

      if (current_HZ != LC.HZ) {
        current_HZ = LC.HZ;
        LC.max_frame = static_cast<int>(LC.max_frame / 2.0);    // 360->720
      }

      LC.xy_data = Eigen::MatrixXd::Zero(LC.HZ, 2);
      LC.frame = 0;
      LC.update_frame = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("360")) {
      LC.HZ = 360;

      if (current_HZ != LC.HZ) {
        current_HZ = LC.HZ;
        LC.max_frame *= 2;    // 720->360
      }

      LC.xy_data = Eigen::MatrixXd::Zero(LC.HZ, 2);
      LC.frame = 0;
      LC.update_frame = true;
    }
    ImGui::SameLine();

    ImGui::SameLine();
    ImGui::Text("HZ:%d", LC.HZ);

    /*----------Label Window Size----------*/
    ImGui::Text("Label Window Size Control:");
    ImGui::SameLine();
    ImGui::PushButtonRepeat(true);

    if (ImGui::ArrowButton("label_window_size_left", ImGuiDir_Left) && LC.window_size > 100)
      --LC.window_size;

    float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    ImGui::SameLine(0.0f, spacing);
    if (ImGui::ArrowButton("label_window_size_right", ImGuiDir_Right) && LC.window_size < 2000)
      ++LC.window_size;

    ImGui::PopButtonRepeat();
    ImGui::SameLine(0.0f, spacing);
    ImGui::PushItemWidth(current_windows_size.x / 3.0f);
    ImGui::SliderInt("Label Window size", &LC.window_size, 100, 2000, "%d");
    ImGui::SameLine();
    ImGui::Text(":%d", LC.window_size);

    /*----------Label Mouse Area Control----------*/
    ImGui::Text("Mouse Area Control:");
    ImGui::SameLine();
    ImGui::PushButtonRepeat(true);

    if (ImGui::ArrowButton("label_mouse_area_left", ImGuiDir_Left) && !LC.auto_play && LC.label_mouse_area > 0) {
      LC.update_frame = true;
      LC.label_mouse_area -= static_cast<float>(0.01);
    }

    ImGui::SameLine(0.0f, spacing);
    if (ImGui::ArrowButton("label_mouse_area_right", ImGuiDir_Right) && !LC.auto_play && LC.label_mouse_area < 5) {
      LC.update_frame = true;
      LC.label_mouse_area += static_cast<float>(0.01);
    }

    ImGui::PopButtonRepeat();
    ImGui::SameLine(0.0f, spacing);
    ImGui::PushItemWidth(current_windows_size.x / 3.0f);
    ImGui::SliderFloat("Label Mouse Area", &LC.label_mouse_area, 0, 5, "%0.01f");

    ImGui::SameLine();
    ImGui::Text(":%f", LC.label_mouse_area);

    /*----------Frame Control----------*/
    ImGui::Text("Max Frame: %d", LC.max_frame);
    ImGui::Text("Writed Max Frame: %d", LC.writed_max_frame);
    ImGui::Text("Writed Frame Numbers: %d", LC.writed_frame_numbers);

    ImGui::Text("Frame Control:");
    ImGui::SameLine();

    ImGui::PushButtonRepeat(true);

    if ((ImGui::ArrowButton("frame_left", ImGuiDir_Left) || ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) && !LC.auto_play && LC.frame > 0) {
      LC.update_frame = true;
      --LC.frame;
    }

    ImGui::SameLine(0.0f, spacing);
    if ((ImGui::ArrowButton("frame_right", ImGuiDir_Right) || ImGui::IsKeyPressed(ImGuiKey_RightArrow)) && !LC.auto_play && LC.frame < LC.max_frame) {
      LC.update_frame = true;
      ++LC.frame;
    }

    ImGui::PopButtonRepeat();
    ImGui::SameLine(0.0f, spacing);
    ImGui::PushItemWidth(current_windows_size.x / 3.0f);
    if (ImGui::SliderInt("Frame", &LC.frame, 0, LC.max_frame, "%d"))
      LC.update_frame = true;

    ImGui::SameLine();
    ImGui::Text(":%d", LC.frame);

    /*----------FPS Control----------*/
    ImGui::Text("FPS Control:");
    ImGui::SameLine();
    ImGui::PushButtonRepeat(true);

    if (ImGui::ArrowButton("fps_left", ImGuiDir_Left) && LC.fps > 1)
      --LC.fps;

    ImGui::SameLine(0.0f, spacing);
    if (ImGui::ArrowButton("fps_right", ImGuiDir_Right) && LC.fps < 200)
      ++LC.fps;

    ImGui::SameLine(0.0f, spacing);
    ImGui::PushItemWidth(current_windows_size.x / 3.0f);
    ImGui::SliderInt("FPS", &LC.fps, 1, 200, "%d");

    ImGui::PopButtonRepeat();
    ImGui::SameLine();
    ImGui::Text(":%d", LC.fps);

    /*----------Auto play and Replay----------*/
    ImGui::Checkbox("Auto Play", &LC.auto_play);
    if (LC.auto_play && LC.update_frame && LC.frame < LC.max_frame) {
      LC.update_frame = true;
      ++LC.frame;
    }

    ImGui::SameLine();

    ImGui::Checkbox("Replay", &LC.replay);
    if (LC.replay && LC.update_frame && LC.frame >= LC.max_frame) {
      LC.update_frame = true;
      LC.frame = 0;
    }

    /*----------Save Label Control----------*/
    ImGui::Checkbox("Enable Enter Key for Saving File", &LC.enable_enter_save);
    if (ImGui::Button("Save Label") ||
        ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) && ImGui::IsKeyDown(ImGuiKey_S)) ||
        ((ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter)) && LC.enable_enter_save)) {
      // avoid save too frequently
      if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - LC.current_save_time).count() > 100) {
        LC.current_save_time = std::chrono::system_clock::now();
        LC.save_label = true;
        LC.current_save_frame = LC.frame;
      }
    }

    if (LC.current_save_frame != -1) {
      ImGui::SameLine();
      ImGui::Text("Save Label data from Frame: %d", LC.current_save_frame);
    }

    /*----------Output txt file Control----------*/
    if (ImGui::Button("Output File")) {
      LC.output_feature_data();
      LC.output_label_data();
    }

    /*----------Show Label Rect and Auto Label----------*/
    ImGui::Checkbox("Show Label Rect", &LC.show_rect);

    ImGui::SameLine();
    ImGui::Checkbox("Show Nearest Segment", &LC.show_nearest);

    ImGui::SameLine();
    ImGui::Checkbox("Auto Label", &LC.auto_label);

    /*----------------------------------------*/
    ImGui::TreePop();
  }
}

/**
 * @brief The main function of Label GUI
 *
 */
void ShowLabel()
{
  ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);
  ImGui::Begin("Label Window");

  LC.check_auto_play();
  ShowLabelInformation();
  LC.check_clean_data();
  LC.check_load_data();
  LC.check_update_frame();

  // draw point
  if (ImGui::TreeNodeEx("Label window")) {
    if (ImPlot::BeginPlot("Label", ImVec2(static_cast<float>(LC.window_size), static_cast<float>(LC.window_size)))) {
      ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 1);
      ImPlot::SetupAxes("x", "y");
      ImPlot::SetupAxisLimits(ImAxis_X1, -5.0, 5.0);
      ImPlot::SetupAxisLimits(ImAxis_Y1, -10, 10);

      double order_using_xy = 0;
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 0, ImVec4(0, 0.7f, 0, 1), IMPLOT_AUTO, ImVec4(0, 0.7f, 0, 1));
      ImPlot::PlotScatter("Normal Point", &order_using_xy, &order_using_xy, 1);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 0, ImVec4(1, 0, 0, 1), IMPLOT_AUTO, ImVec4(1, 0, 0, 1));
      ImPlot::PlotScatter("Target Segment", &order_using_xy, &order_using_xy, 1);

      // the rectangle for label using
      static ImPlotRect rect(-1, 1, -1, 1);
      if (LC.show_rect)
        ImPlot::DragRect(0, &rect.X.Min, &rect.Y.Min, &rect.X.Max, &rect.Y.Max, ImVec4(1, 0, 1, 1), ImPlotDragToolFlags_None);

      int nearest_index = -1;
      double nearest_dis = -1;
      double nearest_x[2] = {};
      double nearest_y[2] = {};

      // iterate through the all segments in one frame
      for (int i = 0; i < LC.segment_vec.size(); ++i) {
        const Eigen::MatrixXd &segment = LC.segment_vec[i];    // the i-th segment

        Eigen::ArrayXd segment_x = segment.col(0).array();
        Eigen::ArrayXd segment_y = segment.col(1).array();
        double X_mean = segment_x.mean();
        double Y_mean = segment_y.mean();
        int segment_size = segment_x.size();

        if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(0)) {
          ImPlotPoint click_point = ImPlot::GetPlotMousePos();

          for (auto data_point : segment.rowwise()) {
            // click the point for labeling
            if ((data_point(0) - LC.label_mouse_area < click_point.x) && (click_point.x < data_point(0) + LC.label_mouse_area) && (data_point(1) - LC.label_mouse_area < click_point.y) && (click_point.y < data_point(1) + LC.label_mouse_area)) {
              if (LC.segment_label[i] == 1)
                LC.segment_label[i] = 0;    // 1 -> 0 (disable the label)
              else
                LC.segment_label[i] = 1;    // 0 -> 1 (label the point was not labeled)

              break;
            }
          }
        }

        // for rectangle label, label the point in the rectangle
        if (LC.show_rect && LC.auto_label) {
          for (auto data_point : segment.rowwise()) {
            if ((rect.X.Min < data_point(0) && data_point(0) < rect.X.Max) && (rect.Y.Min < data_point(1) && data_point(1) < rect.Y.Max)) {
              LC.segment_label[i] = 1;

              break;
            }
          }
        }

        // find the nearest point in this frame
        if (LC.show_nearest) {
          double point_dis = std::sqrt(std::pow(X_mean, 2) + std::pow(Y_mean, 2));
          if (point_dis < nearest_dis || nearest_dis == -1) {
            nearest_dis = point_dis;
            nearest_index = i;
            nearest_x[1] = X_mean;
            nearest_y[1] = Y_mean;
          }
        }

        // plot the point red if it was labeled, otherwise choose a color in the color_arr
        if (LC.segment_label[i] == 1) {
          ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 1, ImVec4(1, 0, 0, 1), IMPLOT_AUTO, ImVec4(1, 0, 0, 1));
          ImPlot::PlotScatter("Target Segment", segment_x.data(), segment_y.data(), segment_size);
        }
        else {
          ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 1, color_arr[i % 9], IMPLOT_AUTO, color_arr[i % 9]);
          ImPlot::PlotScatter("Normal Point", segment_x.data(), segment_y.data(), segment_size);
        }
      }

      // for nearest label, label the point nearest (0, 0)
      if (LC.show_nearest) {
        if (LC.auto_label) {
          LC.segment_label[nearest_index] = 1;
          Eigen::ArrayXd segment_x = LC.segment_vec[nearest_index].col(0).array();
          Eigen::ArrayXd segment_y = LC.segment_vec[nearest_index].col(1).array();
          int segment_size = segment_x.size();

          // plot the nearest segment red (it will cover the original color)
          ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 1, ImVec4(1, 0, 0, 1), IMPLOT_AUTO, ImVec4(1, 0, 0, 1));
          ImPlot::PlotScatter("Target Segment", segment_x.data(), segment_y.data(), segment_size);
        }

        // plot the line connect to the nearest segment
        ImPlot::SetNextLineStyle(ImVec4(1, 0, 0, 1));
        ImPlot::PlotLine("nearest_line", nearest_x, nearest_y, 2);
      }

      // check if it needs to save the data
      LC.check_save_data();

      ImPlot::PopStyleVar();
      ImPlot::EndPlot();
    }

    ImGui::TreePop();
  }

  ImGui::End();
}