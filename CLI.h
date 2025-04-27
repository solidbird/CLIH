/*
 * This is how I imagine the output to lool like:
 * https://zetcode.com/python/click/
 * https://clig.dev
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

#define NAME_LENGTH 50
#define DESCR_LENGTH 500

typedef enum cli_type {
	FLAG,
	BOOL,
	INT,
	DOUBLE,
	STRING
} cli_type;

typedef struct cli_opt_item {
	cli_type type;
	char name_small[NAME_LENGTH];
	char name_big[NAME_LENGTH];
	char description[DESCR_LENGTH];
	void *default_value;
} cli_opt_item;

typedef struct cli_arg_item {
	cli_type type;
	char name[NAME_LENGTH];
	char description[DESCR_LENGTH];
	int required;
} cli_arg_item;

typedef struct cli_opt_list {
	cli_opt_item item;
	struct cli_opt_list *next;
	struct cli_opt_list *prev;
} cli_opt_list;

typedef struct cli_arg_list {
	cli_arg_item item;
	struct cli_arg_list *next;
	struct cli_arg_list *prev;
} cli_arg_list;

typedef struct cli_cmd_group {
	cli_opt_list *opt_head;
	cli_arg_list *arg_head;
} cli_cmd_group;

typedef struct cli_cmd_item {
	char name[NAME_LENGTH];
	char description[DESCR_LENGTH];
	cli_cmd_group *cli_cmd_list_group;	
	void* (*command_function)(cli_opt_list *opts, cli_arg_list *args);
} cli_cmd_item;

typedef struct cli_cmd_list {
	cli_cmd_item item;
	struct cli_cmd_list *next;
	struct cli_cmd_list *prev;
} cli_cmd_list;

typedef struct cli_list {
	char prog_description[DESCR_LENGTH];
	cli_opt_list *opt_head;
	cli_arg_list *arg_head;
	cli_cmd_list *cmd_head;
} cli_list;

int cli_add_command_group(cli_list *cli_list_obj, cli_cmd_group *cli_cmd_list_group, char *name, char *descr, char *help[2]);
int cli_group_add_opt(cli_cmd_group *cli_group, char *option_flag_small, char *option_flag_big, char *descr, cli_type type);
int cli_group_add_arg(cli_cmd_group *cli_group, char *name, char *descr, int required, cli_type type);
int cli_init(cli_list *cli_list_obj, const char *program_desc, char *help[2]);
int cli_display_help(char *help_option_small, char *help_option_big);
int cli_add_option(cli_list *cli_list_obj, char *option_flag_small, char *option_flag_big, char *descr, cli_type type);
int cli_add_argument(cli_list* cli_list_obj, char *argument_name, char *descr, int req, cli_type type);
int cli_help_msg(cli_list *cli_list_obj, char **argv);
int cli_execute(cli_list *cli_list_obj, int argc, char **argv);
int cli_destroy(cli_list *cli_list_obj);

#ifdef CLI_IMPLEMENTATION

int cli_group_add_opt(cli_cmd_group *cli_group, char *option_flag_small, char *option_flag_big, char *descr, cli_type type){
	cli_opt_list **tmp_list = &cli_group->opt_head;

	if((*tmp_list) != NULL){
		while((*tmp_list)->next != NULL){
			(tmp_list) = &(*tmp_list)->next;
		}

		(*tmp_list)->next = malloc(sizeof(cli_opt_list));
		(*tmp_list)->next->prev = (*tmp_list);
		tmp_list = &(*tmp_list)->next;

	}else{
	
		(*tmp_list) = malloc(sizeof(cli_opt_list));
	}

	if(option_flag_small){
		strncpy((*tmp_list)->item.name_small, option_flag_small, NAME_LENGTH);
	}
	if(option_flag_big){
		strncpy((*tmp_list)->item.name_big, option_flag_big, NAME_LENGTH);
	}
	strncpy((*tmp_list)->item.description, descr, DESCR_LENGTH);
	(*tmp_list)->item.default_value = 0;
	(*tmp_list)->item.type = type;

	return 0;
}

int cli_group_add_arg(cli_cmd_group *cli_group, char *name, char *descr, int required, cli_type type){
	cli_arg_list **tmp_list = &cli_group->arg_head;

	if((*tmp_list) != NULL){
		while((*tmp_list)->next != NULL){
			(tmp_list) = &(*tmp_list)->next;
		}

		(*tmp_list)->next = malloc(sizeof(cli_arg_list));
		(*tmp_list)->next->prev = (*tmp_list);
		tmp_list = &(*tmp_list)->next;

	}else{
	
		(*tmp_list) = malloc(sizeof(cli_arg_list));
	}

	strncpy((*tmp_list)->item.name, name, NAME_LENGTH);
	strncpy((*tmp_list)->item.description, descr, DESCR_LENGTH);
	(*tmp_list)->item.type = type;
	(*tmp_list)->item.required = required;

	return 0;
}

int cli_add_command_group(cli_list *cli_list_obj, cli_cmd_group *cli_cmd_list_group, char *name, char *descr, char *help[2]){
	//add new member of command item to cli_list_obj and add pointer of cli_cmd_list_group to that member
	cli_cmd_list **tmp_list = &cli_list_obj->cmd_head;

	if((*tmp_list) != NULL){
		while((*tmp_list)->next != NULL){
			(tmp_list) = &(*tmp_list)->next;
		}

		(*tmp_list)->next = malloc(sizeof(cli_cmd_list));
		(*tmp_list)->next->prev = (*tmp_list);
		tmp_list = &(*tmp_list)->next;

	}else{
	
		(*tmp_list) = malloc(sizeof(cli_cmd_list));
	}

	strncpy((*tmp_list)->item.name, name, NAME_LENGTH);
	strncpy((*tmp_list)->item.description, descr, DESCR_LENGTH);
	(*tmp_list)->item.cli_cmd_list_group = cli_cmd_list_group;
	cli_group_add_opt(cli_cmd_list_group, help[0], help[1], "This is a help message.", FLAG);

	return 0;
}

int cli_init(cli_list *cli_list_obj, const char *program_desc, char *help[2]){
	strncpy(cli_list_obj->prog_description, program_desc, DESCR_LENGTH);
	cli_list_obj->opt_head = NULL;
	cli_list_obj->arg_head = NULL;
	cli_list_obj->cmd_head = NULL;

	if(help == NULL){
		cli_add_option(cli_list_obj, "-h", "--help", "Help option to show this message.", FLAG);
	}else{
		cli_add_option(cli_list_obj, help[0], help[1], "Help option to show this message.", FLAG);
	}

	return 0;
}

int cli_display_help(char *help_option_small, char *help_option_big){}
int cli_add_option(cli_list *cli_list_obj, char *option_flag_small, char *option_flag_big, char *descr,  cli_type type){
	cli_opt_list **tmp_list = &cli_list_obj->opt_head;

	if((*tmp_list) != NULL){
		while((*tmp_list)->next != NULL){
			(tmp_list) = &(*tmp_list)->next;
		}

		(*tmp_list)->next = malloc(sizeof(cli_opt_list));
		(*tmp_list)->next->prev = (*tmp_list);
		tmp_list = &(*tmp_list)->next;

	}else{
	
		(*tmp_list) = malloc(sizeof(cli_opt_list));
	}

	if(option_flag_small){
		strncpy((*tmp_list)->item.name_small, option_flag_small, NAME_LENGTH);
	}
	if(option_flag_big){
		strncpy((*tmp_list)->item.name_big, option_flag_big, NAME_LENGTH);
	}
	strncpy((*tmp_list)->item.description, descr, DESCR_LENGTH);
	(*tmp_list)->item.default_value = 0;
	(*tmp_list)->item.type = type;

	return 0;
}

int cli_add_argument(cli_list* cli_list_obj, char *argument_name, char *descr, int req, cli_type type){
	cli_arg_list **tmp_list = &cli_list_obj->arg_head;

	if((*tmp_list) != NULL){
		while((*tmp_list)->next != NULL){
			(tmp_list) = &(*tmp_list)->next;
		}

		(*tmp_list)->next = malloc(sizeof(cli_arg_list));
		(*tmp_list)->next->prev = (*tmp_list);
		tmp_list = &(*tmp_list)->next;

	}else{
	
		(*tmp_list) = malloc(sizeof(cli_arg_list));
	}

	strncpy((*tmp_list)->item.name, argument_name, NAME_LENGTH);
	strncpy((*tmp_list)->item.description, descr, DESCR_LENGTH);
	(*tmp_list)->item.required = req;
	(*tmp_list)->item.type = type;

	return 0;
}

int cli_help_msg(cli_list *cli_list_obj, char **argv){
	cli_list *tmp_list = cli_list_obj;
	cli_opt_list *tmp_opt = tmp_list->opt_head;
	cli_arg_list *tmp_arg = tmp_list->arg_head;
	cli_cmd_list *tmp_cmd = tmp_list->cmd_head;

	printf("Usage: %s\n\t%s\n\n", argv[0], tmp_list->prog_description);

	if(tmp_opt){
		printf("OPTIONS:\n");
		while(tmp_opt != NULL){
			//printf("\t");
			if(strlen(tmp_opt->item.name_small)){
				printf("%s", tmp_opt->item.name_small);
			}
			if(strlen(tmp_opt->item.name_small) && strlen(tmp_opt->item.name_big)){
				printf(", ");
			}
			if(strlen(tmp_opt->item.name_big)){
				printf("%s", tmp_opt->item.name_big);
			}
			printf("\t%s\n", tmp_opt->item.description);
			tmp_opt = tmp_opt->next;
		}
	}
	
	if(tmp_arg){
		printf("\nARGUMENTS:\n");
		while(tmp_arg != NULL){
			printf("%s%s\n",
				tmp_arg->item.name,
				tmp_arg->item.description);
			tmp_arg = tmp_arg->next;
		}
	}

	if(tmp_cmd){
		printf("\nCOMMANDS:\n");
		while(tmp_cmd != NULL){
			printf("%s%s\n",
				tmp_cmd->item.name,
				tmp_cmd->item.description);
			tmp_cmd = tmp_cmd->next;
		}
	}
	return 0;
}

// [PROGRAM] [OPTIONS_1] [COMMANDS] [OPTIONS_X] [ARGUMENTS]
int cli_execute(cli_list *cli_list_obj, int argc, char **argv){
	cli_list *tmp_list = cli_list_obj;
	cli_opt_list *tmp_opt = tmp_list->opt_head;
	cli_arg_list *tmp_arg = tmp_list->arg_head;
	cli_cmd_list *tmp_cmd = tmp_list->cmd_head;

	if(argc < 2){
		cli_help_msg(cli_list_obj, argv);
		exit(1);
	}

	for(size_t i = 1; i < argc; i++){
		if(strcmp((tmp_opt)->item.name_small, argv[i]) == 0 ||
		strcmp((tmp_opt)->item.name_big, argv[i]) == 0){
			cli_help_msg(cli_list_obj, argv);
			exit(0);
		}
	}

	while(tmp_opt != NULL){
		for(size_t i = 0; i < argc; i++){
			if(tmp_opt->item.name_small && !strcmp(tmp_opt->item.name_small, argv[i])){
				printf("[OPT_SMALL]\n");
			}
			if(tmp_opt->item.name_big && !strcmp(tmp_opt->item.name_big, argv[i])){
				printf("[OPT_LONG]\n");
			}
		}
		tmp_opt = tmp_opt->next;
	}

	while(tmp_arg != NULL){
		for(size_t i = 0; i < argc; i++){
			if(!strcmp(tmp_arg->item.name, argv[i])){
				printf("[ARG]\n");
			}
		}
		tmp_arg = tmp_arg->next;
	}

	return 0;
}

int cli_destroy(cli_list *cli_list_obj){
	//TODO: Run through the lists and free every node at the end free the head node of each type
	cli_opt_list *tmp_opt = cli_list_obj->opt_head;
	cli_arg_list *tmp_arg = cli_list_obj->arg_head;
	cli_cmd_list *tmp_cmd = cli_list_obj->cmd_head;

	if(tmp_opt != NULL){
		while(tmp_opt->next != NULL){
			tmp_opt = tmp_opt->next;
		}
		while(tmp_opt != NULL){
			cli_opt_list *tmp = tmp_opt;
			tmp_opt = tmp_opt->prev;
			free(tmp);
		}
	}
	if(tmp_arg != NULL){
		while(tmp_arg->next != NULL){
			tmp_arg = tmp_arg->next;
		}
		while(tmp_arg != NULL){
			cli_arg_list *tmp = tmp_arg;
			tmp_arg = tmp_arg->prev;
			free(tmp);
		}
	}
	if(tmp_cmd != NULL){
		while(tmp_cmd->next != NULL){
			tmp_cmd = tmp_cmd->next;
		}

		while(tmp_cmd != NULL){
			cli_opt_list *tmp_cmd_opt = tmp_cmd->item.cli_cmd_list_group->opt_head;
			cli_arg_list *tmp_cmd_arg = tmp_cmd->item.cli_cmd_list_group->arg_head;

			if(tmp_cmd_opt != NULL){
				while(tmp_cmd_opt->next != NULL){
					tmp_cmd_opt = tmp_cmd_opt->next;
				}
				while(tmp_cmd_opt != NULL){
					cli_opt_list *tmp = tmp_cmd_opt;
					tmp_cmd_opt = tmp_cmd_opt->prev;
					free(tmp);
				}
			}
			if(tmp_cmd_arg != NULL){
				while(tmp_cmd_arg->next != NULL){
					tmp_cmd_arg = tmp_cmd_arg->next;
				}
				while(tmp_cmd_arg != NULL){
					cli_arg_list *tmp = tmp_cmd_arg;
					tmp_cmd_arg = tmp_cmd_arg->prev;
					free(tmp);
				}
			}

			cli_cmd_list *tmp = tmp_cmd;
			tmp_cmd = tmp_cmd->prev;
			free(tmp);
		}
	}
}

#endif // CLI_IMPLEMENTATION

#endif // CLI_H
