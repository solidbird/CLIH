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

#define IMPLEMENT_IDENTICAL_FUNCTIONS(TYPE) \
	int check_##TYPE##_type(cli_cmd_group *grp, cli_##TYPE##_list *tmp_##TYPE, char *argv){ \
		switch(tmp_##TYPE->item.type){ \
			case FLAG: \
				if(tmp_##TYPE->result == NULL){ \
					tmp_##TYPE->result = malloc(sizeof(uint64_t)); \
				} \
				*tmp_##TYPE->result = 1; \
				remove_req_##TYPE(&grp->TYPE##_req, &tmp_##TYPE->item); \
				return -1; \
			break; \
			case BOOL: \
				if(argv == NULL) return -2; \
	\
				char *bool_values[] = {"yes", "no", "true", "false", "y", "n"}; \
				for(size_t i = 0; i < 6; i++){ \
					if(!strcmp(bool_values[i], argv)){ \
						if(tmp_##TYPE->result == NULL){ \
							tmp_##TYPE->result = malloc(sizeof(uint64_t)); \
						} \
						if(!(i%2)){ \
							*tmp_##TYPE->result = 1; \
							remove_req_##TYPE(&grp->TYPE##_req, &tmp_##TYPE->item); \
							return 0; \
						}else if(i%2){ \
							*tmp_##TYPE->result = 0; \
							remove_req_##TYPE(&grp->TYPE##_req, &tmp_##TYPE->item); \
							return 0; \
						} \
					} \
				} \
				return -2; \
			break; \
			case INT: \
				if(argv == NULL) return -2; \
	\
				for(int i = 0; i < strlen(argv); i++){ \
					if(!isdigit(argv[i])) \
						return -2; \
				} \
				if(tmp_##TYPE->result == NULL){ \
					tmp_##TYPE->result = malloc(sizeof(uint64_t)); \
				} \
				*tmp_##TYPE->result = atoi(argv); \
				remove_req_##TYPE(&grp->TYPE##_req, &tmp_##TYPE->item); \
				return 0; \
			break; \
			case DOUBLE: \
				if(argv == NULL) return -2; \
	\
				char *endptr; \
				double val = strtod(argv, &endptr); \
				if (!strlen(endptr)){ \
					if(tmp_##TYPE->result == NULL){ \
						tmp_##TYPE->result = malloc(sizeof(uint64_t)); \
					} \
					*tmp_##TYPE->result = val; \
					remove_req_##TYPE(&grp->TYPE##_req, &tmp_##TYPE->item); \
					return 0; \
				} \
				return -2; \
			break; \
			case STRING: \
				if(argv == NULL) return -2; \
				if(tmp_##TYPE->result == NULL){ \
					tmp_##TYPE->result = malloc(sizeof(uint64_t)); \
				} \
				*tmp_##TYPE->result = argv; \
				remove_req_##TYPE(&grp->TYPE##_req, &tmp_##TYPE->item); \
				return 0; \
			break; \
		} \
	} \
\
\
	void add_req_##TYPE(cli_req_##TYPE **list, cli_##TYPE##_item *item){ \
		cli_req_##TYPE **tmp_req = list; \
		if((*tmp_req) != NULL){ \
			while((*tmp_req)->next != NULL){ \
				(tmp_req) = &(*tmp_req)->next; \
			} \
	\
			(*tmp_req)->next = malloc(sizeof(cli_req_##TYPE)); \
			(*tmp_req)->next->prev = (*tmp_req); \
			tmp_req = &(*tmp_req)->next; \
	\
		}else{ \
	\
			(*tmp_req) = malloc(sizeof(cli_req_##TYPE)); \
		} \
		(*tmp_req)->item = item; \
	} \
\
\
	void remove_req_##TYPE(cli_req_##TYPE **list, cli_##TYPE##_item *item){ \
		cli_req_##TYPE **tmp_req = list; \
		if(item == NULL) return; \
		if((*tmp_req) == NULL) return; \
	\
		while((*tmp_req) != NULL){ \
			if((*tmp_req)->item == item){ \
				cli_req_##TYPE *tmp = (*tmp_req); \
				if((*tmp_req)->next != NULL){ \
					(*tmp_req)->next->prev = (*tmp_req)->prev; \
				} \
				if((*tmp_req)->prev != NULL){ \
					(*tmp_req)->prev->next = (*tmp_req)->next; \
				}else{ \
					tmp = (*tmp_req); \
					(*tmp_req) = (*tmp_req)->next; \
				} \
				*tmp = (cli_req_##TYPE){0}; \
				free(tmp); \
				break; \
			} \
			(tmp_req) = &(*tmp_req)->next; \
		} \
	}

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

typedef struct cli_example_item {
	char description[DESCR_LENGTH];
	char file_path[DESCR_LENGTH];
} cli_example_item;

typedef struct cli_examples {
	cli_example_item item;
	struct cli_examples *next;
	struct cli_examples *prev;
} cli_examples;

typedef struct cli_list {
	char prog_description[DESCR_LENGTH];
	cli_cmd_group *opt_arg_grp;
	cli_cmd_list *cmd_head;
	cli_examples *examples;
} cli_list;


int cli_init(cli_list *cli_list_obj, const char *program_desc, char *help[2]);
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
int cli_execute(cli_list *cli_list_obj, int argc, char **argv);

cli_opt_list* find_opt_name(cli_opt_list *opt_list, char search_name[NAME_LENGTH]);
cli_arg_list* find_arg_name(cli_arg_list *arg_list, char search_name[NAME_LENGTH]);
cli_cmd_list* find_cmd_name(cli_cmd_list *cmd_list, char search_name[NAME_LENGTH]);

int check_opt_type(cli_cmd_group *grp, cli_opt_list *tmp_opt, char *argv);
int check_arg_type(cli_cmd_group *grp, cli_arg_list *tmp_arg, char *argv);

int cli_set_default(cli_cmd_group group_head, char* search_name, uint64_t default_value);
void add_req_arg(cli_req_arg **list, cli_arg_item *item);
void remove_req_arg(cli_req_arg **list, cli_arg_item *item);
void add_req_opt(cli_req_opt **list, cli_opt_item *item);
void remove_req_opt(cli_req_opt **list, cli_opt_item *item);

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

int cli_grp_add_opt(cli_cmd_group *cli_group, cli_opt_item opt){
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

	if(strlen(opt.name_small)){
		strncpy((*tmp_list)->item.name_small, opt.name_small, NAME_LENGTH);
	}
	if(strlen(opt.name_big)){
		strncpy((*tmp_list)->item.name_big, opt.name_big, NAME_LENGTH);
	}
	strncpy((*tmp_list)->item.description, opt.description, DESCR_LENGTH);
	(*tmp_list)->item.type = opt.type;
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
		(*tmp_list)->next->prev = (*tmp_list);
		tmp_list = &(*tmp_list)->next;

	}else{
	
		(*tmp_list) = malloc(sizeof(cli_arg_list));
	}

	strncpy((*tmp_list)->item.name, arg.name, NAME_LENGTH);
	strncpy((*tmp_list)->item.description, arg.description, DESCR_LENGTH);
	(*tmp_list)->item.type = arg.type;
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
		(*tmp_list)->next->prev = (*tmp_list);
		tmp_list = &(*tmp_list)->next;

	}else{
	
		(*tmp_list) = malloc(sizeof(cli_cmd_list));
	}

	strncpy((*tmp_list)->item.name, name, NAME_LENGTH);
	strncpy((*tmp_list)->item.description, descr, DESCR_LENGTH);
	(*tmp_list)->item.cli_cmd_list_group = malloc(sizeof(cli_cmd_group));
	(*tmp_list)->item.command_function = func;
	cli_grp_add_opt((*tmp_list)->item.cli_cmd_list_group, (cli_opt_item){help[0], help[1], "This is a help message.", FLAG});

	return (*tmp_list)->item.cli_cmd_list_group;
}

int cli_init(cli_list *cli_list_obj, const char *program_desc, char *help[2]){
	strncpy(cli_list_obj->prog_description, program_desc, DESCR_LENGTH);
	cli_list_obj->opt_arg_grp = malloc(sizeof(cli_cmd_group));
	cli_list_obj->cmd_head = NULL;

	if(help == NULL){
		cli_add_opt(cli_list_obj->opt_arg_grp, (cli_opt_item){"-h", "--help", "Help option to show this message.", FLAG});
	}else{
		cli_add_opt(cli_list_obj->opt_arg_grp, (cli_opt_item){help[0], help[1], "Help option to show this message.", FLAG});
	}

	return 0;
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
		(*tmp_list)->next->prev = (*tmp_list);
		tmp_list = &(*tmp_list)->next;

	}else{
	
		(*tmp_list) = malloc(sizeof(cli_arg_list));
	}

	strncpy((*tmp_list)->item.name, arg.name, NAME_LENGTH);
	strncpy((*tmp_list)->item.description, arg.description, DESCR_LENGTH);
	(*tmp_list)->item.type = arg.type;
	(*tmp_list)->item.required = arg.required;
	
	if(arg.required){
		add_req_arg(&cli_group->arg_req, &(*tmp_list)->item);
	}

	return 0;
}

int cli_help_msg(cli_list *cli_list_obj, char **argv){
	cli_list *tmp_list = cli_list_obj;
	cli_opt_list *tmp_opt = tmp_list->opt_arg_grp->opt_head;
	cli_arg_list *tmp_arg = tmp_list->opt_arg_grp->arg_head;
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

IMPLEMENT_IDENTICAL_FUNCTIONS(opt)
IMPLEMENT_IDENTICAL_FUNCTIONS(arg)

int cli_opt_parser(cli_cmd_group *grp_list, int argc, char **argv, int *index){
	cli_opt_list *opt_head = grp_list->opt_head;

	for(; *index < argc; (*index)++){
		cli_opt_list *found_opt = find_opt_name(opt_head, argv[*index]);
		if(found_opt != NULL){
			int check_res = 0;
			if(*index + 1 < argc){
				check_res = check_opt_type(grp_list, found_opt, argv[*index+1]);
			}else{
				check_res = check_opt_type(grp_list, found_opt, NULL);
			}
			if(check_res > -1){
				(*index)++;
			}else if(check_res == -2){
				#ifndef CLI_MUTE
				printf("Option '%s' requires argument of type ", argv[*index]);
				switch(found_opt->item.type){
					case BOOL:
						printf("BOOL");
					break;
					case STRING:
						printf("STRING");
					break;
					case INT:
						printf("INT");
					break;
					case DOUBLE:
						printf("DOUBLE");
					break;
				}
				printf("\n");
				#endif // CLI_MUTE
				return 0;
			}
		}else{
			if(argv[*index][0] == '-'){
				#ifndef CLI_MUTE
				printf("Option '%s' not found.\n", argv[*index]);
				#endif
				return 0;
			}
			break;
		}
	}
	return 1;
}

int cli_arg_parser(cli_cmd_group *grp_list, int argc, char **argv, int *index){
	if(*index >= argc) return 1;

	cli_arg_list *tmp_arg = grp_list->arg_head;
	for(; *index < argc; (*index)++){
		if(tmp_arg == NULL) return 0;
		int check_res = check_arg_type(grp_list, tmp_arg, argv[*index]);
		if(check_res <= -1){
			#ifndef CLI_MUTE
			printf("Argument %s = '%s' requires argument of type ", tmp_arg->item.name, argv[*index]);
			switch(tmp_arg->item.type){
				case BOOL:
					printf("BOOL");
				break;
				case STRING:
					printf("STRING");
				break;
				case INT:
					printf("INT");
				break;
				case DOUBLE:
					printf("DOUBLE");
				break;
			}
			printf("\n");
			#endif // CLI_MUTE
			return 0;
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
		int res_opt = cli_opt_parser(found_cmd->item.cli_cmd_list_group, argc, argv, index);
		cli_req_opt *req_opt = found_cmd->item.cli_cmd_list_group->opt_req;
		if(!res_opt && req_opt != NULL) return -1;
		int res_arg = cli_arg_parser(found_cmd->item.cli_cmd_list_group, argc, argv, index);
		cli_req_arg *req_arg = found_cmd->item.cli_cmd_list_group->arg_req;
		if(!res_arg && req_arg != NULL) return -1;
		if(req_opt != NULL){
			printf("Required Option found: %s%s\n", req_opt->item->name_small, req_opt->item->name_big);
			return -1;
		}
		if(req_arg != NULL){
			printf("Required Argument found: %s\n", req_arg->item->name);
			return -1;
		}
		if(found_cmd->item.command_function != NULL){
			found_cmd->item.command_function(master_grp, found_cmd->item.cli_cmd_list_group);
		}

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

	for(size_t i = 1; i < argc; i++){
		if(strcmp((tmp_opt)->item.name_small, argv[i]) == 0 ||
		strcmp((tmp_opt)->item.name_big, argv[i]) == 0){
			cli_help_msg(cli_list_obj, argv);
			exit(0);
		}
	}

	tmp_opt = tmp_opt->next;
	int i = 1;
	if(!cli_opt_parser(cli_list_obj->opt_arg_grp, argc, argv, &i)) return 0;
	if(grp->opt_req != NULL){
		printf("Required Option found: %s%s\n", grp->opt_req->item->name_small, grp->opt_req->item->name_big);
		return 0;
	}
	int cmd_parse_res = cli_cmd_parser(grp, tmp_cmd, argc, argv, &i);
	if(cmd_parse_res == 0){
		if(!cli_arg_parser(cli_list_obj->opt_arg_grp, argc, argv, &i)) return 0;
		if(grp->arg_req != NULL){ 
			printf("Required Argument found: %s\n", grp->arg_req->item->name);
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
	cli_cmd_group *tmp_grp = cli_list_obj->opt_arg_grp;
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
			cli_req_opt *tmp_cmd_opt_req = tmp_cmd->item.cli_cmd_list_group->opt_req;
			cli_req_arg *tmp_cmd_arg_req = tmp_cmd->item.cli_cmd_list_group->arg_req;
			cli_cmd_group *tmp_cmd_grp = tmp_cmd->item.cli_cmd_list_group;

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
			free(tmp->item.cli_cmd_list_group);
			tmp_cmd = tmp_cmd->prev;
			free(tmp);
		}
	}
	if(tmp_grp != NULL){
		free(tmp_grp);
	}
}

#endif // CLI_IMPLEMENTATION

#endif // CLI_H
