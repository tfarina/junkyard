#include "utest.h"

#include "mkdirp.h"
#include "test.h"

UTEST(mkdirp_test, create_parents)
{
  if (test_exists("./tmp/foo/bar/baz")) { rmdir("./tmp/foo/bar/baz"); }
  if (test_exists("./tmp/foo/bar/"))    { rmdir("./tmp/foo/bar/");    }
  if (test_exists("./tmp/foo/"))        { rmdir("./tmp/foo/");        }
  if (test_exists("./tmp/"))            { rmdir("./tmp/");            }

  ASSERT_EQ(0, f_mkdirp("./tmp/foo/bar/baz", 0700));
  ASSERT_TRUE(test_exists("./tmp"));
  ASSERT_TRUE(test_exists("./tmp/foo"));
  ASSERT_TRUE(test_exists("./tmp/foo/bar"));
  ASSERT_TRUE(test_exists("./tmp/foo/bar/baz"));
}

UTEST_MAIN()