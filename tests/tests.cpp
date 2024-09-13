#include "base/base_inc.h"
#include "base/base_inc.cpp"

static U32 g_failed_tests = 0;

#define TEST_CHECK(cond) \
   do { \
      if (!(cond)) { \
         g_failed_tests++; \
         log_error("Test failed at %s:%d: %s", __FILE__, __LINE__, #cond); \
      } \
   } while (0);

#include "test_string.cpp"
 #include "test_gap_buffer.cpp"

int
main(int argc, char **argv)
{
   test_string();
   test_gap_buffer();

   if (g_failed_tests == 0) {
      log_info("All tests passed successfully!");
   } else {
      log_fatal("%u tests failed.", g_failed_tests);
   }

   return 0;
}