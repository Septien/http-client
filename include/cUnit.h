/**
 * @file cUnit.h
 * @author Jose Antonio Septien Hernandez (ja.septienhernandez@ugto.mx)
 * @brief This is a simple framework for unittests in C, that is intended to run both on Linux and RIOT OS.
 * @version 0.1
 * @date 2022-05-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef C_UNIT_H
#define C_UNIT_H

#include <string.h>
#include <stdbool.h>

#ifndef FUNCTION_NAME_SIZE
#define FUNCTION_NAME_SIZE  80
#endif

typedef struct Test
{
    char        function_name[FUNCTION_NAME_SIZE];
    bool        (*test)(void *);
    struct Test *next;
}Test_t;


typedef struct
{
    void    (*setup)(void *);
    void    (*teardown)(void *);
    void    *data;
    Test_t  *head;
    Test_t  *last;
    int    tests_passed;
    int    tests_failed;
}cUnit_t;

/**
 * @brief Initialization of the cUnit_t data structure. It receives the setup,
 * teardown function, and the data for the set of tests, and allocates the
 * resources needed.
 * 
 * @param setup 
 * @param teardown 
 * @param data 
 */
void cunit_init(cUnit_t **cUnit, void (*setup)(void *), void (*teardown)(void *), void *data);

/**
 * @brief Releases all the resourcess used.
 * 
 */
void cunit_terminate(cUnit_t **cUnit);

/**
 * @brief Add a test to the list.
 * 
 * @param function_name 
 * @param test 
 */
void cunit_add_test(cUnit_t *cUnit, bool (*test)(void *), char *function_name);

/**
 * @brief Execute all the allocated tests.
 * 
 */
void cunit_execute_tests(cUnit_t *cUnit);

/**
 * @brief Check whether the condition @p condition holds. If not, appends
 * @p condition_name to @p str, for later print and debugging information.
 * 
 * @param old_value         previous value of the variable.
 * @param condition         value to update condition.
 * @param condition_name    Description of the condition to evaluate.
 * @param str               String to append the description of the failure.
 * @return @p old_value && @p condition
 */
bool check_condition(bool old_value, bool condition, char *condition_name, char *str);

#endif      // C_UNIT_H
