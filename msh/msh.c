// The MIT License (MIT)
// 
// Copyright (c) 2024 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include <dirent.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 32     

int check_if_executable(const char *filename)//make a function to check if file is executable
{
  return (access(filename, X_OK) == 0) ? 1 : 0;// check if file is executable
  // 1 if executable, 0 if not
}

void execute(char *command_string);// make a function to execute command

int main(int argc, char *argv[])
{
  FILE *file = NULL; // file pointer for reading commands
  char * command_string = (char *)malloc( MAX_COMMAND_SIZE ); // buffer to store command
  char error_message[30] = "An error has occurred\n"; // used the default error message provided
  
  if (argc == 2) //if file name provided as arguement
  {
    file=fopen(argv[1], "r"); // open given file for reading
    if (file==NULL) // print error if file cant be opened
    {
      write(STDERR_FILENO, error_message, strlen(error_message));
      exit(1);
    }
  }
  else if(argc > 2) // check for more than one arguement give error if so
  {
    write(STDERR_FILENO, error_message, strlen(error_message));
    exit(1);
  }

  while(1) // main loop to keep running to accept commands
  {
    if (file == NULL) // if no file provided, read commands from user
    {
      printf ("msh> "); // print out prompt
      if(fgets(command_string, MAX_COMMAND_SIZE,  stdin) == NULL) // if there is no command
      {
        break; // exit loop if there is error
      }
    }  
    else
    {
      if (fgets(command_string, MAX_COMMAND_SIZE, file) == NULL) // if reading from file, read next line
      {
        if (feof(file)) // exit if end of file reached
        {
          exit(0);
        }
      }
    }
    execute(command_string); // process command read from input
  }
  free(command_string); // free memory
  return 0;
}   

void execute(char *command_string) // function to execute user commands
{
  char error_message[30] = "An error has occurred\n"; // error message
  const char *directories[] = {"/bin", "/usr/bin", "/usr/local/bin", "./"}; // executable paths we were told to use
  int token_count = 0; // counter for # of tokens
  char *token[MAX_NUM_ARGUMENTS]; // array to store individual tokens
  char *working_string = strdup(command_string); // duplicate command string for processing
  char *head_ptr = working_string; // keep track of original string for freeing later
  char *arguement_pointer; // help tokenize command

  while (((arguement_pointer = strsep(&working_string, WHITESPACE)) != NULL) && (token_count < MAX_NUM_ARGUMENTS))
  {
    token[token_count] = strndup(arguement_pointer, MAX_COMMAND_SIZE);
    if (strlen(token[token_count]) == 0)
    {
      token[token_count] = NULL;
    }
    else
    {
      token_count++; // increase token
    }
  }
  token[token_count] = NULL; // token list NULL
  if (token[0] == NULL) // if no tokens
  {
    free(head_ptr);
    return;
  }
  if (strcmp(token[0], "cd") == 0) // cd command
  {
    if (token[1] == NULL || chdir(token[1]) != 0)
    {
      write(STDERR_FILENO, error_message, strlen(error_message)); // if cd fails
    }
    free(head_ptr);
    return;
  }
  if(strcmp(token[0], "exit") == 0 || strcmp(token[0], "quit") == 0) // exit command
  {
    if (token[1] != NULL) // added this to help with test 5
    {
      write(STDERR_FILENO, error_message, strlen(error_message));
    }
    else // just had this before
    {
      exit(0);
    }
    return;
  }
  pid_t pid = fork(); // create child to execute commands
  if (pid<0)
  {
    write(STDERR_FILENO, error_message, strlen(error_message)); // if fork fails
  }
  else if (pid==0) // if child 0
  {
    int command_found=0; // check if executable found
    char path[MAX_COMMAND_SIZE];
    for (int i = 0; i < sizeof(directories) / sizeof(directories[0]); ++i) // loop through directories
    {
      snprintf(path, sizeof(path), "%s/%s", directories[i], token[0]);
      if(check_if_executable(path))
      {
        command_found = 1; // set flag if directory found
        break; 
      }
    }
    if (command_found) // if was found, try to execute
    {
      for (int idx=0; idx<token_count; idx++) // output redirection
      {
        if (token[idx] == NULL) // check for null pointers
        {
          break;
        }
        if(strcmp(token[idx], ">") == 0) // redirection
        {
          int file_descriptor = open(token[idx+1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR); // open file for writing
          if((file_descriptor < 0) || (token[idx+2] != NULL))
          { 
            write(STDERR_FILENO, error_message, strlen(error_message)); // error if redirection fails
            exit(0);
          }
            
          dup2(file_descriptor,1);  // redirect stdout to file
          close(file_descriptor); // close file descriptor
          token[idx] = NULL; // terminate token array

        }
      }
      execvp(path, token); // execute command
      
      write(STDERR_FILENO, error_message, strlen(error_message)); // error is execvp fails
      exit(1); // if fails, exit child
    }
    else
    {
      write(STDERR_FILENO, error_message, strlen(error_message));
      exit(1); // exit child
    }
  }
  else // pid
  {
    int status;
    waitpid(pid, &status, 0); // wait for child to finish
  }
  free(head_ptr); // free memory
  for (int idx=0; idx<token_count; idx++)
  {
   free(token[idx]); // free duplicated tokens
  }
}


