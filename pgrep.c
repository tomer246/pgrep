#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "regex.c"

/*************************************************************
 *  DEFINITIONS
 * ***********************************************************/
#define KB             1024
#define MB             (1024*1024)
#define LINEBUF        (4*KB) 
#define THREADSNUM     8
#define THRESHOLD      2

typedef int bool;

// hold the user settings
typedef struct Settings {
	bool invert;
	bool verbose;
	bool isFile;
	bool isRegex;
	bool ignore;
	bool extra;
	int numExtra;
	char *file;
	char *term;
} Settings;

typedef struct Task {
    char *file_name;
    long  start;
    long  end;
} Task;

/*************************************************************
 *  GLOBALS
 * ***********************************************************/
char cwd[PATH_MAX];
Settings *settings;
pthread_t threads[THREADSNUM];

// help option
void help(char *argv[]) {
	if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help") || !strcmp(argv[1], "-help")) {
		puts("\n A grep multithreaded utility, using custom regex\n");
		puts("    Usage: pgrep [-A <#>|-f <file>|-i|-R|-v|-V] <search term>\n");
		puts("    -A    After Context         pgrep will also print a number of lines after the line containing\n");
		puts("                                <search term>.\n");
		puts("    -f    Single File Search    pgrep will only search the <file> for the <search term>. it will\n");
		puts("                                search the file in the entire directory from where pgrep is called.\n");
		puts("    -i    Ignore case		      ignore upper/lower cases when searching the <search term>. can not\n");
		puts("    						      be used with the -R option (only for raw strings)\n");
		puts("    -R    Regex pattern	      look for <search term> as a regex pattern instead of a raw string\n");
		puts("    -v    Search Inversion      Search for every line that does not include the <search term>.\n");
		puts("    -V    Enable Verbose        The file path to the file will be printed along with the search result.\n");
		puts("    To Be Done: -improve the custom regex patterns.\n");
		puts("    			  -add the -A option\n");
		puts("    			  -add an option for a grep in a directory,\n");
		puts("    			  		with option for per file multithreading (few threads on the same file)\n");
		puts("    			   		as well as per directory (one thread for each file)\n");
		exit(0);
	}
}

// Gets the settings given by the user
void getSettings(int argc, char *argv[], Settings *instance) {
	int arg_index = 1;
	instance->term = NULL;
	
	// Sets settings based off of arguments received.
	while (arg_index < argc) {
		char *arg = argv[arg_index];
		if (!strcmp(arg, "-i")) {
			instance->ignore = 1;
		} if (!strcmp(arg, "-R")) {
			instance->isRegex = 1;
		} else if (!strcmp(arg, "-v")) {
			instance->invert = 1;
		} else if (!strcmp(arg, "-V")) {
			instance->verbose = 1;
		} else if (!strcmp(arg, "-f")) {
			instance->isFile = 1;
			arg_index++;
			if (arg_index >= argc){
				puts("ERROR: The path to the file was not given. \"pgrep -h\" for help.");
				exit(0);
			}
			arg = argv[arg_index];
			if (*arg == '-') {
				puts("ERROR: The path to the file was not given. \"pgrep -h\" for help.");
				exit(0);
			}
			instance->file = arg;
		} else if (!strcmp(arg, "-A")) {
			instance->extra = 1;
			arg_index++;
			if (arg_index >= argc){
				puts("ERROR: The number of after context lines was not given. \"pgrep -h\" for help.");
				exit(0);
			}
			arg = argv[arg_index];
			if (*arg == '-') {
				puts("ERROR: The number of after context lines was not given. \"pgrep -h\" for help.");
				exit(0);
			}
			instance->numExtra = atoi(arg); // 0 if invalid number
		} else {
			if (arg_index < argc-1) {
				puts("ERROR: pgrep was called incorrectly. \"pgrep -h\" for command syntax.");
				exit(0);
			}
			instance->term = arg;
		}
		arg_index++;
	}

	// Check that the search term has been given.
	if (instance->term == NULL) {
		puts("ERROR: Search term not given. \"pgrep -h\" for help.\n");
		exit(0);
	}
}

void grepFile(Task* file_task){
	char line[LINEBUF];
	int total_size = file_task->end - file_task->start;
	int size_left  = total_size;
	FILE *file_ptr;
	if (!(file_ptr = fopen(file_task->file_name, "r"))){
		printf("Error: Could not open the file: %s", file_task->file_name);
		exit(0);
	}
	if (fseek(file_ptr, file_task->start, SEEK_SET)){
		printf("Error: Could not seek for start of file: %s", file_task->file_name);
		fclose(file_ptr);
		exit(0);
	}
	while(size_left>0 && fgets(line, LINEBUF, file_ptr)){
		if (settings->isRegex){
			for(int pos=0; pos<strlen(line); pos++){
				if(run_search(&line[pos])){
					if (settings->verbose)
						printf("%s : %s\n", file_task->file_name, line);
					else
						printf("%s\n", line);
					break;
				}
			}
		}
		else{
			if (strstr(line, settings->term) != NULL) {
				if (settings->verbose)
					printf("%s : %s\n", file_task->file_name, line);
				else
					printf("%s\n", line);
        	} 
		}

		size_left -= strlen(line);
	}
	fclose(file_ptr);
}


void grepFileParallel(char *file_name, long size){
	FILE *file_ptr;
	Task threads_tasks[THREADSNUM];
	long thread_block_size = size / THREADSNUM;
	int pos = 0;

	if (!(file_ptr = fopen(file_name, "r"))){
		printf("Error: Could not open the file: %s", file_name);
		exit(0);
	}

	for (int i=0; i<THREADSNUM - 1; i++){
		threads_tasks[i].file_name = file_name;
		threads_tasks[i].start     = i * thread_block_size + pos;
		threads_tasks[i].end 	   = (i+1) * thread_block_size;

		fseek(file_ptr, threads_tasks[i].end, SEEK_SET);
		for(pos=0; fgetc(file_ptr) != '\n'; pos++);
		threads_tasks[i].end 	   += pos;
		pthread_create(&threads[i], NULL, (void *)grepFile, &threads_tasks[i]);
	}

	threads_tasks[THREADSNUM-1].file_name = file_name;
	threads_tasks[THREADSNUM-1].start     = (THREADSNUM-1) * thread_block_size + pos;
	threads_tasks[THREADSNUM-1].end 	  = size-1;
	pthread_create(&threads[THREADSNUM-1], NULL, (void *)grepFile, &threads_tasks[THREADSNUM-1]);

	fclose(file_ptr);
	for (int i=0; i<THREADSNUM; i++){
		pthread_join(threads[i], NULL);
	}
}

int main(int argc, char *argv[]) {
	settings = (Settings*)malloc(sizeof(Settings));
	help(argv); // check if the user asks for help
	getcwd(cwd, PATH_MAX);
	getSettings(argc, argv, settings);

	if (settings->isFile){
		struct stat file_info;

		if (lstat(settings->file, &file_info) == -1) {
			puts("Error: Could not open the specified file.\n");
			exit(0);
        }
		if (S_ISDIR(file_info.st_mode)){
			puts("Error: Recieved a directory instead of file with.\n");
			exit(0);
		}

		if (settings->isRegex)
			activate_regex(settings->term);

		if (file_info.st_size > THRESHOLD * MB){
			grepFileParallel(settings->file, file_info.st_size);
		}
		else{
			Task *file_task = (Task*)malloc(sizeof(Task));
			file_task->file_name  = settings->file;
			file_task->start 	  = 0;
			file_task->end 		  = file_info.st_size;
			grepFile(file_task);
			free(file_task);
            }
        }

	else{
		puts("Error: Sorry, currently supports only one file search\n");
	}
	
	if (settings->isRegex)
		clean_regex();
	free(settings);
	return 0;
}