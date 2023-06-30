README

TEAM

This project, Tetrashell, was completed by Nathan Flinchum and Aknazar Janibek, 
students of Professor Joshua Bakita’s COMP 211 Spring 23 class. This README 
assumes that the user has already visited the page that details the basic 
instructions of this assignment. Details can be found here: 
	https://www.cs.unc.edu/~jbakita/teach/comp211-s23/a5/

COMMANDS

This assignment has multiple commands that are supported in Tetrashell, 
including commands such as: modify, rank, check, recover, switch, exit, help, 
info, and undo. A brief description of how to use them are listed below.

Modify: First write command ‘modify’ which is then followed by either score or 
	lines (user’s choice), finally followed by a number (user’s choice) that 
	updates the new score or lines.

Rank: First write command ‘rank’, which is then followed by either score or 
	lines (user’s choice), finally followed by an N number (user’s choice) 
	that displays the top N quicksaves sorted either by score or lines. 
	"uplink" is added as the fourth argument internally to connect to a 
	central ranking database.

Check: First and only write the command ‘check’. The purpose of this command is 
	to verify if the current quicksave the user is on is legitimate to or not.

Recover: First write command ‘recover’, which is then followed by the path to a 
	disk image file. The purpose of this command is to recover any potential 
	quicksaves from the disk image file. 

Switch: First write command ‘switch’, which is then followed by the name of 
	the quicksave that the current quicksave will be switched to. 

Exit: First and only write the command ‘exit’. The purpose of this command is 
	to exit out of the Tetrashell program.

Info: First and only write the command ‘info’. The purpose of this command is 
	to show the user the current name of the quicksave, the quicksave’s score 
	and lines.

Undo: First and only write the command ‘undo’. The purpose of this command is 
	to revert the last ‘modify’ command that was executed. If it has not been 
	modified before in this instance of running the program, then user does 
	not have the ability to undo. Un-doing an undo is considered a 'redo'.

Help: First write command ‘help’, which is then followed by the name of any of 
	these listed commands to give the user more information of what the purpose 
	of each command is.

FEATURES

Writing commands do not require the full word to be written out. Abbreviated 
	commands will work. For example instead of writing switch, the user can 
	write s, sw, swi, swit, switc, and switch are all acceptable to the prompt. 
	It will reject commands such as switck or swii.

The user can simply write the command ‘rank’ which is equated to the command 
	‘rank score 10’ by default. If the user writes the command 'rank' along 
	with a second (metric) argument, the third argument is set to 10 by 
	default. This is known as the quick-rank feature.

STYLE IMPLEMENTATION

- The Tetrashell prompt "tetrashell> " is customized to be red.

- The welcome Tetrashell message is is in a custom ASCII font. The message also 
	is colored red mixed with bright red and is flashing/blinking. These ANSI 
	features are dependent on the user's terminal.
