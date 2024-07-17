#include <stdlib.h>
#include <stdio.h>
#include "assert.h"

#include "cUnit.h"

#ifdef __RIOT__
#include "malloc.h"
#endif

void cunit_init(cUnit_t **cUnit, void (*setup)(void *), void (*teardown)(void *), void *data)
{
    assert(cUnit != NULL);
    assert(setup != NULL);
    assert(teardown != NULL);
    assert(data != NULL);

    // Allocate memory
    *cUnit = (cUnit_t *)malloc(sizeof(cUnit_t));

    // Store the setup and teardown functions.
    (*cUnit)->setup = setup;
    (*cUnit)->teardown = teardown;
    (*cUnit)->data = data;
    (*cUnit)->head = NULL;
    (*cUnit)->last = NULL;
    (*cUnit)->tests_passed = 0;
    (*cUnit)->tests_failed = 0;
}

void cunit_terminate(cUnit_t **cUnit)
{
    assert(cUnit != NULL);
    assert(*cUnit != NULL);
    
    (*cUnit)->setup = NULL;
    (*cUnit)->teardown = NULL;
    (*cUnit)->data = NULL;


    for (Test_t *head = (*cUnit)->head; head != NULL; )
    {
        Test_t *next = NULL;
        if (head != NULL)
        {
            next = head->next;
        }
        free(head);
        head = next;
    }

    free(*cUnit);
    *cUnit = NULL;
}

void cunit_add_test(cUnit_t *cUnit, bool (*test)(void *), char *function_name)
{
    assert(cUnit != NULL);
    assert(test != NULL);
    assert(function_name != NULL);

    Test_t *new_test = (Test_t *)malloc(sizeof(Test_t));
    if (new_test == NULL)
        return;
    
    strcpy(new_test->function_name, function_name);
    new_test->test = test;
    new_test->next = NULL;
    
    // No tests added
    if (cUnit->head == NULL)
    {
        cUnit->head = new_test;
        cUnit->last = new_test;
    }
    else        // Append at end of list
    {
        cUnit->last->next = new_test;
        cUnit->last = cUnit->last->next;
    }
}

void cunit_execute_tests(cUnit_t *cUnit)
{
    assert(cUnit != NULL);
    assert(cUnit->setup != NULL);
    assert(cUnit->teardown != NULL);
    assert(cUnit->data != NULL);
    assert(cUnit->head != NULL);

    for (Test_t *test = cUnit->head; test != NULL; test = test->next)
    {
        printf("\033[0;30mTesting %s\033[0m.\n", test->function_name);
        cUnit->setup(cUnit->data);
        bool value = test->test(cUnit->data);
        cUnit->teardown(cUnit->data);
        if (value) {
            printf("\033[0;32mTest passed\033[0m.\n");
            cUnit->tests_passed++;
        }
        else {
            printf("\033[0;31mTest failed\033[0m.\n");
            cUnit->tests_failed++;
        }
    }
    printf("\n\nFrom a total of \033[0;30m %d tests\033[0m, \033[0;32m%d passed\033[0m, and \033[0;31m%d failed\033[0m.\n",
            cUnit->tests_passed + cUnit->tests_failed, cUnit->tests_passed, cUnit->tests_failed);
}

bool check_condition(bool old_value, bool condition, char *condition_name, char *str)
{
    assert(str != NULL);
    bool return_value = old_value && condition;

    if (!condition) {
        strcat(str, condition_name);
        strcat(str, " failed.\n");
    }

    return return_value;
}
