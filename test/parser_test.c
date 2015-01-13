#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#include <unistd.h>

#include "pipe.h"
#include "parser.h"

static const char* testCmd = "cmd_for_test first second third last";
static const int testArgc = 5;
static const char testArgv[][40] = {
	"cmd_for_test",
	"first",
	"second",
	"third",
	"last"
};

static void
setup_system(void** state)
{
	assert_return_code(cs_parser_init(), 1);
	assert_return_code(cs_pipe_init(), 1);
}

static void
cleanup_system(void** state)
{
	cs_parser_quit();
	cs_pipe_quit();
}

static void
test_cs_parser_parse(void** state)
{
	cs_parser_parse(testCmd);
}

static void
test_cs_parser_getArgc(void** state)
{
	assert_int_equal(testArgc, cs_parser_getArgc());
}

static void
test_cs_parser_getArgv(void** state)
{
	char** argv = cs_parser_getArgv();

	assert_non_null(argv);

	for (int i = 0; i < testArgc; ++i) {
		assert_non_null(argv[i]);
		assert_string_equal(testArgv[i], argv[i]);
	}
}

int
main(int argc, char* argv[])
{
	const UnitTest tests[] = {
		group_test_setup(setup_system),
		unit_test(test_cs_parser_parse),
		unit_test(test_cs_parser_getArgc),
		unit_test(test_cs_parser_getArgv),
		group_test_teardown(cleanup_system)
	};

	return run_group_tests(tests);
}
