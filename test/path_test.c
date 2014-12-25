#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#include <unistd.h>

#include "path.h"

static void
setup_system(void** state)
{
	assert_true(cs_path_init() == 0);
}

static void
cleanup_system(void** state)
{
	cs_path_quit();
}

static void
test_cs_path_getWorkingPath(void** state)
{
	char* wd = NULL;;
	size_t len = 30;

	wd = (char*) test_malloc(sizeof(char) * len);
	while (getcwd(wd, len) == NULL) {
		test_free(wd);
		len *= 2;
		wd = (char*) test_malloc(sizeof(char) * len);
	}

	assert_string_equal(cs_path_getWorkingPath(), wd);
	test_free(wd);
}

static void
test_cs_path_changeWorkingPath(void** state)
{
	const char path1[] = "/proc";
	const char path2[] = "/vvvvvvv";

	assert_int_equal(cs_path_changeWorkingPath(path1), 0);
	assert_string_equal(cs_path_getWorkingPath(), path1);

	assert_int_equal(cs_path_changeWorkingPath(path2), -1);
	assert_string_not_equal(cs_path_getWorkingPath(), path2);
}

int
main(int argc, char* argv[])
{
	const UnitTest tests[] = {
		unit_test_setup_teardown(test_cs_path_getWorkingPath,
					 setup_system, cleanup_system),
		unit_test_setup_teardown(test_cs_path_changeWorkingPath,
					 setup_system, cleanup_system)
	};

	return run_tests(tests);
}
