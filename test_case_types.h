
#ifndef TEST_CASE_TYPES_H
#define TEST_CASE_TYPES_H

#include <string>
#include <vector>

typedef struct test_result
{
    std::string test_date;
    std::string test_time;
    std::string test_outcome;
    bool valid = true;
}test_result;

typedef struct test
{
    std::string test_name;
    std::vector<test_result> results;
}test;

#endif // TEST_CASE_TYPES_H
