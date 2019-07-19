/**
 * @file    testDoOptimize.cpp
 * @brief   Unit test for pcm and optimize conditions
 * @author  Yun Chang
 */

#include <CppUnitLite/TestHarness.h>
#include <random>
#include <memory>

#include "RobustPGO/RobustSolver.h"
#include "RobustPGO/outlier/pcm.h"
#include "RobustPGO/SolverParams.h"
#include "test_config.h"

using namespace RobustPGO;

/* ************************************************************************* */
TEST(RobustSolver, Load1)
{
  // load graph
  // read g2o file for robot a
  gtsam::NonlinearFactorGraph::shared_ptr nfg;
  gtsam::Values::shared_ptr values;
  boost::tie(nfg, values) = gtsam::load3D(std::string(DATASET_PATH) + "/robot_a.g2o");

  // set up RobustPGO solver
  RobustSolverParams params;
  params.setPcm3DParams(0.0, 10.0, Verbosity::QUIET);

  std::unique_ptr<RobustSolver> pgo = std::make_unique<RobustSolver>(params);

  // Create prior
  static const gtsam::SharedNoiseModel& noise =
      gtsam::noiseModel::Isotropic::Variance(6, 0.01);

  gtsam::Key init_key = gtsam::Symbol('a', 0);
  gtsam::PriorFactor<gtsam::Pose3> init(init_key, values->at<gtsam::Pose3>(init_key), noise);

  // Load graph using prior
  pgo->loadGraph(*nfg, *values, init);

  gtsam::NonlinearFactorGraph nfg_out = pgo->getFactorsUnsafe();
  gtsam::Values values_out = pgo->calculateEstimate();

  // Since odom check threshold is 0, should only have the odom edges + prior (no lc should have passed)
  EXPECT(nfg_out.size()==size_t(50));
  EXPECT(values_out.size()==size_t(50));
}

/* ************************************************************************* */
TEST(RobustSolver, Add1)
{
  // load graph for robot a (same as above)
  gtsam::NonlinearFactorGraph::shared_ptr nfg;
  gtsam::Values::shared_ptr values;
  boost::tie(nfg, values) = gtsam::load3D(std::string(DATASET_PATH) + "/robot_a.g2o");

  // set up RobustPGO solver
  RobustSolverParams params;
  params.setPcm3DParams(0.0, 10.0, Verbosity::QUIET);

  std::unique_ptr<RobustSolver> pgo = std::make_unique<RobustSolver>(params);

  static const gtsam::SharedNoiseModel& noise =
      gtsam::noiseModel::Isotropic::Variance(6, 0.01);

  gtsam::Key init_key = gtsam::Symbol('a', 0);
  gtsam::PriorFactor<gtsam::Pose3> init(init_key, values->at<gtsam::Pose3>(init_key), noise);
  pgo->loadGraph(*nfg, *values, init);// first load

  // add graph
  // read g2o file for robot b
  gtsam::NonlinearFactorGraph::shared_ptr nfg_b;
  gtsam::Values::shared_ptr values_b;
  boost::tie(nfg_b, values_b) = gtsam::load3D(std::string(DATASET_PATH) + "/robot_b.g2o");


  // create the between factor for connection
  gtsam::Key init_key_b = gtsam::Symbol('b', 0);
  gtsam::Pose3 transform_ab = values->at<gtsam::Pose3>(init_key).between(values_b->at<gtsam::Pose3>(init_key_b));
  gtsam::BetweenFactor<gtsam::Pose3> bridge(init_key, init_key_b, transform_ab, noise);

  // add graph
  pgo->addGraph(*nfg_b, *values_b, bridge);

  gtsam::NonlinearFactorGraph nfg_out = pgo->getFactorsUnsafe();
  gtsam::Values values_out = pgo->calculateEstimate();

  // Since odom check threshold is 0, should only have the odom edges + prior + between (no lc should have passed)
  EXPECT(nfg_out.size()==size_t(92));
  EXPECT(values_out.size()==size_t(92));
}

/* ************************************************************************* */
TEST(RobustSolver, Load2)
{
  // load graph
  // read g2o file for robot a
  gtsam::NonlinearFactorGraph::shared_ptr nfg;
  gtsam::Values::shared_ptr values;
  boost::tie(nfg, values) = gtsam::load3D(std::string(DATASET_PATH) + "/robot_a.g2o");

  // set up RobustPGO solver
  RobustSolverParams params;
  params.setPcm3DParams(100.0, 100.0, Verbosity::QUIET);

  std::unique_ptr<RobustSolver> pgo = std::make_unique<RobustSolver>(params);

  // Create prior
  static const gtsam::SharedNoiseModel& noise =
      gtsam::noiseModel::Isotropic::Variance(6, 0.01);

  gtsam::Key init_key = gtsam::Symbol('a', 0);
  gtsam::PriorFactor<gtsam::Pose3> init(init_key, values->at<gtsam::Pose3>(init_key), noise);

  // Load graph using prior
  pgo->loadGraph(*nfg, *values, init);

  gtsam::NonlinearFactorGraph nfg_out = pgo->getFactorsUnsafe();
  gtsam::Values values_out = pgo->calculateEstimate();

  // Since thresholds are high, should have all the edges
  EXPECT(nfg_out.size()==size_t(53));
  EXPECT(values_out.size()==size_t(50));
}

/* ************************************************************************* */
TEST(RobustSolver, Add2)
{
  // load graph for robot a (same as above)
  gtsam::NonlinearFactorGraph::shared_ptr nfg;
  gtsam::Values::shared_ptr values;
  boost::tie(nfg, values) = gtsam::load3D(std::string(DATASET_PATH) + "/robot_a.g2o");

  // set up RobustPGO solver
  RobustSolverParams params;
  params.setPcm3DParams(100.0, 100.0, Verbosity::QUIET);

  std::unique_ptr<RobustSolver> pgo = std::make_unique<RobustSolver>(params);

  static const gtsam::SharedNoiseModel& noise =
      gtsam::noiseModel::Isotropic::Variance(6, 0.01);

  gtsam::Key init_key = gtsam::Symbol('a', 0);
  gtsam::PriorFactor<gtsam::Pose3> init(init_key, values->at<gtsam::Pose3>(init_key), noise);
  pgo->loadGraph(*nfg, *values, init);// first load

  // add graph
  // read g2o file for robot b
  gtsam::NonlinearFactorGraph::shared_ptr nfg_b;
  gtsam::Values::shared_ptr values_b;
  boost::tie(nfg_b, values_b) = gtsam::load3D(std::string(DATASET_PATH) + "/robot_b.g2o");

  // create the between factor for connection
  gtsam::Key init_key_b = gtsam::Symbol('b', 0);
  gtsam::Pose3 transform_ab = values->at<gtsam::Pose3>(init_key).between(values_b->at<gtsam::Pose3>(init_key_b));
  gtsam::BetweenFactor<gtsam::Pose3> bridge(init_key, init_key_b, transform_ab, noise);

  // add graph
  pgo->addGraph(*nfg_b, *values_b, bridge);

  gtsam::NonlinearFactorGraph nfg_out = pgo->getFactorsUnsafe();
  gtsam::Values values_out = pgo->calculateEstimate();

  // Since thresholds are high, should have all the edges
  EXPECT(nfg_out.size()==size_t(97));
  EXPECT(values_out.size()==size_t(92));
}

/* ************************************************************************* *
TEST(GenericSolver, Load)
{
  // load graph
  // read g2o file for robot a
  gtsam::NonlinearFactorGraph::shared_ptr nfg;
  gtsam::Values::shared_ptr values;
  boost::tie(nfg, values) = gtsam::load3D(std::string(DATASET_PATH) + "/robot_a.g2o");

  // set up GenericSolver
  std::unique_ptr<GenericSolver> pgo = std::make_unique<GenericSolver>();

  // Create prior
  static const gtsam::SharedNoiseModel& noise =
      gtsam::noiseModel::Isotropic::Variance(6, 0.01);

  gtsam::Key init_key = gtsam::Symbol('a', 0);
  gtsam::PriorFactor<gtsam::Pose3> init(init_key, values->at<gtsam::Pose3>(init_key), noise);

  // Load graph using prior
  pgo->loadGraph(*nfg, *values, init);

  gtsam::NonlinearFactorGraph nfg_out = pgo->getFactorsUnsafe();
  gtsam::Values values_out = pgo->calculateEstimate();

  // Since thresholds are high, should have all the edges
  EXPECT(nfg_out.size()==size_t(53));
  EXPECT(values_out.size()==size_t(50));
}

/* ************************************************************************* *
TEST(GenericSolver, Add)
{
  // load graph for robot a (same as above)
  gtsam::NonlinearFactorGraph::shared_ptr nfg;
  gtsam::Values::shared_ptr values;
  boost::tie(nfg, values) = gtsam::load3D(std::string(DATASET_PATH) + "/robot_a.g2o");

  // set up GenericSolver
  std::unique_ptr<GenericSolver> pgo = std::make_unique<GenericSolver>();

  static const gtsam::SharedNoiseModel& noise =
      gtsam::noiseModel::Isotropic::Variance(6, 0.01);

  gtsam::Key init_key = gtsam::Symbol('a', 0);
  gtsam::PriorFactor<gtsam::Pose3> init(init_key, values->at<gtsam::Pose3>(init_key), noise);
  pgo->loadGraph(*nfg, *values, init);// first load

  // add graph
  // read g2o file for robot b
  gtsam::NonlinearFactorGraph::shared_ptr nfg_b;
  gtsam::Values::shared_ptr values_b;
  boost::tie(nfg_b, values_b) = gtsam::load3D(std::string(DATASET_PATH) + "/robot_b.g2o");

  // create the between factor for connection
  gtsam::Key init_key_b = gtsam::Symbol('b', 0);
  gtsam::Pose3 transform_ab = values->at<gtsam::Pose3>(init_key).between(values_b->at<gtsam::Pose3>(init_key_b));
  gtsam::BetweenFactor<gtsam::Pose3> bridge(init_key, init_key_b, transform_ab, noise);

  // add graph
  pgo->addGraph(*nfg_b, *values_b, bridge);

  gtsam::NonlinearFactorGraph nfg_out = pgo->getFactorsUnsafe();
  gtsam::Values values_out = pgo->calculateEstimate();

  // Since thresholds are high, should have all the edges
  EXPECT(nfg_out.size()==size_t(97));
  EXPECT(values_out.size()==size_t(92));
}
/* ************************************************************************* */
  int main() { TestResult tr; return TestRegistry::runAllTests(tr);}
/* ************************************************************************* */