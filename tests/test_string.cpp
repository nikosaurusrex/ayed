#include "base/base_inc.h"

intern void
test_string()
{
   // Test case 1: Default constructor
   String8 s1;
   TEST_CHECK(s1.ptr == 0);
   TEST_CHECK(s1.len == 0);

   // Test case 2: Constructor with C-string
   const char* test_str = "Hello, World!";
   String8 s2(test_str);
   TEST_CHECK(s2.ptr == (U8*)test_str);
   TEST_CHECK(s2.len == strlen(test_str));

   // Test case 3: Constructor with pointer and length
   U8* test_ptr = (U8*)"Test";
   String8 s3(test_ptr, 4);
   TEST_CHECK(s3.ptr == test_ptr);
   TEST_CHECK(s3.len == 4);

   // Test case 4: Operator[] const
   const String8& s4 = s2;
   TEST_CHECK(s4[0] == 'H');
   TEST_CHECK(s4[7] == 'W');

   // Test case 5: Operator[] non-const
   char test_str2[] = "Hello, World!";
   String8 s5(test_str2);
   s5[0] = 'h';
   TEST_CHECK(s5[0] == 'h');

   // Test case 6: Operator== with C-string
   String8 s6("Test");
   TEST_CHECK(s6 == "Test");
   TEST_CHECK(s6 != "test");
   TEST_CHECK(s6 != "Testing");
  
   // Test case 7: Operator== with String8
   String8 s7("Test");
   TEST_CHECK(s6 == s7);
   String8 s8("test");
   TEST_CHECK(s6 != s8);

   // Test case 8: Edge cases for operator==
   String8 empty_str((U8*)"", 0);
   TEST_CHECK(empty_str == "");
   TEST_CHECK(empty_str != "a");

   // Test case 9: Comparison with 0
   TEST_CHECK(s6 != 0);
   String8 null_str = null_str8;
   TEST_CHECK(null_str == 0);
   TEST_CHECK(null_str == null_str8);
}