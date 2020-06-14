#include <gtest/gtest.h>
#include "version_lib.h"
#include "utility.h"




// Тест кейсы на проверку версий
TEST(version_test_case, version_test_patch)
{
    ASSERT_GE(version_patch(), 1);
}
TEST(version_test_case, version_test_minor)
{
    EXPECT_EQ(version_minor(), 1);
}
TEST(version_test_case, version_test_major)
{
    EXPECT_EQ(version_major(), 1);
}

// utility test
TEST(utility_test_case, norm_test)
{
    EXPECT_EQ(normolize::norm(normolize::min::latitude,normolize::max::latitude,-90), 0);
    EXPECT_EQ(normolize::norm(normolize::min::latitude,normolize::max::latitude,-1000), 0);

    EXPECT_EQ(normolize::norm(normolize::min::latitude,normolize::max::latitude,90), 1);
    EXPECT_EQ(normolize::norm(normolize::min::latitude,normolize::max::latitude,1000), 1);

    sample_type m;
    double f_min = -180;
    m(0) = normolize::norm(normolize::min::longitude,normolize::max::longitude,f_min);
    EXPECT_EQ(m(0), 0);
    f_min = -280;
    m(0) = normolize::norm(normolize::min::longitude,normolize::max::longitude,f_min);
    EXPECT_EQ(m(0), 0);
}

TEST(utility_test_case, convert_to_sample_test)
{
    auto f = split_data("180;90;1;0;1;0;10;10");
    auto a = normolize::convert_to_sample(f);

    EXPECT_EQ(a(0), normolize::norm(normolize::min::longitude,normolize::max::longitude,f(0)));
    EXPECT_EQ(a(1), normolize::norm(normolize::min::latitude,normolize::max::latitude,f(1)));
    EXPECT_EQ(a(2), normolize::norm(normolize::min::room_count,normolize::max::room_count,f(2)));
    EXPECT_EQ(a(3), normolize::norm(normolize::min::price,normolize::max::price,f(3)));
    EXPECT_EQ(a(4), normolize::norm(normolize::min::area,normolize::max::area,f(4)));
    EXPECT_EQ(a(5), normolize::norm(normolize::min::kitchen,normolize::max::kitchen,f(5)));
    EXPECT_EQ(a(6), 0);
}

TEST(utility_test_case, replace_missed_data_test)
{
    auto f = split_data(";;;;;;;");
    EXPECT_EQ(f(0),0);
    EXPECT_EQ(f(1),0);
    EXPECT_EQ(f(2), normolize::min::room_count);
    EXPECT_EQ(f(3), normolize::min::price);
    EXPECT_EQ(f(4), normolize::min::area);
    EXPECT_EQ(f(5), normolize::min::kitchen);
    EXPECT_EQ(f(6), normolize::min::floor);
    EXPECT_EQ(f(7), normolize::min::floor);
}

TEST(utility_test_case, get_distance_test)
{
    std::string request_str = "85;55;2;5000000.00;55.00;5.00;5;5";
    auto req  = split_data(request_str);
    auto one  = split_data(request_str);
    EXPECT_EQ(req,one);
    auto dist_1 = get_distance(normolize::convert_to_sample(req),normolize::convert_to_sample(one));
    EXPECT_EQ(dist_1,0);

    auto two  = split_data("86;56;3;5100000.00;56.00;6.00;5;5");
    auto dist_2 = get_distance(normolize::convert_to_sample(req),normolize::convert_to_sample(two));
    EXPECT_GE(dist_2,dist_1);

    auto three  = split_data("87;57;4;5200000.00;57.00;7.00;5;5");
    auto dist_3 = get_distance(normolize::convert_to_sample(req),normolize::convert_to_sample(three));
    EXPECT_GE(dist_3,dist_2);
}



int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}