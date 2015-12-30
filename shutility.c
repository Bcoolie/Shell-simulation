#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define max_hname 128
#define max_path 256
#define max_cwd 256

void display_info()
{
	struct passwd *p;
	char hostname[max_hname] = {'\0'};
	char cwd[max_cwd]={'\0'};
	char *token;
	const char delimiter[2] = "/";	

	getcwd(cwd, sizeof(cwd));
	/* get the first token */
	token = strtok(cwd, delimiter);
	/* walk through other tokens */
	while ( token != NULL ) {
		token = strtok(NULL, delimiter);
		if (token != NULL)
			strcpy(cwd,token);
	}
	p = getpwuid(getuid());
	gethostname(hostname, max_hname);
	printf("[%s@%s %s]# ",p->pw_name,hostname,cwd);
}

void redirection_input(char *word1, char *word2)
{
	int input;
	int chk;
	char c='0';

	input = open(word2, O_RDONLY, S_IRUSR | S_IWUSR);
	if (-1 == input) {
		perror("Opening file error!");
		return;
	}		
	if (-1 == dup2(input, fileno(stdin)))
		perror("Cannot redirect stdin!");

	while ((chk=read(STDIN_FILENO, &c, 1)) > 0)
		write(STDOUT_FILENO, &c, 1);

	puts("\n");
	close(input);
}

void redirection_output(char *word1, char *word2)
{
	int out;

	out = open(word2, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if (-1 == out)
		perror("Opening file error!");
	if (-1 == dup2(out, fileno(stdout)))
		perror("Cannot redirect stdout!");

	fflush(stdout);
	close(out);
}