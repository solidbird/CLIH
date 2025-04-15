/*
 * This is how I imagine the output to lool like:
 * https://zetcode.com/python/click/
 *
Usage: program [OPTIONS] FILE1 FILE2 TEST

	This is a super cool program I wrote.

Arguments:
	FILE1 [required]	First file to check.
	FILE2 [required]	Second file to check.
	TEST [required]		Test file submitted.

Options:
	-t, --test LEVEL				Command to test something with either
									CRIT, ERROR, WARN, INFO, DEBUG.
									[default: INFO]
	-p, --ping DEST					Ping the corresponging argument
	--ls	 						List all contents
	-h, --help						Show this mesage and exit.

*/

#ifndef CLI_H
#define CLI_H

enum cli_types = {
	BOOL,
	INT,
	FLOAT,
	DOUBLE,
	CHAR,
	STRING,
	PATH
};

typedef struct cli_option_item {
	const char *name[2],
	const char *argument_name,
	const char *description,
	void *default_value,
} cli_option_item;

typedef struct cli_argument_item {
	const char *name,

}

typedef struct cli_argument_command {
	const char *name,
	void *command_function
}

typedef struct cli_output {
	const char* program_descr,
	cli_option_item *items
} cli_output;

int cli_display_help(const char *help_option_small, const char *help_option_big);
int cli_add_option(const char *option_flag_small, const char *option_flag_big,  cli_types type);
int cli_add_argument(const char *argument_name, cli_types type);

#ifdef CLI_IMPLEMENTATION

int cli_display_help(const char *help_option_small, const char *help_option_big){}
int cli_add_option(const char *option_flag_small, const char *option_flag_big,  cli_types type){}
int cli_add_argument(const char *argument_name, cli_types type){}

#endif // CLI_IMPLEMENTATION

#endif // CLI_H
