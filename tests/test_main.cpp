#include <gtest/gtest.h>

// Test main runner
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    // Run all tests
    int result = RUN_ALL_TESTS();
    
    return result;
}
