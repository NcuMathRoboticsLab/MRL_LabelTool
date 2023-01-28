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

  if (raw_bin_open__)
    raw_bin_file__.close();
  else
    raw_bin_open__ = true;


  int r_buf[360] = {};
  {
    std::ifstream infile(raw_data_path);
    if (infile.fail()) {
      std::cerr << "cant found " << raw_data_path << '\n';
      std::cin.get();
      exit(1);
    }

    std::ofstream outfile(raw_bin_path__, std::ios::binary, std::ios::trunc);
    if (outfile.fail()) {
      std::cerr << "cant found " << raw_bin_path__ << '\n';
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

      if (theta_cnt < 360)
        r_buf[theta_cnt++] = static_cast<int>(x * 10);

      outfile.write(reinterpret_cast<char *>(&x), sizeof(double));
      outfile.write(reinterpret_cast<char *>(&y), sizeof(double));

      ss.str("");
      ss.clear();
    }
  }

  int theta1 = r_buf[0], theta2 = r_buf[1];
  bool rtheta_data = true;
  if (!(theta2 - theta1 == 5 || theta2 - theta1 == 10))
    rtheta_data = false;

  for (int i = 2; i < 360; ++i) {
    theta1 = theta2;
    theta2 = r_buf[i];

    if (!(theta2 - theta1 == 5 || theta2 - theta1 == 10)) {
      rtheta_data = false;
      break;
    }
  }
  is_xydata = !rtheta_data;

  max_frame /= HZ;
  --max_frame;

  raw_bin_file__.open(raw_bin_path__, std::ios::in | std::ios::binary);
  if (raw_bin_file__.fail()) {
    std::cerr << "cant open " << raw_bin_path__ << '\n';
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
  raw_bin_file__.seekg(frame * sizeof(double) * 2 * HZ, std::ios::beg);

  for (int i{}; i < HZ; ++i) {
    raw_bin_file__.read(reinterpret_cast<char *>(&xy_data(i, 0)), sizeof(double));
    raw_bin_file__.read(reinterpret_cast<char *>(&xy_data(i, 1)), sizeof(double));
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
        std::chrono::duration_cast<std::chrono::milliseconds>(now - current_time__).count() > (1000 / fps)) {
      current_time__ = now;
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
  HZ = 720;
  frame = 0, max_frame = 0;
  window_size = 750;

  update_frame = true;
  current_time__ = std::chrono::system_clock::now();

  auto_play = false;
  replay = false;

  xy_data = Eigen::MatrixXd::Zero(HZ, 2);
  raw_bin_open__ = false;
  is_xydata = false;
}