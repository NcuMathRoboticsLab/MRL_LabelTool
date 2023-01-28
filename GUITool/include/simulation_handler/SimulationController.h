#ifndef LABEL_CONTROLLER_H__
#define LABEL_CONTROLLER_H__

#include "Controller.h"

struct SimulationController : public AnimationController {
public:
  void check_update_frame() override;

  SimulationController();

public:
  std::string weight_data_path;
  double Target_X, Target_Y;
};

#endif