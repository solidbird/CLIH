/*
 * This is how I imagine the output to lool like:
 * https://zetcode.com/python/click/
 * https://clig.dev
 * https://stackoverflow.com/questions/70120431/how-would-i-fix-the-spacing-in-my-c-code-output
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

const char* BOLD_START = "\033[1m";
const char* BOLD_END = "\033[0m";

typedef enum cli_type {
	FLAG,
	BOOL,
	INT,
	DOUBLE,
	STRING
} cli_type;

typedef struct cli_type_block {
	cli_type type;
	int n;
} cli_type_block;

typedef union cli_result {
	int f;
	int b;
	int i;
	double d;
	char *s;

	int *lb;
	int *li;
	double *ld;
	char **ls;
} cli_result;

typedef struct cli_opt_item {
	char *name_small;
	char *name_big;
	char *description;
	cli_type_block type_block;
	int required;
} cli_opt_item;

typedef struct cli_arg_item {
	char *name;
	char *description;
	cli_type_block type_block;
	int required;
} cli_arg_item;

typedef struct cli_opt_list {
	cli_opt_item item;
	cli_result *result;
	struct cli_opt_list *next;
	struct cli_opt_list *prev;
} cli_opt_list;

typedef struct cli_arg_list {
	cli_arg_item item;
	cli_result *result;
	struct cli_arg_list *next;
	struct cli_arg_list *prev;
} cli_arg_list;

typedef struct cli_req_opt {
	cli_opt_item *item;
	struct cli_req_opt *next;
	struct cli_req_opt *prev;
} cli_req_opt;

typedef struct cli_req_arg {
	cli_arg_item *item;
	struct cli_req_arg *next;
	struct cli_req_arg *prev;
} cli_req_arg;

typedef struct cli_cmd_group {
	cli_opt_list *opt_head;
	cli_arg_list *arg_head;
	cli_req_opt *opt_req;
	cli_req_arg *arg_req;
} cli_cmd_group;

typedef struct cli_cmd_item {
	char *name;
	char *description;
	cli_cmd_group *cli_cmd_list_group;	
	void* (*command_function)(cli_cmd_group *master_group, cli_cmd_group *cmd_group);
} cli_cmd_item;

typedef struct cli_cmd_list {
	cli_cmd_item item;
	cli_result *result;
	struct cli_cmd_list *next;
	struct cli_cmd_list *prev;
} cli_cmd_list;

typedef struct cli_example_item {
	char *description;
	char *file_path;
} cli_example_item;

typedef struct cli_examples {
	cli_example_item item;
	struct cli_examples *next;
	struct cli_examples *prev;
} cli_examples;

typedef struct cli_list {
	char *prog_description;
	cli_cmd_group *opt_arg_grp;
	cli_cmd_list *cmd_head;
	cli_examples *examples;
} cli_list;


cli_list * cli_init(char *program_desc, char *help[2]);
int cli_destroy(cli_list *cli_list_obj);
//Options
int cli_add_opt_basic(cli_cmd_group *cli_group, char *option_flag_small, char *option_flag_big, char *descr);
int cli_add_opt(cli_cmd_group *cli_group, cli_opt_item opt);
//Arguments
int cli_add_arg_basic(cli_list* cli_list_obj, char *argument_name, char *descr);
int cli_add_arg(cli_cmd_group *cli_group, cli_arg_item arg);
//Commands
cli_cmd_group* cli_add_cmd_grp(cli_list *cli_list_obj, char *name, char *descr, char *help[2], void * func(cli_cmd_group*, cli_cmd_group*));
int cli_grp_add_opt_basic(cli_cmd_group *cli_group, char *option_flag_small, char *option_flag_big, char *descr);
int cli_grp_add_opt(cli_cmd_group *cli_group, cli_opt_item opt);
int cli_grp_add_arg_basic(cli_cmd_group *cli_group, char *name, char *descr);
int cli_grp_add_arg(cli_cmd_group *cli_group, cli_arg_item arg);

int cli_display_help(char *help_option_small, char *help_option_big);
int cli_help_msg(cli_list *cli_list_obj, char **argv);
int cli_help_msg_cmd(cli_cmd_item *cli_list_obj, char **argv);
int cli_execute(cli_list *cli_list_obj, int argc, char **argv);

cli_opt_list* find_opt_name(cli_opt_list *opt_list, char *search_name);
cli_arg_list* find_arg_name(cli_arg_list *arg_list, char *search_name);
cli_cmd_list* find_cmd_name(cli_cmd_list *cmd_list, char *search_name);

int check_opt_type(cli_cmd_group *grp, cli_opt_list *tmp_opt, char *argv, int *list_index);
int check_arg_type(cli_cmd_group *grp, cli_arg_list *tmp_arg, char *argv, int *list_index);

int cli_set_default(cli_cmd_group group_head, char* search_name, cli_result default_value);
void add_req_arg(cli_req_arg **list, cli_arg_item *item);
void remove_req_arg(cli_req_arg **list, cli_arg_item *item);
void add_req_opt(cli_req_opt **list, cli_opt_item *item);
void remove_req_opt(cli_req_opt **list, cli_opt_item *item);
int help_msg_found(cli_opt_list *opt_head, cli_opt_list *found_opt);
void print_formatted(const char *str, int indent, int line_width);
void print_space(int indent);

#ifdef CLI_IMPLEMENTATION
//------------------------------------------------------

void fprintf_type(FILE *fd, cli_type type, int list_n){
	char buf[256];
	switch(type){
		case FLAG:
				snprintf(buf, sizeof(buf), "FLAG");
			break;
		case BOOL:
			if(list_n){
				snprintf(buf, sizeof(buf), "BOOL_LIST[%d]", list_n);
			}else{
				snprintf(buf, sizeof(buf), "BOOL");
			}
			break;
		case INT:
			if(list_n){
				snprintf(buf, sizeof(buf), "INT_LIST[%d]", list_n);
			}else{
				snprintf(buf, sizeof(buf), "INT");
			}
			break;
		case DOUBLE:
			if(list_n){
				snprintf(buf, sizeof(buf), "DOUBLE_LIST[%d]", list_n);
			}else{
				snprintf(buf, sizeof(buf), "DOUBLE");
			}
			break;
		case STRING:
			if(list_n){
				snprintf(buf, sizeof(buf), "STRING_LIST[%d]", list_n);
			}else{
				snprintf(buf, sizeof(buf), "STRING");
			}
			break;
	}
	fprintf(fd, "%s", buf);
	return;
}

void print_space(int indent){
	for (int j = 0; j < indent; j++) {
		putchar(' ');
	}
}

void print_formatted(const char *str, int indent, int line_width) {
    int len = strlen(str);
    int i = 0;

    while (i < len) {
		print_space(indent);

        // Print up to 'line_width' characters or until the string ends
        for (int k = 0; k < line_width && i < len; k++, i++) {
            putchar(str[i]);
        }

        // Newline after each line
        putchar('\n');
    }
}

void _str_malloc_cpy(char **dest, char *src, size_t n){
	*dest = (char*) malloc(n + 1);
	strncpy(*dest, src, n);
}

int cli_set_default(cli_cmd_group group_head, char* search_name, cli_result default_value){
	cli_opt_list* found_opt = NULL;
	cli_arg_list* found_arg = NULL;

	if(group_head.opt_head != NULL){
		found_opt = find_opt_name(group_head.opt_head, search_name);
	}else if(group_head.arg_head != NULL){
		found_arg = find_arg_name(group_head.arg_head, search_name);
	}

	if(found_opt != NULL){
		found_opt->result = malloc(sizeof(cli_result));
		*found_opt->result = (cli_result)default_value;
		return 1;
	}else if(found_arg != NULL){
		found_arg->result = malloc(sizeof(cli_result));
		*found_arg->result = (cli_result)default_value;
		return 1;
	}else{
		return 0;
	}
}

int cli_grp_add_opt(cli_cmd_group *cli_group, cli_opt_item opt){
	cli_opt_list **tmp_list = &cli_group->opt_head;

	if((*tmp_list) != NULL){
		while((*tmp_list)->next != NULL){
			(tmp_list) = &(*tmp_list)->next;
		}

		(*tmp_list)->next = malloc(sizeof(cli_opt_list));
		memset((*tmp_list)->next, 0, sizeof(cli_opt_list));
		(*tmp_list)->next->prev = (*tmp_list);
		tmp_list = &(*tmp_list)->next;

	}else{
	
		(*tmp_list) = malloc(sizeof(cli_opt_list));
		memset((*tmp_list), 0, sizeof(cli_opt_list));
	}

	if(strlen(opt.name_small)){
		_str_malloc_cpy(&(*tmp_list)->item.name_small, opt.name_small, NAME_LENGTH);
		(*tmp_list)->item.name_small[NAME_LENGTH] = 0;
	}
	if(strlen(opt.name_big)){
		_str_malloc_cpy(&(*tmp_list)->item.name_big, opt.name_big, NAME_LENGTH);
		(*tmp_list)->item.name_big[NAME_LENGTH] = 0;
	}
	_str_malloc_cpy(&(*tmp_list)->item.description, opt.description, DESCR_LENGTH);
	(*tmp_list)->item.type_block = opt.type_block;
	(*tmp_list)->item.required = opt.required;

	if(opt.required){
		add_req_opt(&cli_group->opt_req, &(*tmp_list)->item);
	}

	return 0;
}

int cli_grp_add_arg(cli_cmd_group *cli_group, cli_arg_item arg){
	cli_arg_list **tmp_list = &cli_group->arg_head;

	if((*tmp_list) != NULL){
		while((*tmp_list)->next != NULL){
			(tmp_list) = &(*tmp_list)->next;
		}

		(*tmp_list)->next = malloc(sizeof(cli_arg_list));
		memset((*tmp_list)->next, 0, sizeof(cli_arg_list));
		(*tmp_list)->next->prev = (*tmp_list);
		tmp_list = &(*tmp_list)->next;

	}else{
	
		(*tmp_list) = malloc(sizeof(cli_arg_list));
		memset((*tmp_list), 0, sizeof(cli_arg_list));
	}

	_str_malloc_cpy(&(*tmp_list)->item.name, arg.name, NAME_LENGTH);
	_str_malloc_cpy(&(*tmp_list)->item.description, arg.description, DESCR_LENGTH);
	(*tmp_list)->item.type_block = arg.type_block;
	(*tmp_list)->item.required = arg.required;
	
	if(arg.required){
		add_req_arg(&cli_group->arg_req, &(*tmp_list)->item);
	}

	return 0;
}

cli_cmd_group* cli_add_cmd_grp(cli_list *cli_list_obj, char *name, char *descr, char *help[2], void * func(cli_cmd_group*, cli_cmd_group*)){
	cli_cmd_list **tmp_list = &cli_list_obj->cmd_head;

	if((*tmp_list) != NULL){
		while((*tmp_list)->next != NULL){
			(tmp_list) = &(*tmp_list)->next;
		}

		(*tmp_list)->next = malloc(sizeof(cli_cmd_list));
		memset((*tmp_list)->next, 0, sizeof(cli_cmd_list));
		(*tmp_list)->next->prev = (*tmp_list);
		tmp_list = &(*tmp_list)->next;

	}else{
	
		(*tmp_list) = malloc(sizeof(cli_cmd_list));
		memset((*tmp_list), 0, sizeof(cli_cmd_list));
	}

	_str_malloc_cpy(&(*tmp_list)->item.name, name, NAME_LENGTH);
	_str_malloc_cpy(&(*tmp_list)->item.description, descr, DESCR_LENGTH);
	(*tmp_list)->item.cli_cmd_list_group = malloc(sizeof(cli_cmd_group));
	memset((*tmp_list)->item.cli_cmd_list_group, 0, sizeof(cli_cmd_group));
	(*tmp_list)->item.command_function = func;
	if(help == NULL){
		cli_grp_add_opt((*tmp_list)->item.cli_cmd_list_group, (cli_opt_item){"-h", "--help", "Help option to show this message in command.", FLAG, 0});
	}else{
		cli_grp_add_opt((*tmp_list)->item.cli_cmd_list_group, (cli_opt_item){help[0], help[1], "Help option to show this message in command.", FLAG, 0});
	}
	return (*tmp_list)->item.cli_cmd_list_group;
}

cli_list * cli_init(char *program_desc, char *help[2]){
	cli_list *cli = malloc(sizeof(cli_list));
	memset(cli, 0, sizeof(cli_list));
	_str_malloc_cpy(&cli->prog_description, program_desc, DESCR_LENGTH);
	cli->prog_description[DESCR_LENGTH] = 0;
	cli->opt_arg_grp = malloc(sizeof(cli_cmd_group));
	memset(cli->opt_arg_grp, 0, sizeof(cli_cmd_group));

	if(help == NULL){
		cli_add_opt(cli->opt_arg_grp, (cli_opt_item){"-h", "--help", "Help option to show this message.", FLAG, 0});
	}else{
		cli_add_opt(cli->opt_arg_grp, (cli_opt_item){help[0], help[1], "Help option to show this message.", FLAG, 0});
	}

	return cli;
}

int cli_display_help(char *help_option_small, char *help_option_big){}
int cli_add_opt(cli_cmd_group *cli_group, cli_opt_item opt){
	cli_opt_list **tmp_list = &cli_group->opt_head;
	cli_req_opt **tmp_req = &cli_group->opt_req;

	if((*tmp_list) != NULL){
		while((*tmp_list)->next != NULL){
			(tmp_list) = &(*tmp_list)->next;
		}

		(*tmp_list)->next = malloc(sizeof(cli_opt_list));
		memset((*tmp_list)->next, 0, sizeof(cli_opt_list));
		(*tmp_list)->next->prev = (*tmp_list);
		tmp_list = &(*tmp_list)->next;

	}else{
	
		(*tmp_list) = malloc(sizeof(cli_opt_list));
		memset((*tmp_list), 0, sizeof(cli_opt_list));
	}

	if(strlen(opt.name_small)){
		_str_malloc_cpy(&(*tmp_list)->item.name_small, opt.name_small, NAME_LENGTH);
	}
	if(strlen(opt.name_big)){
		_str_malloc_cpy(&(*tmp_list)->item.name_big, opt.name_big, NAME_LENGTH);
	}
	_str_malloc_cpy(&(*tmp_list)->item.description, opt.description, DESCR_LENGTH);
	(*tmp_list)->item.type_block = opt.type_block;
	(*tmp_list)->item.required = opt.required;

	if(opt.required){
		add_req_opt(&cli_group->opt_req, &(*tmp_list)->item);
	}

	return 0;
}

int cli_add_arg(cli_cmd_group *cli_group, cli_arg_item arg){
	cli_arg_list **tmp_list = &cli_group->arg_head;
	cli_req_arg **tmp_req = &cli_group->arg_req;

	if((*tmp_list) != NULL){
		while((*tmp_list)->next != NULL){
			(tmp_list) = &(*tmp_list)->next;
		}

		(*tmp_list)->next = malloc(sizeof(cli_arg_list));
		memset((*tmp_list)->next, 0, sizeof(cli_arg_list));
		(*tmp_list)->next->prev = (*tmp_list);
		tmp_list = &(*tmp_list)->next;

	}else{
	
		(*tmp_list) = malloc(sizeof(cli_arg_list));
		memset((*tmp_list), 0, sizeof(cli_arg_list));
	}

	_str_malloc_cpy(&(*tmp_list)->item.name, arg.name, NAME_LENGTH);
	_str_malloc_cpy(&(*tmp_list)->item.description, arg.description, DESCR_LENGTH);
	(*tmp_list)->item.type_block = arg.type_block;
	(*tmp_list)->item.required = arg.required;
	
	if(arg.required){
		add_req_arg(&cli_group->arg_req, &(*tmp_list)->item);
	}

	return 0;
}

int cli_help_msg_cmd(cli_cmd_item *cli_list_obj, char **argv){
	cli_cmd_group *tmp_list = cli_list_obj->cli_cmd_list_group;
	cli_opt_list *tmp_opt = tmp_list->opt_head;
	cli_arg_list *tmp_arg = tmp_list->arg_head;


	printf("%s %s\n\t%s\n\n", argv[0], cli_list_obj->name, cli_list_obj->description);

	if(tmp_opt){
		printf("%sOPTIONS:%s\n", BOLD_START, BOLD_END);
		while(tmp_opt != NULL){
			print_space(5);
			if(tmp_opt->item.name_small != NULL){
				printf("%s", tmp_opt->item.name_small);
			}
			if(tmp_opt->item.name_small != NULL && tmp_opt->item.name_big != NULL){
				printf(", ");
			}
			if(tmp_opt->item.name_big != NULL){
				printf("%s", tmp_opt->item.name_big);
			}
			//printf("\n%s\n", tmp_opt->item.description);
			printf("\n");
			print_formatted(tmp_opt->item.description, 8, 90);
			tmp_opt = tmp_opt->next;
		}
	}
	
	if(tmp_arg){
		printf("\n%sARGUMENTS:%s\n", BOLD_START, BOLD_END);
		while(tmp_arg != NULL){
			print_space(5);
			printf("%s\n",
				tmp_arg->item.name);
			print_formatted(tmp_arg->item.description, 8, 90);
			tmp_arg = tmp_arg->next;
		}
	}
}

int cli_help_msg(cli_list *cli_list_obj, char **argv){
	cli_cmd_group *tmp_list = cli_list_obj->opt_arg_grp;
	cli_opt_list *tmp_opt = tmp_list->opt_head;
	cli_arg_list *tmp_arg = tmp_list->arg_head;
	cli_cmd_list *tmp_cmd = cli_list_obj->cmd_head;

	printf("Usage: %s\n\t%s\n\n", argv[0], cli_list_obj->prog_description);

	if(tmp_opt){
		printf("%sOPTIONS:%s\n", BOLD_START, BOLD_END);
		while(tmp_opt != NULL){
			print_space(5);
			if(tmp_opt->item.name_small != NULL){
				printf("%s", tmp_opt->item.name_small);
			}
			if(tmp_opt->item.name_small != NULL && tmp_opt->item.name_big != NULL){
				printf(", ");
			}
			if(tmp_opt->item.name_big != NULL){
				printf("%s ", tmp_opt->item.name_big);
			}
			fprintf_type(stdout, tmp_opt->item.type_block.type, tmp_opt->item.type_block.n);
			printf("\n");
			print_formatted(tmp_opt->item.description, 8, 90);
			tmp_opt = tmp_opt->next;
		}
	}
	
	if(tmp_arg){
		printf("\n%sARGUMENTS:%s\n", BOLD_START, BOLD_END);
		while(tmp_arg != NULL){
			print_space(5);
			printf("%s ", tmp_arg->item.name);
			fprintf_type(stdout, tmp_arg->item.type_block.type, tmp_arg->item.type_block.n);
			printf("\n");
			print_formatted(tmp_arg->item.description, 8, 90);
			tmp_arg = tmp_arg->next;
		}
	}

	if(tmp_cmd){
		printf("\n%sCOMMANDS:%s\n", BOLD_START, BOLD_END);
		while(tmp_cmd != NULL){
			print_space(5);
			printf("%s\n",
				tmp_cmd->item.name);
			print_formatted(tmp_cmd->item.description, 8, 90);
			tmp_cmd = tmp_cmd->next;
		}
	}
	return 0;
}

cli_opt_list* find_opt_name(cli_opt_list *opt_list, char *search_name){
	cli_opt_list *tmp = opt_list;
	while(tmp != NULL){
		if(tmp->item.name_small && !strcmp(tmp->item.name_small, search_name)){
			return tmp;
		}
		if(tmp->item.name_big && !strcmp(tmp->item.name_big, search_name)){
			return tmp;
		}
		tmp = tmp->next;
	}
	return NULL;
}

cli_arg_list* find_arg_name(cli_arg_list *arg_list, char *search_name){
	cli_arg_list *tmp = arg_list;
	while(tmp != NULL){
		if(!strncmp(tmp->item.name, search_name, NAME_LENGTH)){
			return tmp;
		}
		tmp = tmp->next;
	}
	return NULL;
}

cli_cmd_list* find_cmd_name(cli_cmd_list *cmd_list, char *search_name){
	size_t i = 0;
	cli_cmd_list *tmp = cmd_list;
	while(tmp != NULL){
		if(!strncmp(tmp->item.name, search_name, NAME_LENGTH)){
			return tmp;
		}
		i++;
		tmp = tmp->next;
	}
	return NULL;
}

int check_opt_type(cli_cmd_group *grp, cli_opt_list *tmp_opt, char *argv, int *list_index){
	switch(tmp_opt->item.type_block.type){
		case FLAG:
			if(tmp_opt->result == NULL){
				tmp_opt->result = malloc(sizeof(cli_result));
			}
			*tmp_opt->result = (cli_result) 1;
			remove_req_opt(&grp->opt_req, &tmp_opt->item);
			return -1;
		break;
		case BOOL:
			if(argv == NULL) return -2;

			char *bool_values[] = {"yes", "no", "true", "false", "y", "n"};
			for(size_t i = 0; i < 6; i++){
				if(!strcmp(bool_values[i], argv)){
					if(tmp_opt->result == NULL){
						tmp_opt->result = malloc(sizeof(cli_result));
					}
					if(!(i%2)){
						*tmp_opt->result = (cli_result) 1;
						remove_req_opt(&grp->opt_req, &tmp_opt->item);
						return 0;
					}else if(i%2){
						*tmp_opt->result = (cli_result) 0;
						remove_req_opt(&grp->opt_req, &tmp_opt->item);
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
			if(tmp_opt->result == NULL){
				tmp_opt->result = malloc(sizeof(cli_result));
			}
			if(list_index == NULL){
				*tmp_opt->result = (cli_result) atoi(argv);
				remove_req_opt(&grp->opt_req, &tmp_opt->item);
			}else{
				(*tmp_opt->result).li[*list_index] = atoi(argv);
				if(*list_index == tmp_opt->item.type_block.n - 1){
					remove_req_opt(&grp->opt_req, &tmp_opt->item);
				}
			}
			return 0;
		break;
		case DOUBLE:
			if(argv == NULL) return -2;

			char *endptr;
			double val = strtod(argv, &endptr);
			if (!strlen(endptr)){
				if(tmp_opt->result == NULL){
					tmp_opt->result = malloc(sizeof(cli_result));
				}
				if(list_index == NULL){
					*tmp_opt->result = (cli_result) val;
					remove_req_opt(&grp->opt_req, &tmp_opt->item);
				}else{
					(*tmp_opt->result).ld[*list_index] = val;
					if(*list_index == tmp_opt->item.type_block.n - 1){
						remove_req_opt(&grp->opt_req, &tmp_opt->item);
					}
				}
				return 0;
			}
			return -2;
		break;
		case STRING:
			if(argv == NULL) return -2;
			if(tmp_opt->result == NULL){
				tmp_opt->result = malloc(sizeof(cli_result));
			}
			if(list_index == NULL){
				*tmp_opt->result = (cli_result) argv;
				remove_req_opt(&grp->opt_req, &tmp_opt->item);
			}else{
				if(argv[0] == '-'){
					return -2;
				}
				(*tmp_opt->result).ls[*list_index] = argv;
				if(*list_index == tmp_opt->item.type_block.n - 1){
					remove_req_opt(&grp->opt_req, &tmp_opt->item);
				}
			}
			return 0;
		break;
	}
}

void add_req_opt(cli_req_opt **list, cli_opt_item *item){
	cli_req_opt **tmp_req = list;
	if((*tmp_req) != NULL){
		while((*tmp_req)->next != NULL){
			(tmp_req) = &(*tmp_req)->next;
		}

		(*tmp_req)->next = malloc(sizeof(cli_req_opt));
		memset((*tmp_req)->next, 0, sizeof(cli_req_opt));
		(*tmp_req)->next->prev = (*tmp_req);
		tmp_req = &(*tmp_req)->next;

	}else{

		(*tmp_req) = malloc(sizeof(cli_req_opt));
		memset((*tmp_req), 0, sizeof(cli_req_opt));
	}
	(*tmp_req)->item = item;
}

void remove_req_opt(cli_req_opt **list, cli_opt_item *item){
	cli_req_opt **tmp_req = list;
	if(item == NULL) return;
	if((*tmp_req) == NULL) return;

	while((*tmp_req) != NULL){
		if((*tmp_req)->item == item){
			cli_req_opt *tmp = (*tmp_req);
			if((*tmp_req)->next != NULL){
				(*tmp_req)->next->prev = (*tmp_req)->prev;
			}
			if((*tmp_req)->prev != NULL){
				(*tmp_req)->prev->next = (*tmp_req)->next;
			}else{
				tmp = (*tmp_req);
				(*tmp_req) = (*tmp_req)->next;
			}
			*tmp = (cli_req_opt){0};
			free(tmp);
			break;
		}
		(tmp_req) = &(*tmp_req)->next;
	}
}


int check_arg_type(cli_cmd_group *grp, cli_arg_list *tmp_arg, char *argv, int *list_index){
	switch(tmp_arg->item.type_block.type){
		case FLAG:
			if(tmp_arg->result == NULL){
				tmp_arg->result = malloc(sizeof(cli_result));
			}
			*tmp_arg->result = (cli_result) 1;
			remove_req_arg(&grp->arg_req, &tmp_arg->item);
			return -1;
		break;
		case BOOL:
			if(argv == NULL) return -2;

			char *bool_values[] = {"yes", "no", "true", "false", "y", "n"};
			for(size_t i = 0; i < 6; i++){
				if(!strcmp(bool_values[i], argv)){
					if(tmp_arg->result == NULL){
						tmp_arg->result = malloc(sizeof(cli_result));
					}
					if(list_index == NULL){
						if(!(i%2)){
							*tmp_arg->result = (cli_result) 1;
							remove_req_arg(&grp->arg_req, &tmp_arg->item);
							return 0;
						}else if(i%2){
							*tmp_arg->result = (cli_result) 0;
							if(*list_index == tmp_arg->item.type_block.n - 1){
								remove_req_arg(&grp->arg_req, &tmp_arg->item);
							}
							return 0;
						}
					}else{
						if(!(i%2)){
							(*tmp_arg->result).li[*list_index] = 1;
							remove_req_arg(&grp->arg_req, &tmp_arg->item);
							return 0;
						}else if(i%2){
							(*tmp_arg->result).li[*list_index] = 0;
							if(*list_index == tmp_arg->item.type_block.n - 1){
								remove_req_arg(&grp->arg_req, &tmp_arg->item);
							}
							return 0;
						}
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
				tmp_arg->result = malloc(sizeof(cli_result));
			}
			if(list_index == NULL){
				*tmp_arg->result = (cli_result) atoi(argv);
				remove_req_arg(&grp->arg_req, &tmp_arg->item);
			}else{
				(*tmp_arg->result).li[*list_index] = atoi(argv);
				if(*list_index == tmp_arg->item.type_block.n - 1){
					remove_req_arg(&grp->arg_req, &tmp_arg->item);
				}
			}
			return 0;
		break;
		case DOUBLE:
			if(argv == NULL) return -2;

			char *endptr;
			double val = strtod(argv, &endptr);
			if (!strlen(endptr)){
				if(tmp_arg->result == NULL){
					tmp_arg->result = malloc(sizeof(cli_result));
				}
				if(list_index == NULL){
					*tmp_arg->result = (cli_result) val;
					remove_req_arg(&grp->arg_req, &tmp_arg->item);
				}else{
					(*tmp_arg->result).ld[*list_index] = val;
					if(*list_index == tmp_arg->item.type_block.n - 1){
						remove_req_arg(&grp->arg_req, &tmp_arg->item);
					}
				}
				return 0;
			}
			return -2;
		break;
		case STRING:
			if(argv == NULL) return -2;
			if(tmp_arg->result == NULL){
				tmp_arg->result = malloc(sizeof(cli_result));
			}
			if(list_index == NULL){
				*tmp_arg->result = (cli_result) argv;
				strcpy(tmp_arg->result->s, argv);
				remove_req_arg(&grp->arg_req, &tmp_arg->item);
			}else{
				strcpy(tmp_arg->result->ls[*list_index], argv);
				if(*list_index == tmp_arg->item.type_block.n - 1){
					remove_req_arg(&grp->arg_req, &tmp_arg->item);
				}
			}
			return 0;
		break;
	}
}

void add_req_arg(cli_req_arg **list, cli_arg_item *item){
	cli_req_arg **tmp_req = list;
	if((*tmp_req) != NULL){
		while((*tmp_req)->next != NULL){
			(tmp_req) = &(*tmp_req)->next;
		}

		(*tmp_req)->next = malloc(sizeof(cli_req_arg));
		memset((*tmp_req)->next, 0, sizeof(cli_req_arg));
		(*tmp_req)->next->prev = (*tmp_req);
		tmp_req = &(*tmp_req)->next;

	}else{

		(*tmp_req) = malloc(sizeof(cli_req_arg));
		memset((*tmp_req), 0, sizeof(cli_req_arg));
	}
	(*tmp_req)->item = item;
}

void remove_req_arg(cli_req_arg **list, cli_arg_item *item){
	cli_req_arg **tmp_req = list;
	if(item == NULL) return;
	if((*tmp_req) == NULL) return;

	while((*tmp_req) != NULL){
		if((*tmp_req)->item == item){
			cli_req_arg *tmp = (*tmp_req);
			if((*tmp_req)->next != NULL){
				(*tmp_req)->next->prev = (*tmp_req)->prev;
			}
			if((*tmp_req)->prev != NULL){
				(*tmp_req)->prev->next = (*tmp_req)->next;
			}else{
				tmp = (*tmp_req);
				(*tmp_req) = (*tmp_req)->next;
			}
			*tmp = (cli_req_arg){0};
			free(tmp);
			break;
		}
		(tmp_req) = &(*tmp_req)->next;
	}
}

void allocate_result_list_arg(cli_arg_list *node){
	node->result = malloc(sizeof(cli_result));
	memset(node->result, 0, sizeof(cli_result));
	switch(node->item.type_block.type){
		case INT:
			node->result->li = malloc(sizeof(int) * node->item.type_block.n);
			break;
		case BOOL:
			node->result->lb = malloc(sizeof(int) * node->item.type_block.n);
			break;
		case DOUBLE:
			node->result->ld = malloc(sizeof(double) * node->item.type_block.n);
			break;
		case STRING:
			node->result->ls = malloc(sizeof(char*) * (node->item.type_block.n + 1));
			for(size_t i = 0; i < node->item.type_block.n + 1; i++){
				node->result->ls[i] = malloc(sizeof(char) * DESCR_LENGTH);
			}
			break;
	}
}

void allocate_result_list_opt(cli_opt_list *node){
	node->result = malloc(sizeof(cli_result));
	memset(node->result, 0, sizeof(cli_result));
	switch(node->item.type_block.type){
		case INT:
			node->result->li = malloc(sizeof(int) * node->item.type_block.n);
			break;
		case BOOL:
			node->result->lb = malloc(sizeof(int) * node->item.type_block.n);
			break;
		case DOUBLE:
			node->result->ld = malloc(sizeof(double) * node->item.type_block.n);
			break;
		case STRING:
			node->result->ls = malloc(sizeof(char*) * (node->item.type_block.n + 1));
			for(size_t i = 0; i < node->item.type_block.n + 1; i++){
				node->result->ls[i] = NULL;
			}
			break;
	}
}

int insert_result_opt(cli_cmd_group *grp_list, cli_opt_list *found_opt, int argc, char **argv, int *arg_index, int *list_index){
	int check_res = 0;
	if(*arg_index + 1 < argc){
		check_res = check_opt_type(grp_list, found_opt, argv[*arg_index+1], list_index);
	}else{
		check_res = check_opt_type(grp_list, found_opt, NULL, list_index);
	}
	if(check_res > -1){
		(*arg_index)++;
	}else if(check_res == -2){
		#ifndef CLI_MUTE
		if(list_index == NULL){
			fprintf(stderr, "Option '%s' requires argument of type ", argv[*arg_index]);
		}else{
			fprintf(stderr, "Option '%s' requires argument of list type ", found_opt->item.name_small == NULL ? found_opt->item.name_big : found_opt->item.name_small);
		}
		fprintf_type(stderr, found_opt->item.type_block.type, found_opt->item.type_block.n);	
		if(list_index == NULL){
			fprintf(stderr, "\n");
		}else{
			fprintf(stderr, " at position %d\n", *list_index);
		}
		#endif // CLI_MUTE
		return 0;
	}
	return 1;
}

int cli_opt_parser(cli_list *list, cli_cmd_item *grp, int argc, char **argv, int *index){
	cli_cmd_group *grp_list;
	if(list != NULL){
		grp_list = list->opt_arg_grp;
	}else if(grp_list != NULL){
		grp_list = grp->cli_cmd_list_group;
	}
	cli_opt_list *opt_head = grp_list->opt_head;

	for(; *index < argc; (*index)++){
		cli_opt_list *found_opt = find_opt_name(opt_head, argv[*index]);
		if(found_opt != NULL){
			if(help_msg_found(opt_head, found_opt)){
				if(list != NULL){
					cli_help_msg(list, argv);
				}else if(grp_list != NULL){
					cli_help_msg_cmd(grp, argv);
				}
				exit(0);
			}
			if(found_opt->item.type_block.n == 0){
				if(!insert_result_opt(grp_list, found_opt, argc, argv, index, NULL)){
					return 0;
				}
			}else if(found_opt->item.type_block.n > 0){
				int *list_index = malloc(sizeof(int));
				allocate_result_list_opt(found_opt);
				for(int i = 0; i < found_opt->item.type_block.n; i++){
					*list_index = i;
					if(!insert_result_opt(grp_list, found_opt, argc, argv, index, list_index)){
						return 0;
					}
				}
				free(list_index);
			}
		}else{
			if(argv[*index][0] == '-'){
				#ifndef CLI_MUTE
				fprintf(stderr, "Option '%s' not found.\n", argv[*index]);
				#endif
				return 0;
			}
			break;
		}
	}
	return 1;
}

int insert_result_arg(cli_cmd_group *grp_list, cli_arg_list *tmp_arg, int argc, char **argv, int *arg_index, int *list_index){
	int check_res = check_arg_type(grp_list, tmp_arg, argv[*arg_index], list_index);
	if(check_res <= -1){
		#ifndef CLI_MUTE
		if(list_index == NULL){
			fprintf(stderr, "Argument '%s' requires type ", argv[*arg_index]);
		}else{
			fprintf(stderr, "Argument '%s' requires list type ", tmp_arg->item.name);
		}
		fprintf_type(stderr, tmp_arg->item.type_block.type, tmp_arg->item.type_block.n);
		if(list_index == NULL){
			fprintf(stderr, "\n");
		}else{
			fprintf(stderr, " at position %d\n", *list_index);
		}
		#endif // CLI_MUTE
		return 0;
	}
	return 1;
}

//TODO: Maybe think about a way of changing the ARG positions based on them being required
//		but only if the minimal number of requirements are met
// 		exp: 
// 								arg0 arg1[req] arg2 arg3[req]
// 			only 2 arg given -> arg1[req] arg3[req]
// 			only 3 arg given -> error ??? maybe
// 								arg1[req] arg3[req]
//
int cli_arg_parser(cli_cmd_group *grp_list, int argc, char **argv, int *index){
	if(*index >= argc) return 1;

	cli_arg_list *tmp_arg = grp_list->arg_head;
	for(; *index < argc; (*index)++){
		if(tmp_arg == NULL) return 0;
		if(tmp_arg->item.type_block.n == 0){
			if(!insert_result_arg(grp_list, tmp_arg, argc, argv, index, NULL)){
				return 0;
			}
		}else if(tmp_arg->item.type_block.n > 0){
			int *list_index = malloc(sizeof(int));
			allocate_result_list_arg(tmp_arg);
			for(int i = 0; i < tmp_arg->item.type_block.n; i++){
				*list_index = i;
				if(!insert_result_arg(grp_list, tmp_arg, argc, argv, index, list_index)){
					return 0;
				}
				(*index)++;
			}
			(*index)--;
			free(list_index);
		}else{
			tmp_arg->item.type_block.n = argc - *index;
			int *list_index = malloc(sizeof(int));
			int check_res = 0;
			allocate_result_list_arg(tmp_arg);
			for(int i = 0; i < argc - *index; i++){
				*list_index = i;
				check_res = check_arg_type(grp_list, tmp_arg, argv[*index + i], list_index);
				if(check_res <= -1){ 
					fprintf(stderr, "Wrong type '%s' at index [%d].\n", argv[*index + i], i);
					break;
				}
			}
			free(list_index);
			*index = argc;
			return check_res > -1;
		}
		tmp_arg = tmp_arg->next;
	}

	return 1;
}

int cli_cmd_parser(cli_cmd_group *master_grp, cli_cmd_list *cmd_head, int argc, char **argv, int *index){
	if(*index >= argc) return 0;

	cli_cmd_list *found_cmd = find_cmd_name(cmd_head, argv[*index]);
	if(found_cmd != NULL){
		(*index)++;
		int res_opt = cli_opt_parser(NULL, &found_cmd->item, argc, argv, index);
		cli_req_opt *req_opt = found_cmd->item.cli_cmd_list_group->opt_req;
		if(!res_opt || req_opt != NULL) return -1;
		int res_arg = cli_arg_parser(found_cmd->item.cli_cmd_list_group, argc, argv, index);
		cli_req_arg *req_arg = found_cmd->item.cli_cmd_list_group->arg_req;
		if(!res_arg || req_arg != NULL) return -1;
		if(req_opt != NULL){
			#ifndef CLI_MUTE
			fprintf(stderr, "Required Option found: (%s/%s)\n", req_opt->item->name_small, req_opt->item->name_big);
			#endif
			return -1;
		}
		if(req_arg != NULL){
			#ifndef CLI_MUTE
			fprintf(stderr, "Required Argument found: %s\n", req_arg->item->name);
			#endif
			return -1;
		}
		if(found_cmd->item.command_function != NULL){
			found_cmd->item.command_function(master_grp, found_cmd->item.cli_cmd_list_group);
		}

		return 1;
	}

	return 0;
}

int help_msg_found(cli_opt_list *opt_head, cli_opt_list *node){
	if(node->item.name_small == NULL || node->item.name_big == NULL) return 0;
	if(strcmp((opt_head)->item.name_small, node->item.name_small) == 0 &&
	strcmp((opt_head)->item.name_big, node->item.name_big) == 0){
		return 1;
	}
	return 0;
}

// [PROGRAM]: [OPTIONS | OPTIONS=args]* ([COMMANDS]? | [ARGUMENTS]*)
// [COMMANDS]: [OPTIONS | OPTIONS=args]* [ARGUMENTS]*
int cli_execute(cli_list *cli_list_obj, int argc, char **argv){
	cli_list *tmp_list = cli_list_obj;
	cli_opt_list *tmp_opt = tmp_list->opt_arg_grp->opt_head;
	cli_arg_list *tmp_arg = tmp_list->opt_arg_grp->arg_head;
	cli_cmd_list *tmp_cmd = tmp_list->cmd_head;
	
	cli_cmd_group *grp = cli_list_obj->opt_arg_grp;
	
	/*if(argc < 2 && (grp->opt_req != NULL || grp->arg_req != NULL)){
		#ifndef CLI_MUTE
		cli_help_msg(cli_list_obj, argv);
		#endif // CLI_MUTE
		exit(1);
	}*/

	tmp_opt = tmp_opt->next;
	int i = 1;
	if(!cli_opt_parser(cli_list_obj, NULL, argc, argv, &i)) return 0;
	if(grp->opt_req != NULL){
		#ifndef CLI_MUTE
		fprintf(stderr, "Required Option found: (%s/%s)\n", grp->opt_req->item->name_small, grp->opt_req->item->name_big);
		#endif
		return 0;
	}
	int cmd_parse_res = cli_cmd_parser(grp, tmp_cmd, argc, argv, &i);
	if(cmd_parse_res == 0){
		if(!cli_arg_parser(cli_list_obj->opt_arg_grp, argc, argv, &i)) return 0;
		if(grp->arg_req != NULL){ 
			#ifndef CLI_MUTE
			fprintf(stderr, "Required Argument found: %s\n", grp->arg_req->item->name);
			#endif
			return 0;
		}
		return 1;
	}else if(cmd_parse_res == 1 && i == argc){
		return 1;
	}

	return 0;
}

int cli_destroy(cli_list *cli_list_obj){
	cli_opt_list *tmp_opt = cli_list_obj->opt_arg_grp->opt_head;
	cli_arg_list *tmp_arg = cli_list_obj->opt_arg_grp->arg_head;
	cli_req_opt *tmp_opt_req = cli_list_obj->opt_arg_grp->opt_req;
	cli_req_arg *tmp_arg_req = cli_list_obj->opt_arg_grp->arg_req;
	cli_cmd_group *tmp_grp = cli_list_obj->opt_arg_grp;
	cli_cmd_list *tmp_cmd = cli_list_obj->cmd_head;
	//TODO: cleanup result->ls allocated memory
	if(tmp_opt != NULL){
		while(tmp_opt->next != NULL){
			tmp_opt = tmp_opt->next;
		}
		while(tmp_opt != NULL){
			cli_opt_list *tmp = tmp_opt;
			free(tmp->item.name_small);
			free(tmp->item.name_big);
			free(tmp->item.description);
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
			free(tmp->item.name);
			free(tmp->item.description);
			free(tmp->result);
			tmp_arg = tmp_arg->prev;
			free(tmp);
		}
	}
	if(tmp_opt_req != NULL){
		while(tmp_opt_req->next != NULL){
			tmp_opt_req = tmp_opt_req->next;
		}
		while(tmp_opt_req != NULL){
			cli_req_opt *tmp = tmp_opt_req;
			tmp_opt_req = tmp_opt_req->prev;
			free(tmp);
		}
	}
	if(tmp_arg_req != NULL){
		while(tmp_arg_req->next != NULL){
			tmp_arg_req = tmp_arg_req->next;
		}
		while(tmp_arg_req != NULL){
			cli_req_arg *tmp = tmp_arg_req;
			tmp_arg_req = tmp_arg_req->prev;
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
			cli_req_opt *tmp_cmd_opt_req = tmp_cmd->item.cli_cmd_list_group->opt_req;
			cli_req_arg *tmp_cmd_arg_req = tmp_cmd->item.cli_cmd_list_group->arg_req;
			cli_cmd_group *tmp_cmd_grp = tmp_cmd->item.cli_cmd_list_group;

			if(tmp_cmd_opt != NULL){
				while(tmp_cmd_opt->next != NULL){
					tmp_cmd_opt = tmp_cmd_opt->next;
				}
				while(tmp_cmd_opt != NULL){
					cli_opt_list *tmp = tmp_cmd_opt;
					free(tmp->item.name_small);
					free(tmp->item.name_big);
					free(tmp->item.description);
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
					free(tmp->item.name);
					free(tmp->item.description);
					free(tmp->result);
					tmp_cmd_arg = tmp_cmd_arg->prev;
					free(tmp);
				}
			}
			if(tmp_cmd_opt_req != NULL){
				while(tmp_cmd_opt_req->next != NULL){
					tmp_cmd_opt_req = tmp_cmd_opt_req->next;
				}
				while(tmp_cmd_opt_req != NULL){
					cli_req_opt *tmp = tmp_cmd_opt_req;
					tmp_cmd_opt_req = tmp_cmd_opt_req->prev;
					free(tmp);
				}
			}
			if(tmp_cmd_arg_req != NULL){
				while(tmp_cmd_arg_req->next != NULL){
					tmp_cmd_arg_req = tmp_cmd_arg_req->next;
				}
				while(tmp_cmd_arg_req != NULL){
					cli_req_arg *tmp = tmp_cmd_arg_req;
					tmp_cmd_arg_req = tmp_cmd_arg_req->prev;
					free(tmp);
				}
			}

			cli_cmd_list *tmp = tmp_cmd;
			free(tmp->item.name);
			free(tmp->item.description);
			free(tmp->item.cli_cmd_list_group);
			tmp_cmd = tmp_cmd->prev;
			free(tmp);
		}
	}
	free(tmp_grp);
	free(cli_list_obj->prog_description);
	free(cli_list_obj);
}

#endif // CLI_IMPLEMENTATION

#endif // CLI_H
