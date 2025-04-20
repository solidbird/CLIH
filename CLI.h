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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum cli_type {
	BOOL,
	INT,
	DOUBLE,
	STRING
} cli_type;

typedef enum cli_variant {
	MAIN,
	OPT,
	ARG,
	COM
} cli_variant;

typedef struct cli_option_item {
	cli_type type;
	char name_small[50];
	char name_big[50];
	char description[50];
	void *default_value;
} cli_option_item;

typedef struct cli_argument_item {
	cli_type type;
	char name[50];
	char description[50];
	int required;
} cli_argument_item;

typedef struct cli_command_item {
	char name[50];
	char description[50];
	void *command_function;
} cli_command_item;

typedef struct cli_output {
	char program_descr[50];
	cli_option_item *items;
} cli_output;

typedef struct cli_list_opt {
	cli_option_item item;
	struct cli_list_opt *next;
} cli_list_opt;

typedef struct cli_list_arg {
	cli_argument_item item;
	struct cli_list_arg *next;
} cli_list_arg;

typedef struct cli_list_cmd {
	cli_option_item item;
	struct cli_list_cmd *next;
} cli_list_cmd;

typedef struct cli_list {
	char prog_description[50];
	cli_list_opt *opt_head;
	cli_list_arg *arg_head;
	cli_list_cmd *cmd_head;
} cli_list;

int cli_init(cli_list *cli_list_obj, const char *program_desc);
int cli_display_help(char *help_option_small, char *help_option_big);
int cli_add_option(cli_list *cli_list_obj, char *option_flag_small, char *option_flag_big, char *descr, cli_type type);
int cli_add_argument(char *argument_name, cli_type type);
int cli_execute(cli_list *cli_list_obj, int argc, char **argv);

/*cli_init(program_descr);
cli_add_option("-r", "--reload", "This is a description of the whole thing.", cli_type.BOOL);
cli_add_option("-s", "--start", "Start the program with a delay of X seconds.", cli_type.INT);
cli_add_option("-t", "--time", "This is a description of the whole thing.", cli_type.BOOL);
cli_add_option(NULL, "--ls", "This is a description of the whole thing.", cli_type.BOOL);
cli_execute(int argc, char **argv);*/

#ifdef CLI_IMPLEMENTATION

int cli_init(cli_list *cli_list_obj, const char *program_desc){
	strncpy(cli_list_obj->prog_description, program_desc, 50);
	cli_list_obj->opt_head = NULL;
	cli_list_obj->arg_head = NULL;
	cli_list_obj->cmd_head = NULL;
	return 0;
}

int cli_display_help(char *help_option_small, char *help_option_big){}
int cli_add_option(cli_list *cli_list_obj, char *option_flag_small, char *option_flag_big, char *descr,  cli_type type){
	cli_list_opt **tmp_list = &cli_list_obj->opt_head;

	if((*tmp_list) != NULL){
		while((*tmp_list)->next != NULL){
			(tmp_list) = &(*tmp_list)->next;
		}

		(*tmp_list)->next = malloc(sizeof(cli_list_opt));
		tmp_list = &(*tmp_list)->next;

	}else{
	
		(*tmp_list) = malloc(sizeof(cli_list_opt));
	}

	if(option_flag_small){
		strncpy((*tmp_list)->item.name_small, option_flag_small, 50);
	}
	if(option_flag_big){
		strncpy((*tmp_list)->item.name_big, option_flag_big, 50);
	}
	strncpy((*tmp_list)->item.description, descr, 50);
	(*tmp_list)->item.default_value = 0;

	return 0;

}

int cli_add_argument(char *argument_name, cli_type type){}
int cli_execute(cli_list *cli_list_obj, int argc, char **argv){
	cli_list *tmp_list = cli_list_obj;
	cli_list_opt *tmp_opt = tmp_list->opt_head;
	cli_list_arg *tmp_arg = tmp_list->arg_head;
	cli_list_cmd *tmp_cmd = tmp_list->cmd_head;

	printf("Usage: %s\n\t%s\n\n", argv[0], tmp_list->prog_description);

	while(tmp_opt != NULL){
		printf("'%s, %s':\n\t\t\t%s\n",
			tmp_opt->item.name_small ? tmp_opt->item.name_small : "",
			tmp_opt->item.name_big ? tmp_opt->item.name_big: "",
			tmp_opt->item.description);
		tmp_opt = tmp_opt->next;
	}

	return 0;
}

#endif // CLI_IMPLEMENTATION

#endif // CLI_H
