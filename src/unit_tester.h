/*
 *  Created on: 23 October, 2020
 *      Author: Lior Sinai
 * Description: Minimal custom unit test framework
 *
 * Sources
 * https://accu.org/journals/overload/10/52/crickett_368/
 */

#ifndef UNIT_TESTER_H
#define UNIT_TESTER_H

#include <iostream>
#include <string>
#include <list>

namespace TestTypes
{
    void isTrue(bool condition, const char *code, const char *file, size_t line);
    void isFalse(bool condition, const char *code, const char *file, size_t line);
    void assert_test(bool condition, const char *code, const char *file, size_t line);
};

#define IS_TRUE(exp) TestTypes::isTrue(exp, #exp, __FILE__, __LINE__)
#define IS_FALSE(exp) TestTypes::isFalse(exp, #exp, __FILE__, __LINE__)

class UnitTest
{
public:
    UnitTest(const std::string &name_) : name(name_) {}
    virtual void run() = 0;
    virtual bool setUp() { return true; };
    virtual std::string get_name()
    {
        return this->name;
    };
    virtual bool did_pass() { return this->passed; }

private:
    std::string name;
    bool passed;
};

class TestSuite
{
public:
    void addTest(UnitTest *test);
    void run();

private:
    int pass_count{0};
    int fail_count{0};
    int nTests{0};

    std::list<UnitTest *> tests_collection;
};

#endif //  UNIT_TESTER_H
