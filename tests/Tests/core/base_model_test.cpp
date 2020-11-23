#include <gtest/gtest.h>

#include "include/core/base_model.h"
// #include "src/core/base_model.cpp"
#include <Eigen/Dense>

class base_model_fixture : public ::testing::Test {
  public:
    std::vector<int> sub_compartments = {5, 1, 13, 1, 1};
    std::vector<float> residence_times = {3, 3, 8, 5};
    std::vector<float> empty = {};
    Eigen::VectorXf initial_states;
};

TEST_F(base_model_fixture, static_members) {
    BaseModel *base_model = new BaseModel;
    // EXPECT_EQ(base_model->tau_, empty);
    EXPECT_EQ(BaseModel::sub_compartments, sub_compartments);
}

TEST(base_model, constructor) { EXPECT_TRUE((std::is_default_constructible_v<BaseModel>)); }

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
