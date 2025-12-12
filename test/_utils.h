#ifndef _TEST_UTILS_H
#define _TEST_UTILS_H



#define GREEN "\033[0;32m"
#define RED   "\033[0;31m"
#define CYAN  "\x1b[36m"
#define RESET "\033[0m"

#define TEST_MSG printf("%s: " CYAN "%s" RESET "... ", __FILE__, __func__);
#define PASSED   printf(GREEN "passed\n" RESET)



#endif
