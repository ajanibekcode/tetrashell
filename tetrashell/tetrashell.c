// I, Nathan Flinchum (730483098), pledge that I have neither given nor received unauthorized aid on this assignment.
// I, Aknazar Janibek (730484600), pledge that I have neither given nor received unauthorized aid on this assignment.
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "tetris.h"
#include <unistd.h>

#define MAX_STR 256
#define MAX_ARGS 10
#define PROG_MODIFY "./modify"
#define PROG_RANK "./rank"
#define PROG_CHECK "./check"
#define PROG_RECOVER "./recover"

typedef struct lastSaveValues {
	unsigned score;
	unsigned lines;
} last;

int main(int argc, char* argv[]) {
	// Print welcome message
	printf("Welcome to..."
		"\x1b[5;91m\n▄▄▄▄▄▄▄▄ .▄▄▄▄▄▄▄▄   ▄▄▄· .▄▄ ·  ▄ .▄▄▄▄ .▄▄▌  ▄▄▌  "
		"\x1b[31m\n•██  ▀▄.▀·•██  ▀▄ █·▐█ ▀█ ▐█ ▀. ██▪▐█▀▄.▀·██•  ██•  "
		"\x1b[5;91m\n ▐█.▪▐▀▀▪▄ ▐█.▪▐▀▀▄ ▄█▀▀█ ▄▀▀▀█▄██▀▐█▐▀▀▪▄██▪  ██▪  "
		"\x1b[31m\n ▐█▌·▐█▄▄▌ ▐█▌·▐█•█▌▐█ ▪▐▌▐█▄▪▐███▌▐▀▐█▄▄▌▐█▌▐▌▐█▌▐▌"
		"\x1b[5;91m\n ▀▀▀  ▀▀▀  ▀▀▀ .▀  ▀ ▀  ▀  ▀▀▀▀ ▀▀▀ · ▀▀▀ .▀▀▀ .▀▀▀ \x1b[0m"
		"\nthe ultimate Tetris quicksave hacking tool!\n");
	
	// Obtain path to the target quicksave
	char path[MAX_STR];
	printf("Enter the path to the quicksave you'd like to begin hacking: ");
	if (fgets(path, MAX_STR, stdin) == NULL) {
		perror("Error opening quicksave. Check the path name");
		return 1;
	}
	path[strcspn(path, "\n")] = '\0';
	
	// Check whether the user-specified quicksave exists
	if (access(path, F_OK) == -1) {
		perror("Error verifying quicksave file's existence in provided path");
		return 1;
	}
	
	printf("'%s' set as the current quicksave.\n", path);
	printf("Enter your command below to get started: \n");

	// Open path & read data in
	TetrisGameState currentSave;
	last lastSave;
	char lastMetric[6]; 	// Size of "score" or "lines"
	FILE* fp;
	int8_t modified = -1;	// Flag for undo

	openSave:
	
	// Update retrieve values & store last save
	if (modified > 0 && lastMetric[0] == 's') {
		lastSave.score = currentSave.score;
	} else if (modified > 0 && lastMetric[0] == 'l') {
		lastSave.lines = currentSave.lines;
	}

	fp = fopen(path, "rb");
	if (fp == NULL) {
		perror("Error opening quicksave");
		return 1;
	}
	if (fread(&currentSave, sizeof(TetrisGameState), 1, fp) != 1) {
		perror("Error reading quicksave");
		return 1;
	}
	fclose(fp);

	char command[MAX_STR];
	while (1) {
		start:	// Goto for invalid commands

		// Get user input
		printf("\x1b[91mtetrashell> \x1b[0m");
		if (fgets(command, MAX_STR, stdin) == NULL) {
			perror("Error getting command");
			return 1;
		}

		// Retrieve arguments from user command string
		command[strcspn(command, "\n")] = '\0';
		char cmd_args[MAX_ARGS][MAX_STR];
		int n_args = 0;
		char* ptr = command;
		while (sscanf(ptr, "%s", cmd_args[n_args]) == 1 && n_args < MAX_ARGS) {
			n_args++;
			ptr += strlen(cmd_args[n_args - 1]) + 1; // Move ptr to next arg
		}

		pid_t id;

		// Exit - shortest command is 'e'
		if (cmd_args[0][0] == 'e') { 	// Test 1st char to avoid str comps
			if (strlen(cmd_args[0]) != 1
					&& strcmp(cmd_args[0], "exit") != 0
					&& strcmp(cmd_args[0], "ex") != 0
					&& strcmp(cmd_args[0], "exi") != 0) {
				perror("Invalid command");
				goto start;
			}
			break;
		}

		// Undo - shortest command is 'u'
		if (cmd_args[0][0] == 'u') {
			if (strlen(cmd_args[0]) != 1
					&& strcmp(cmd_args[0], "undo") != 0
					&& strcmp(cmd_args[0], "un") != 0
					&& strcmp(cmd_args[0], "und") != 0) {
				perror("Invalid command");
				goto start;
			}
			if (modified < 0) {
				perror("Nothing to undo - modify quicksave first");
			} else {
				// Update command arguments to re-run modify with old values
				strcpy(cmd_args[0], "modify");
				strcpy(cmd_args[1], lastMetric);
				if (cmd_args[1][0] == 's') {
					snprintf(cmd_args[2],
						sizeof(unsigned),
						"%u",
						lastSave.score);
				} else if (cmd_args[1][0] == 'l') {
					snprintf(cmd_args[2],
						sizeof(unsigned),
						"%u",
						lastSave.lines);
				}
				n_args = 3;
			}
		}

		// Modify - shortest command is 'm'
		if (cmd_args[0][0] == 'm') {
			if (strlen(cmd_args[0]) != 1
					&& strcmp(cmd_args[0], "modify") != 0
					&& strcmp(cmd_args[0], "mod") != 0
					&& strcmp(cmd_args[0], "mo") != 0
					&& strcmp(cmd_args[0], "modi") != 0
					&& strcmp(cmd_args[0], "modif") != 0) {
				perror("Inavlid command");
				goto start;
			}
			if (n_args != 3) {	// Args: field to modify, value
				perror("Invalid number of arguments for modify");
				goto start;
			}
			
			// Run Subprocess
			strcpy(cmd_args[n_args], path); 	// Add path as arg
			n_args++;
			id = fork();
			if (id == -1) {
				perror("Error creating subprocess with fork");
				return 1;
			} else if (id == 0) {
				// In child process
				char* mod_args[] = {
					cmd_args[0],
					cmd_args[1],
					cmd_args[2],
					path,
					NULL
				};
				execvp(PROG_MODIFY, mod_args);
				error(1, errno, "%s failed to run", PROG_MODIFY);
			} else {
				// In main/parent process
				int res;
				wait(&res);
				strcpy(lastMetric, cmd_args[1]);	// Update last metric
				modified = 1;	// Update flag to give ability to undo
				goto openSave;
			}
		}

		// Check - shortest command is 'c'
		if (cmd_args[0][0] == 'c') {
			if (strlen(cmd_args[0]) != 1
					&& strcmp(cmd_args[0], "check") != 0
					&& strcmp(cmd_args[0], "ch") != 0
					&& strcmp(cmd_args[0], "che") != 0
					&& strcmp(cmd_args[0], "chec") != 0) {
				perror("Invalid command");
				goto start;
			}
			if (n_args != 1) {	// Args: none (just command name)
				perror("Invalid number of arguments for check");
				goto start;
			}
			
			// Run subprocess
			strcpy(cmd_args[n_args], path); 	// Add path as arg
			n_args++;
			id = fork();
			if (id == -1) {
				perror("Error creating subprocess with fork");
				return 1;
			} else if (id == 0) {
				// In child process
				char* check_args[] = {
					cmd_args[0],
					cmd_args[1],
					NULL
				};
				execvp(PROG_CHECK, check_args);
				error(1, errno, "%s failed to run", PROG_CHECK);
			} else {
				// In main/parent process
				int res;
				wait(&res);
			}
		}

		// Recover - shortest command is "re"
		if (cmd_args[0][0] == 'r') {
			if (strlen(cmd_args[0]) == 1) {
				perror("Invalid command. More than one command with 'r'");
				goto start;
			}
			if (cmd_args[0][1] != 'a'
					&& strcmp(cmd_args[0], "re") != 0
					&& strcmp(cmd_args[0], "recover") != 0
					&& strcmp(cmd_args[0], "rec") != 0
					&& strcmp(cmd_args[0], "reco") != 0
					&& strcmp(cmd_args[0], "recov") != 0
					&& strcmp(cmd_args[0], "recove") != 0) {
				perror("Invalid command");
				goto start;
			}
			if (cmd_args[0][1] != 'a') {
				if (n_args != 2) {	// Args: disk image path
					perror("Invalid number of arguments for recover");
					goto start;
				}
				
				// Run subprocess
				id = fork();
				if (id == -1) {
					perror("Error creating subprocess with fork");
					return 1;
				} else if (id == 0) {
					// In child process
					char* recover_args[] = {
						cmd_args[0],
						cmd_args[1],
						NULL
					};
					execvp(PROG_RECOVER, recover_args);
					error(1, errno, "%s failed to run", PROG_RECOVER);
				} else {
					// In main/parent process
					int res;
					wait(&res);
				}
			}
		}

		// Rank - shortest command is "ra"
		if (cmd_args[0][0] == 'r') {
			if (cmd_args[0][1] == 'e')	// For when recover was called
				goto start;
			if (strcmp(cmd_args[0], "ra") != 0
						&& strcmp(cmd_args[0], "rank") != 0
						&& strcmp(cmd_args[0], "ran") != 0) {
				perror("Invalid command");
				goto start;
			}
			if (n_args > 3) {
				perror("Invalid number of arguments for rank");
				goto start;
			}

			// Run subprocess
			int fd[2];
			if (pipe(fd) == -1) {
				perror("Error creating pipe");
				return 1;
			}
			id = fork();
			if (id == -1) {
				perror("Error creating subprocess with fork");
				return 1;
			} else if (id == 0) {
				// In child process
				char* rank_args[] = {
					cmd_args[0],
					"score",
					"10",
					"uplink",
					NULL
				};
				if (n_args == 1) {
					n_args += 3;	// Metric, value, & uplink auto-added
				} else if (n_args == 2) {
					rank_args[1] = cmd_args[1];
					n_args += 2;	// Value & uplink auto-added
				} else {
					rank_args[1] = cmd_args[1];
					rank_args[2] = cmd_args[2];
					n_args += 1;	// Uplink auto-added
				}
				close(fd[1]);	// Close write end of pipe
				if (dup2(fd[0], STDIN_FILENO) == -1) {
					perror("Error copying file descriptor");
					return 1;
				}
				close(fd[0]);	// Close read end of pipe
				execvp(PROG_RANK, rank_args);
				error(1, errno, "%s failed to run", PROG_RANK);
			} else {
				// In main/parent process
				int res;
				close(fd[0]);	// Close read end of pipe
				if (write(fd[1], path, strlen(path)) == -1) {
					perror("Error writing to pipe");
					return 1;
				}
				close(fd[1]);	// Close write end of pipe
				wait(&res);
			}
		}

		// Switch - shortest command is 's'
		if (cmd_args[0][0] == 's'){
			if (strlen(cmd_args[0]) != 1 
					&& strcmp(cmd_args[0], "sw") != 0 
					&& strcmp(cmd_args[0], "switch") != 0 
					&& strcmp(cmd_args[0], "swi") != 0 
					&& strcmp(cmd_args[0], "swit") != 0 
					&& strcmp(cmd_args[0], "switc") != 0){
				perror("Invalid command");
				goto start;
			}
			if (n_args != 2){
				perror("Invalid number of arguments for help");
				goto start;
			}
			if (access(cmd_args[1], F_OK) == -1){
				perror("Error verifying quicksave");
			}
			printf("Switched current quicksave from '%s' to '%s'.\n"
					,path
					,cmd_args[1]);
			strcpy(path, cmd_args[1]);
			goto openSave;
		}

		// Help - shortest command is 'h'
		if (cmd_args[0][0] == 'h') {
			if (strlen(cmd_args[0]) != 1 
				&& strcmp(cmd_args[0], "help") != 0
				&& strcmp(cmd_args[0], "he") != 0 
				&& strcmp(cmd_args[0], "hel") != 0) {
				perror("Invalid command");
				goto start;
			}
			if (n_args != 2){
				perror("Invalid number of arguments for help");
				goto start;
			}
			if (strcmp(cmd_args[1], "modify") == 0) {
				printf("This command calls the 'modify' program with the "
						"current quicksave and gives the user the option to "
						"either modify their score or lines.\n");
			} else if (strcmp(cmd_args[1], "check") == 0) {
				printf("This command calls the 'check' program with the "
						"current quicksave to verify if it will pass legitimacy"
						" checks.\n");
			} else if (strcmp(cmd_args[1], "recover") == 0) {
				printf("This command calls the 'recover' program and recovers "
						"quicksaves from a disk image file and prints a list "
						"of all these recovered quicksaves.\n");
			} else if (strcmp(cmd_args[1], "rank") == 0) {
				printf("This command calls the 'rank' program and displays "
						"an N number of top games based on scores or lines, "
						"where N is the number of games displayed chosen "
						"by the user.\n");
			} else if (strcmp(cmd_args[1], "exit") == 0) {
				printf("This command calls exit, meaning the user exits "
						"out of the main TETRASHELL program.\n");
			} else if (strcmp(cmd_args[1], "switch") == 0) {
				printf("This command calls switch, allowing the user to change "
						"the current quicksave being hacked.\n");
			} else if (strcmp(cmd_args[1], "info") == 0) {
				printf("This command calls info, which gives the user basic "
						"information about the quicksave, such as its path, "
						"score, and lines.\n");
			} else if (strcmp(cmd_args[1], "undo") == 0) {
				printf("This command call undo, which reverts the last "
						"'modify' command executed.\n");
			} else {
				printf("No command found.\n");
			}
		}

		// Info - shortest command is 'i'
		if (cmd_args[0][0] == 'i'){
			if (strlen(cmd_args[0]) != 1 
					&& strcmp(cmd_args[0], "info") != 0
					&& strcmp(cmd_args[0], "in") != 0 
					&& strcmp(cmd_args[0], "inf") != 0) {
				perror("Invalid command");
				goto start;
			}
			if (n_args != 1) {
				perror("Invalid number of arguments for info");
				goto start;
			}
			printf("Current savefile: %s\n", path);
			printf("Score: %d\n", currentSave.score);
			printf("Lines: %d\n", currentSave.lines);
		}

		// Any other command attempts
		if (cmd_args[0][0] != 'e'
				&& cmd_args[0][0] != 'u'
				&& cmd_args[0][0] != 'm'
				&& cmd_args[0][0] != 'c'
				&& cmd_args[0][0] != 's'
				&& cmd_args[0][0] != 'i'
				&& cmd_args[0][0] != 'h'
				&& cmd_args[0][1] != 'a'	// rank
				&& cmd_args[0][1] != 'e')	// recover
			perror("Invalid command");
	}
	return 0;
}
