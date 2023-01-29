/**
 * @file show_control_window.cpp
 * @author Mes (mes900903@gmail.com) (Discord: Mes#0903)
 * @brief Show the control window, which can show the label window and simulation window
 * @version 0.1
 * @date 2023-01-24
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "imgui_header.h"
#include "show_control_window.h"

#include <iostream>

/**
 * @brief Show the control window, which can show the label window and simulation window
 *
 * @param show_label_window If true, show the label window
 * @param show_simulation_window If true, show the simulation window
 */
void ShowControlWindow(bool &show_label_window, bool &show_simulation_window)
{
  ImGui::Begin("Control window");
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::Checkbox("Show Label window", &show_label_window);
  ImGui::Checkbox("Show Simulation window", &show_simulation_window);
  ImGui::End();
}