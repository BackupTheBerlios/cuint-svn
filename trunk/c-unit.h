/* Minimal unit testing for C
   Version 0.2

Usage:
- Write you test function in the format void test_func();
- Sprinkle c_unit_assert(value, msg) in your test functions
- Add desired tests using c_unit_add(test, name)
- Run the tests using c_unit_run()

Example:
    #include "c-unit.h"

    void
    test_ok()
    {
        c_unit_assert(1, "this one is OK");
    }

    void
    test_fail()
    {
        c_unit_assert(0, "this one fails");
    }

    int
    main()
    {
        c_unit_add(test_ok, "test_ok");
        c_unit_add(test_fail, "test_fail");

        return c_unit_run() != 1;
    }

It is also recommened to use the following scheme in your source files:

    static int
    do_something()
    {
        printf("working...\n");

        return 1;
    }

    #ifdef TESTING
    #include "tests/test_t.c"
    #endif

tests/test_t.c will contain the "main" function that adds this
modules test functions and calls main.
*/

/*
=====================================================
Copyright (c) Miki Tebeka <miki.tebeka@google.com> 
This file is under the GNU Public License (GPL), see
http://www.gnu.org/copyleft/gpl.html for more details
=====================================================

$Id: c-unit.h 972 2004-11-07 06:42:51Z mikit $
*/
#ifndef C_UNIT_H
#define C_UNIT_H

#include <stdio.h>
#include <setjmp.h>
#include <stdarg.h>

static int __c_unit_curr_test; /**< Current test */
static jmp_buf __c_unit_err; /**< longjmp target */
#define __C_UNIT_MAX_TESTS 100 /**< Maximal number of tests */
typedef void (*__c_unit_test_func)(); /**< Test function type */

/** Test structure */
typedef struct {
	__c_unit_test_func test; /**< Test function */
	char *name; /**< Test name */
} __c_unit_test_t;

__c_unit_test_t __c_unit_tests[__C_UNIT_MAX_TESTS]; /**< Array of tests */
static int __c_unit_num_tests = 0; /**< Number of tests */

/** Add a test to the test suite
  @param test Test function
  @param name Test name
*/
static void
c_unit_add(__c_unit_test_func test, char *name)
{
	__c_unit_tests[__c_unit_num_tests].test = test;
	__c_unit_tests[__c_unit_num_tests].name = name;
	++__c_unit_num_tests;
}

/** Assert a test value
  @param value Assert value 
  @param format "printf" format
  @param ... Rest of arguments

  Modifies __c_unit_ok
*/
static void
c_unit_assert(int value, char *format, ...)
{
	char *name; /* Name of test */
    va_list argptr; /* Argument pointer */

    if (value) {
        return;
    }
	
    /* Test name */
    name = __c_unit_tests[__c_unit_curr_test].name;
    printf("%s: error: ", name);
    va_start(argptr, format);
    vprintf(format, argptr);
    va_end(argptr);

    printf("\n");
    fflush(stdout);

    longjmp(__c_unit_err, 1);
}

/** Run test suite. Print progress and summary
  @return 0 if any test failed, 1 otherwise

  Modifies __c_unit_ok, __c_unit_curr_test
*/
static int
c_unit_run()
{
	int success, fail;

	success = fail = 0;

	printf("Running %d tests...\n", __c_unit_num_tests);
	for (__c_unit_curr_test = 0; 
		 __c_unit_curr_test < __c_unit_num_tests;
		 ++__c_unit_curr_test) {
		printf("* %s:\n", __c_unit_tests[__c_unit_curr_test].name);
        if (setjmp(__c_unit_err)) {
            ++fail;
			printf("Test FAILED\n\n");
            continue;
        }
		__c_unit_tests[__c_unit_curr_test].test();
        ++success;
        printf("Test OK\n\n");
	}

	printf("Total of %d tests passed, %d failed\n", success, fail);

	return (fail == 0);
}

#endif /* C_UNIT_H */
