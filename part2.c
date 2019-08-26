//Kuldeep Kushwaha
//160349

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<unistd.h>

int pid;
int pipe1[2];
int pipe2[2];
void wcandsort(char *matchstring, char *directory, char *outfile, char *five, char *six){
  if (pipe(pipe1) == -1) {
      exit(1);
  }

  if ((pid = fork()) == -1) {
    exit(1);
  } else if (pid == 0) {
    exec1(matchstring,directory);
  }
  
  // create pipe2
  if (pipe(pipe2) == -1) {
    exit(1);
  }

  // fork (grep root)
  if ((pid = fork()) == -1) {
    exit(1);
  } else if (pid == 0) {
    // pipe1 --> grep --> pipe2
    exec2(outfile);
  }
  
  // close unused fds
  close(pipe1[0]);
  close(pipe1[1]);

  if ((pid = fork()) == -1) {
    exit(1);
  } else if (pid == 0) {
    exec3(five,six);
  }
}

void exec1(char *matchstring,char *directory) {
  dup2(pipe1[1], 1);
  // close fds
  close(pipe1[0]);
  close(pipe1[1]);
  // exec
  execlp("grep", "grep", "-rF",matchstring ,directory,NULL);
  _exit(1);
}

void exec2(char *outfile) {
  // input from pipe1
  dup2(pipe1[0], 0);
  // output to pipe2
  dup2(pipe2[1], 1);
  // close fds
  close(pipe1[0]);
  close(pipe1[1]);
  close(pipe2[0]);
  close(pipe2[1]);
  // exec
  execlp("tee", "tee", outfile, NULL);
  _exit(1);
}

void exec3(char *five,char *six) {
  // input from pipe2
  dup2(pipe2[0], 0);
  // output to stdout (already done)
  // close fds
  close(pipe2[0]);
  close(pipe2[1]);
  // exec
  execlp(five, five,six, NULL);
  _exit(1);
}

const char *myfile = NULL;
int main(int argc,char *argv[]) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    // printf("%s\n",cwd);

    if(argc==4 && strcmp(argv[1], "@") == 0){
        wcandsort(argv[2],argv[3],NULL,"wc","-l");
    }
    
    else if((argc==6 || argc==7) && strcmp(argv[1], "$")==0){
        if(argc==6){
            wcandsort(argv[2],argv[3],argv[4],argv[5],NULL);
        }
        else if(argc==7){
                wcandsort(argv[2],argv[3],argv[4],argv[5],argv[6]);
        }
        else{
            printf("Enter correct input\n");
        }
    }
    else{
        printf("Enter correct input\n");
    }
    return 0;
}


