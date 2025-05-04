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
#include <ctype.h>
#include <stdint.h>

#define NAME_LENGTH 25
#define DESCR_LENGTH 250

typedef enum cli_type {
	FLAG,
	BOOL,
	INT,
	DOUBLE,
	STRING
} cli_type;

typedef struct cli_opt_item {
	char name_small[NAME_LENGTH];
	char name_big[NAME_LENGTH];
	char description[DESCR_LENGTH];
	cli_type type;
	int required;
} cli_opt_item;

typedef struct cli_arg_item {
	char name[NAME_LENGTH];
	char description[DESCR_LENGTH];
	cli_type type;
	int required;
} cli_arg_item;

typedef struct cli_opt_list {
	cli_opt_item item;
	uint64_t *result;
	struct cli_opt_list *next;
	struct cli_opt_list *prev;
} cli_opt_list;

typedef struct cli_arg_list {
	cli_arg_item item;
	uint64_t *result;
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
	void* (*command_function)(cli_cmd_group *master_group, cli_cmd_group *cmd_group);
} cli_cmd_item;

typedef struct cli_cmd_list {
	cli_cmd_item item;
	uint64_t *result;
	struct cli_cmd_list *next;
	struct cli_cmd_list *prev;
} cli_cmd_list;

typedef struct cli_list {
	char prog_description[DESCR_LENGTH];
	cli_opt_list *opt_head;
	cli_arg_list *arg_head;
	cli_cmd_list *cmd_head;
} cli_list;


int cli_init(cli_list *cli_list_obj, const char *program_desc, char *help[2]);
int cli_destroy(cli_list *cli_list_obj);
//Options
int cli_add_opt_basic(cli_list *cli_list_obj, char *option_flag_small, char *option_flag_big, char *descr);
int cli_add_opt(cli_list *cli_list_obj, cli_opt_item opt);
//Arguments
int cli_add_arg_basic(cli_list* cli_list_obj, char *argument_name, char *descr);
int cli_add_arg(cli_list* cli_list_obj, cli_arg_item arg);
//Commands
int cli_add_cmd_grp(cli_list *cli_list_obj, cli_cmd_group *cli_cmd_list_group, char *name, char *descr, char *help[2]);
int cli_grp_add_opt(cli_cmd_group *cli_group, char *option_flag_small, char *option_flag_big, char *descr, cli_type type);
int cli_grp_add_arg(cli_cmd_group *cli_group, char *name, char *descr, int required, cli_type type);

int cli_display_help(char *help_option_small, char *help_option_big);
int cli_help_msg(cli_list *cli_list_obj, char **argv);
int cli_execute(cli_list *cli_list_obj, int argc, char **argv);
int check_opt_type(cli_opt_list *tmp_opt, char *argv);

cli_opt_list* find_opt_name(cli_opt_list *opt_list, char search_name[NAME_LENGTH]);
cli_arg_list* find_arg_name(cli_arg_list *arg_list, char search_name[NAME_LENGTH]);
cli_cmd_list* find_cmd_name(cli_cmd_list *cmd_list, char search_name[NAME_LENGTH]);

int check_opt_type(cli_opt_list *tmp_opt, char *optv);
int check_arg_type(cli_arg_list *tmp_arg, char *argv);

int cli_set_default(cli_cmd_group group_head, char* search_name, uint64_t default_value);

#ifdef CLI_IMPLEMENTATION

int cli_set_default(cli_cmd_group group_head, char* search_name, uint64_t default_value){
	cli_opt_list* found_opt = NULL;
	cli_arg_list* found_arg = NULL;

	if(group_head.opt_head != NULL){
		found_opt = find_opt_name(group_head.opt_head, search_name);
	}else if(group_head.arg_head != NULL){
		found_arg = find_arg_name(group_head.arg_head, search_name);
	}

	if(found_opt != NULL){
		found_opt->result = malloc(sizeof(uint64_t));
		*found_opt->result = default_value;
		return 1;
	}else if(found_arg != NULL){
		found_arg->result = malloc(sizeof(uint64_t));
		*found_arg->result = default_value;
		return 1;
	}else{
		return 0;
	}
}

int cli_grp_add_opt(cli_cmd_group *cli_group, char *option_flag_small, char *option_flag_big, char *descr, cli_type type){
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

	if(strlen(option_flag_small)){
		strncpy((*tmp_list)->item.name_small, option_flag_small, NAME_LENGTH);
	}
	if(strlen(option_flag_big)){
		strncpy((*tmp_list)->item.name_big, option_flag_big, NAME_LENGTH);
	}
	strncpy((*tmp_list)->item.description, descr, DESCR_LENGTH);
	(*tmp_list)->item.type = type;

	return 0;
}

int cli_grp_add_arg(cli_cmd_group *cli_group, char *name, char *descr, int required, cli_type type){
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

int cli_add_cmd_grp(cli_list *cli_list_obj, cli_cmd_group *cli_cmd_list_group, char *name, char *descr, char *help[2]){
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
	cli_grp_add_opt(cli_cmd_list_group, help[0], help[1], "This is a help message.", FLAG);

	return 0;
}

int cli_init(cli_list *cli_list_obj, const char *program_desc, char *help[2]){
	strncpy(cli_list_obj->prog_description, program_desc, DESCR_LENGTH);
	cli_list_obj->opt_head = NULL;
	cli_list_obj->arg_head = NULL;
	cli_list_obj->cmd_head = NULL;

	if(help == NULL){
		cli_add_opt(cli_list_obj,(cli_opt_item){"-h", "--help", "Help option to show this message.", FLAG});
	}else{
		cli_add_opt(cli_list_obj,(cli_opt_item){help[0], help[1], "Help option to show this message.", FLAG});
	}

	return 0;
}

int cli_display_help(char *help_option_small, char *help_option_big){}
int cli_add_opt(cli_list *cli_list_obj, cli_opt_item opt){
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

	if(strlen(opt.name_small)){
		strncpy((*tmp_list)->item.name_small, opt.name_small, NAME_LENGTH);
	}
	if(strlen(opt.name_big)){
		strncpy((*tmp_list)->item.name_big, opt.name_big, NAME_LENGTH);
	}
	strncpy((*tmp_list)->item.description, opt.description, DESCR_LENGTH);
	(*tmp_list)->item.type = opt.type;
	(*tmp_list)->item.required = opt.required;

	return 0;
}

int cli_add_arg(cli_list* cli_list_obj, cli_arg_item arg){
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

	strncpy((*tmp_list)->item.name, arg.name, NAME_LENGTH);
	strncpy((*tmp_list)->item.description, arg.description, DESCR_LENGTH);
	(*tmp_list)->item.type = arg.type;
	(*tmp_list)->item.required = arg.required;
	
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

cli_opt_list* find_opt_name(cli_opt_list *opt_list, char search_name[NAME_LENGTH]){
	cli_opt_list *tmp = opt_list;
	while(tmp != NULL){
		if(!strcmp(tmp->item.name_small, search_name)){
			return tmp;
		}
		if(!strcmp(tmp->item.name_big, search_name)){
			return tmp;
		}
		tmp = tmp->next;
	}
	return NULL;
}

cli_arg_list* find_arg_name(cli_arg_list *arg_list, char search_name[NAME_LENGTH]){
	cli_arg_list *tmp = arg_list;
	while(tmp != NULL){
		if(!strcmp(tmp->item.name, search_name)){
			return tmp;
		}
		tmp = tmp->next;
	}
	return NULL;
}

cli_cmd_list* find_cmd_name(cli_cmd_list *cmd_list, char search_name[NAME_LENGTH]){
	size_t i = 0;
	cli_cmd_list *tmp = cmd_list;
	while(tmp != NULL){
		if(!strcmp(tmp->item.name, search_name)){
			return tmp;
		}
		i++;
		tmp = tmp->next;
	}
	return NULL;
}

int check_arg_type(cli_arg_list *tmp_arg, char *argv){
	switch(tmp_arg->item.type){
		case FLAG:
			if(tmp_arg->result == NULL){
				tmp_arg->result = malloc(sizeof(uint64_t));
			}
			*tmp_arg->result = 1;
			return -1;
		break;
		case BOOL:
			if(argv == NULL) return -2;

			char *bool_values[] = {"yes", "no", "true", "false", "y", "n"};
			for(size_t i = 0; i < 6; i++){
				//tolower(argv)
				if(!strcmp(bool_values[i], argv)){
					if(tmp_arg->result == NULL){
						tmp_arg->result = malloc(sizeof(uint64_t));
					}
					if(!(i%2)){
						*tmp_arg->result = 1;
						return 0; 
					}else if(i%2){
						*tmp_arg->result = 0;
						return 0; 
					}
				}
			}
			return -2;
		break;
		case INT:
			if(argv == NULL) return -2;

			for(int i = 0; i < strlen(argv); i++){
				if(!isdigit(argv[i]))
					return -2;
			}
			if(tmp_arg->result == NULL){
				tmp_arg->result = malloc(sizeof(uint64_t));
			}
			*tmp_arg->result = atoi(argv);
			return 0;
		break;
		case DOUBLE:
			if(argv == NULL) return -2;

			char *endptr;
			double val = strtod(argv, &endptr);
			if (!strlen(endptr)){
				if(tmp_arg->result == NULL){
					tmp_arg->result = malloc(sizeof(uint64_t));
				}
				*tmp_arg->result = val;
				return 0;
			}
			return -2;
		break;
		case STRING:
			if(argv == NULL) return -2;
			if(tmp_arg->result == NULL){
				tmp_arg->result = malloc(sizeof(uint64_t));
			}
			*tmp_arg->result = argv;
			return 0;
		break;
	}
}

int check_opt_type(cli_opt_list *tmp_opt, char *argv){
	switch(tmp_opt->item.type){
		case FLAG:
			if(tmp_opt->result == NULL){
				tmp_opt->result = malloc(sizeof(uint64_t));
			}
			*tmp_opt->result = 1;
			return -1;
		break;
		case BOOL:
			if(argv == NULL) return -2;
			if(argv[0] == '-') return -2;

			char *bool_values[] = {"yes", "no", "true", "false", "y", "n"};
			for(size_t i = 0; i < 6; i++){
				//tolower(argv)
				if(!strcmp(bool_values[i], argv)){
					if(tmp_opt->result == NULL){
						tmp_opt->result = malloc(sizeof(uint64_t));
					}
					tmp_opt->result = malloc(sizeof(uint64_t));
					if(!(i%2)){
						*tmp_opt->result = 1;
						return 0; 
					}else if(i%2){
						*tmp_opt->result = 0;
						return 0; 
					}
				}
			}
			return -2;
		break;
		case INT:
			if(argv == NULL) return -2;
			if(argv[0] == '-') return -2;

			for(int i = 0; i < strlen(argv); i++){
				if(!isdigit(argv[i]))
					return -2;
			}
			if(tmp_opt->result == NULL){
				tmp_opt->result = malloc(sizeof(uint64_t));
			}
			*tmp_opt->result = atoi(argv);
			return 0;
		break;
		case DOUBLE:
			if(argv == NULL) return -2;
			if(argv[0] == '-') return -2;

			char *endptr;
			double val = strtod(argv, &endptr);
			if (argv == endptr){
				if(tmp_opt->result == NULL){
					tmp_opt->result = malloc(sizeof(uint64_t));
				}
				*tmp_opt->result = val;
				return 0;
			}
			return -2;
		break;
		case STRING:
			if(argv == NULL) return -2;
			if(argv[0] == '-') return -2;
			if(tmp_opt->result == NULL){
				tmp_opt->result = malloc(sizeof(uint64_t));
			}
			*tmp_opt->result = argv;

			return 0;
		break;
	}
}


int cli_opt_parser(cli_opt_list *opt_head, int argc, char **argv, int *index){
	for(; *index < argc; (*index)++){
		cli_opt_list *found_opt = find_opt_name(opt_head, argv[*index]);
		if(found_opt != NULL){
			printf("[OPT]\n");
			int check_res = 0;
			if(*index + 1 < argc){
				check_res = check_opt_type(found_opt, argv[*index+1]);
			}else{
				check_res = check_opt_type(found_opt, NULL);
			}

			if(check_res > -1){
				printf("[OPT-ARG]\n");
				(*index)++;
			}else if(check_res == -2){
				printf("!!EXPECTED ARG!!\n");
				return 0;
			}else{
				printf("[FLAG]\n");
			}
		}else{
			if(argv[*index][0] == '-')
				return 0;
			break;
		}
	}

	return 1;
}

int cli_arg_parser(cli_arg_list *arg_head, int argc, char **argv, int *index){
	if(*index >= argc) return 1;

	cli_arg_list *tmp_arg = arg_head;
	for(; *index < argc; (*index)++){
		if(tmp_arg == NULL) return 0;
		int check_res = check_arg_type(tmp_arg, argv[*index]);
		if(check_res > -1){
			printf("[ARG]\n");
		}else{
			printf("!!EXPECTED ARG!!\n");
			return 0;
		}
		tmp_arg = tmp_arg->next;
	}

	return 1;
}

int cli_cmd_parser(cli_cmd_list *cmd_head, int argc, char **argv, int *index){
	if(*index >= argc) return 1;

	cli_cmd_list *found_cmd = find_cmd_name(cmd_head, argv[*index]);
	if(found_cmd != NULL){
		printf("[CMD]\n");
		(*index)++;
		int res_opt = cli_opt_parser(found_cmd->item.cli_cmd_list_group->opt_head, argc, argv, index);
		if(!res_opt) return -1;
		int res_arg = cli_arg_parser(found_cmd->item.cli_cmd_list_group->arg_head, argc, argv, index);
		return (res_opt && res_arg) ? 1 : -1;
	}

	return 0;
}

// [PROGRAM]: [OPTIONS | OPTIONS=args]* ([COMMANDS]? | [ARGUMENTS]*)
// [COMMANDS]: [OPTIONS | OPTIONS=args]* [ARGUMENTS]*
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

	tmp_opt = tmp_opt->next;
	int i = 1;
	if(!cli_opt_parser(tmp_opt, argc, argv, &i))
		return 0;
	int cmd_parse_res = cli_cmd_parser(tmp_cmd, argc, argv, &i);
	if(cmd_parse_res == 0){
		return cli_arg_parser(tmp_arg, argc, argv, &i);
	}else if(cmd_parse_res == 1 && i == argc){
		return 1;
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
			free(tmp->result);
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
			free(tmp->result);
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
					free(tmp->result);
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
					free(tmp->result);
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
