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

#include <stdlib.h>

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

typedef struct cli_list {
	cli_variant cli_var;
	cli_option_item option_item;
	cli_argument_item argument_item;
	cli_command_item command_item;
	struct cli_list *next;
} cli_list;

int cli_init(cli_list **cli_list_obj, char *program_desc);
int cli_display_help(char *help_option_small, char *help_option_big);
int cli_add_option(cli_list **cli_list_obj, char *option_flag_small, char *option_flag_big, char *descr, cli_type type);
int cli_add_argument(char *argument_name, cli_type type);
int cli_execute(cli_list **cli_list_obj, int argc, char **argv);

/*cli_init(program_descr);
cli_add_option("-r", "--reload", "This is a description of the whole thing.", cli_type.BOOL);
cli_add_option("-s", "--start", "Start the program with a delay of X seconds.", cli_type.INT);
cli_add_option("-t", "--time", "This is a description of the whole thing.", cli_type.BOOL);
cli_add_option(NULL, "--ls", "This is a description of the whole thing.", cli_type.BOOL);
cli_execute(int argc, char **argv);*/

#ifdef CLI_IMPLEMENTATION

int cli_init(cli_list **cli_list_obj, char *program_desc){
	(*cli_list_obj) = malloc(sizeof(cli_list));
	(*cli_list_obj)->cli_var = MAIN;
	strncpy(&(*cli_list_obj)->command_item.description, program_desc, 50);
	(*cli_list_obj)->next = NULL;

	return 0;
}

int cli_display_help(char *help_option_small, char *help_option_big){}
int cli_add_option(cli_list **cli_list_obj, char *option_flag_small, char *option_flag_big, char *descr,  cli_type type){
	cli_list *tmp_list = (*cli_list_obj);

	while(tmp_list->next != NULL){
		tmp_list = tmp_list->next;
	}

	tmp_list->next = malloc(sizeof(cli_list));
	tmp_list->next->cli_var = OPT;
	if(option_flag_small){
		strncpy(&tmp_list->next->option_item.name_small, option_flag_small, 50);
	}
	if(option_flag_big){
		strncpy(&tmp_list->next->option_item.name_big, option_flag_big, 50);
	}
	strncpy(&tmp_list->next->option_item.description, descr, 50);
	tmp_list->next->option_item.default_value = 0;
	tmp_list->next->next = NULL;

	return 0;
}
int cli_add_argument(char *argument_name, cli_type type){}
int cli_execute(cli_list **cli_list_obj, int argc, char **argv){
	cli_list *tmp_list = (*cli_list_obj);
	int i = 0;

	while(tmp_list != NULL){
		switch(tmp_list->cli_var){
			case MAIN:
				printf("Usage: %s\n\t%s\n\n", argv[0], tmp_list->command_item.description);
			break;
			case OPT:
				printf("'%s, %s':\n\t\t\t%s\n",
					tmp_list->option_item.name_small ? tmp_list->option_item.name_small : "",
					tmp_list->option_item.name_big ? tmp_list->option_item.name_big: "",
					tmp_list->option_item.description);
			break;
			case ARG:
			break;
			case COM:
			break;
		}
		i++;
		tmp_list = tmp_list->next;
	}

	return 0;
}

#endif // CLI_IMPLEMENTATION

#endif // CLI_H
