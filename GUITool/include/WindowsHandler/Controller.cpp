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

#include "Controller.h"
#include "metric.h"
#include "file_handler.h"

/**
 * @brief Transform the raw data into binary data
 */
void AnimationController::transform_frame()
{
  max_frame = 0;

  // if the file has been open, it means it was going to load another file, so close it
  if (_raw_bin_open)
    _raw_bin_file.close();
  else
    _raw_bin_open = true;


  int r_buf[360] = {};
  {
    std::ifstream infile(raw_data_path);
    if (infile.fail()) {
      std::cerr << "cant found " << raw_data_path << '\n';
      std::cin.get();
      exit(1);
    }

    std::ofstream outfile(_raw_bin_path, std::ios::binary | std::ios::trunc);
    if (outfile.fail()) {
      std::cerr << "cant found " << _raw_bin_path << '\n';
      std::cin.get();
      exit(1);
    }

    std::string line;
    std::stringstream ss;
    double x, y;
    int theta_cnt = 0;

    while (std::getline(infile, line)) {
      ++max_frame;

      ss << line;
      ss >> x >> y;

      // buffer the first 360 line for checking if the data is rtheta data later
      if (theta_cnt < 360)
        r_buf[theta_cnt++] = static_cast<int>(x * 10);

      // write the binary data
      outfile.write(reinterpret_cast<char *>(&x), sizeof(double));
      outfile.write(reinterpret_cast<char *>(&y), sizeof(double));

      ss.str("");
      ss.clear();
    }
  }

  // check if the data is xy data or rtheta data, the theta difference of minibot and turtlebot was 0.5 and 1
  int theta1 = r_buf[0], theta2 = r_buf[1];
  bool rtheta_data = true;

  // check the first data
  if (!(theta2 - theta1 == 5 || theta2 - theta1 == 10))
    rtheta_data = false;

  // check all the remain data in the first 360 line
  for (int i = 2; i < 360; ++i) {
    theta1 = theta2;
    theta2 = r_buf[i];

    // if the theta difference is not 0.5 or 1, it means the data is xy data
    if (!(theta2 - theta1 == 5 || theta2 - theta1 == 10)) {
      rtheta_data = false;
      break;
    }
  }
  is_xydata = !rtheta_data;

  max_frame /= HZ;
  --max_frame;    // 0 ~ max_frame-1

  _raw_bin_file.open(_raw_bin_path, std::ios::in | std::ios::binary);
  if (_raw_bin_file.fail()) {
    std::cerr << "cant open " << _raw_bin_path << '\n';
    std::cin.get();
    exit(1);
  }
}

/**
 * @brief read one frame in laser data (minibot is 720*2, you can changed the HZ in control window or manually changed it in the constructor)
 *
 */
void AnimationController::read_frame()
{
  _raw_bin_file.seekg(frame * sizeof(double) * 2 * HZ, std::ios::beg);

  for (int i{}; i < HZ; ++i) {
    _raw_bin_file.read(reinterpret_cast<char *>(&xy_data(i, 0)), sizeof(double));
    _raw_bin_file.read(reinterpret_cast<char *>(&xy_data(i, 1)), sizeof(double));
  }

  if (!is_xydata)
    metric::rtheta_to_xy(xy_data, HZ);
}

/**
 * @brief Check if the GUI should auto play
 */
void AnimationController::check_auto_play()
{
  if (auto_play) {
    // check the fps and determine if it should update frame
    if (auto now = std::chrono::system_clock::now();
        std::chrono::duration_cast<std::chrono::milliseconds>(now - _current_time).count() > (1000 / fps)) {
      _current_time = now;
      update_frame = true;
    }
    else {
      update_frame = false;
    }
  }
}

AnimationController::AnimationController()
{
  fps = 60;
  frame = 0, max_frame = 0;
  window_size = 750;

  update_frame = true;
  _current_time = std::chrono::system_clock::now();

  auto_play = false;
  replay = false;

  _raw_bin_open = false;
  is_xydata = false;
}