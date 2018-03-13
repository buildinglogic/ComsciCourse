#include "dsh.h"


void seize_tty(pid_t callingprocess_pgid); /* Grab control of the terminal for the calling process pgid.  */
void continue_job(job_t *j); /* resume a stopped job */
void spawn_job(job_t *j, bool fg); /* spawn a new job */

int logfd, pid;
int ttyfd; // a seperate fd pointing to terminal
pid_t dsh_pgid; // process group ID associated to terminal
char* prompt;
job_t* first_job = NULL;

/* Write error message to log file */
void unixError(char *error)
{
    printf("%s\n", error); // print to terminal
    perror(error); /* print error */
}

/* Sends SIGCONT signal to wake up the blocked job */
void continue_job(job_t *j) 
{
 if(kill(-j->pgid, SIGCONT) < 0) {
  perror("kill(SIGCONT)");
 }
  unixError("Error: kill(SIGCONT)");
}

/* Sets the process group id for a given job and process */
int set_child_pgid(job_t *j, process_t *p)
{
  if (j->pgid < 0) { /* first child: use its pid for job pgid */
    j->pgid = p->pid;
  }
  return(setpgid(p->pid,j->pgid));
}

/* Creates the context for a new child by setting the pid, pgid and tcsetpgrp */
void new_child(job_t *j, process_t *p, bool fg)
{
  /* establish a new process group, and put the child in
   * foreground if requested
   */

  /* Put the process into the process group and give the process
   * group the terminal, if appropriate.  This has to be done both by
   * the dsh and in the individual child processes because of
   * potential race conditions.  
   * */

   p->pid = getpid();

  /* also establish child process group in child to avoid race (if parent has not done it yet). */
   set_child_pgid(j, p);

  if(fg) // if fg is set
  {
    //printf("j pgid : %d \n", j->pgid);
    //dup2(ttyfd, STDIN_FILENO);
    tcsetpgrp(ttyfd, j->pgid);
    //seize_tty(j->pgid); // assign the terminal
    //printf("here 2 \n");
  }

  /* Set the handling for job control signals back to the default. */
  signal(SIGTTOU, SIG_DFL);
}

/* Spawning a process with job control. fg is true if the 
 * newly-created process is to be placed in the foreground. 
 * (This implicitly puts the calling process in the background, 
 * so watch out for tty I/O after doing this.) pgid is -1 to 
 * create a new job, in which case the returned pid is also the 
 * pgid of the new job.  Else pgid specifies an existing job's 
 * pgid: this feature is used to start the second or 
 * subsequent processes in a pipeline.
 * */

void spawn_job(job_t *j, bool fg) 
 {
  // the first process has input fd 0
  // every process run as a child process
  // if curt process has next process, then create a pipe, update stdout to pipe write end; 
  // mark "in" as pipe read end
  // if cat Makefile | wc | ls, three child process to run each one, the parent will wait for them all
  //int pipefd[2];
  dsh_pgid = tcgetpgrp(ttyfd); // record tty pgid in case yield tty to child
   pid_t pid;
   process_t *p;
   bool firstP = true; // help print cmdline only once
   int pipefd[2];

  int in = j->mystdin; 
  int oldin, oldout; // record the input/output before update
  int curtin, curtout;

  for(p = j->first_process; p; p = p->next) { /* Builtin commands are already taken care earlier */

    curtin = in; // input fd, the read end of the last pipe or default mystdin
    curtout = j->mystdout; // output fd
    printf("\n \n crutint : %d, curtout : %d \n", curtin, curtout);
    printf(" curt exe %s \n", p->argv[0]);
    // create an pipe
    if(p->next != NULL) { // last process in list, don't need create pipe
      if (pipe(pipefd) == -1) {
        perror("parent: Failed to create pipe\n");
        exit(EXIT_FAILURE);
      }
      in = pipefd[0]; // set up input fd for next process
      curtout = pipefd[1]; // set up output of curt proces to the write end of pipe
    }

    switch (pid = fork()) {

      case -1: /* fork failure */
      perror("fork");
      exit(EXIT_FAILURE);

      case 0: /* child process  */
      p->pid = getpid();      
      new_child(j, p, fg);

      // handle process input file 
      // if has input file, always use input file, 
      // otherwise, use default in or previous pipe read end
      oldin = dup(STDIN_FILENO);
      printf("oldin %d \n", oldin);
      if(p->ifile != NULL) {
        close(curtin);
        curtin = open(p->ifile, O_RDONLY);
        dup2(curtin, STDIN_FILENO);
        close(curtin);
      }
      else if (curtin != STDIN_FILENO) { // (p->ifile == NULL) 
        dup2(curtin,STDIN_FILENO);   
        close(curtin); 
      }   

      // handle next pipe connection 
      // if has next process, ignore output file
      printf("STDOUT_FILENO %d \n", STDOUT_FILENO);
      oldout = dup(STDOUT_FILENO);
      printf("oldout %d, curtout %d\n", oldout, curtout);
      if (p->next != NULL){
        close(pipefd[0]); // close unused read end
        if(curtout != STDOUT_FILENO) {
          dup2(curtout,STDOUT_FILENO); 
          close(curtout); 
        }
      }
      else if (p->ofile != NULL) { 
        close(curtout);
        curtout = open(p->ofile, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR | S_IROTH);
        dup2(curtout, STDOUT_FILENO);
        close(curtout);
      }

        // exec child process
      sprintf(ttyfd, " curt execting %s \n", p->argv[0]);
      if (p->argv[0] != NULL) {
        p->stopped=false;
        if (execvp(p->argv[0], p->argv)<0) { // only return when error
          p->stopped=true;
          p->completed=true;
          perror("execvp ");
        }
      }
      exit(EXIT_FAILURE);  /* NOT REACHED */
      break;    /* NOT REACHED */

      default: /* parent */
      /* establish child process group */
      p->pid = pid;
      set_child_pgid(j, p);

      if(firstP) {
        printf("%d(Launched): %s  \n", pid, j->commandinfo); // print to terminal
        fprintf(stderr, "%d(Launched): %s  \n", pid, j->commandinfo); // print to dsh.log
        firstP = false;
      }

    } // end switch

    // parent wait for children finish process
    int stat;
    if(fg) {
      if (pid != 0) {
        waitpid(pid, &stat, WUNTRACED);
      }
      // if (WIFEXITED(stat)) {
      if(stat == 0) {
        p->completed = true;
        p->stopped = true;
      }
      else /*if (WIFSTOPPED(stat)) */{
        p->completed = false;
        p->stopped = true;
        fprintf(stderr, "%d$: Stopped\n", j->pgid);
      }
      tcsetpgrp(ttyfd, dsh_pgid); // reset tty to the previous process group
    }

    // reset stdin, and stdout if we changed them for child process
    if(p->ifile != NULL) {
      dup2(oldin, STDIN_FILENO);
    }
    if(p->ofile != NULL) {
      dup2(oldout, STDOUT_FILENO);
    }
    //printf("dsh_pgid : %d, curtpid : %d\n", dsh_pgid, getpid());
    tcsetpgrp(ttyfd, dsh_pgid);
    //seize_tty(getpid()); // assign the terminal back to dsh
  }//end for

} // end spawn_child


// find the job with job_number: for bg, and fg
job_t * findJob(pid_t job_pgid) {
  job_t * curt = first_job;
  while(curt != NULL && curt->pgid != job_pgid) curt = curt->next;
  if(curt == NULL) {
    fprintf(stderr, "background nonexisting job: %d\n", job_pgid); 
  }
  return curt;
}

// deactivate job: stopped, or completed
void setCompleted(job_t * job, char* s) {
  if(!strcmp("Completed", s)) { // deactivate due to completed
    for(process_t * p = job->first_process; p; p = p->next) {
      p->stopped = true;
      p->completed = true;
    }
    job->notified  = true;
  }
}

// activate job: set all process stopped, completed 0, and job->notified false;
void activateJob(job_t * job) {
  for(process_t * p = job->first_process; p; p = p->next) {
    p->completed = 0;
    p->stopped = 0;
  }
  job->notified = false; // reset notified for stopped 
}

// return the status of job: Completed, Stopped, Running
char * jobCSR(job_t *j) { 
  if(job_is_completed(j)) {
    return "Completed"; // only completed return
  }
  else if(job_is_stopped(j)) {
    return "Stopped"; // since completed already returned, only stopped return
  }
  return "Running";
}

bool builtin_cmd(job_t *curt_job, int argc, char **argv) 
{
  /* check whether the cmd is a built in command */

  if (!strcmp("quit", argv[0])) { // clear all jobs list
    while(first_job != NULL && first_job->next != NULL) {
      delete_job(first_job->next, first_job);
    }
    delete_job(first_job, first_job);
    exit(EXIT_SUCCESS);
  }

  else if (!strcmp("jobs", argv[0])) { // report completed and stopped and remove completed from joblist
    //print_job(first_job);
    job_t * j = first_job;
    while(j) {
      if( strcmp(j->commandinfo, "jobs") ) { // skip job with "jobs" as cmd
        for(process_t * p = j->first_process; p; p = p->next) {
          int childCSR;
          while(waitpid(-1, &childCSR, WNOHANG) > 0) { // don't stop calling process while wait
            if(WIFEXITED(childCSR)) {
              p->completed = true;
              p->stopped = true;
            }
            if(WIFSTOPPED(childCSR)) {
              p->completed = false;
              p->stopped = true;
            }
          }
        } // end for

        // only thos non-jobs job, with stopped, completed status can reach here
        
        char* jobstatus = jobCSR(j);
        printf("%d(%s): %s\n", j->first_process->pid, jobstatus, j->commandinfo); // to terminal
        fprintf(stderr, "%d(%s): %s\n", j->first_process->pid, jobstatus, j->commandinfo); // to dsh.log
      } // end if

      j = j->next;
    }
    
    curt_job->first_process->completed = true;

    // remove completed jobs
    j = first_job;
    while(j) {
      if( !strcmp(jobCSR(j), "Completed") ) {
        job_t * nextJob = j->next;
        if(j->pgid == first_job->pgid) { // what if j is the the first job?? 
          delete_job(j, first_job); 
          first_job = j = nextJob;
        }
        else {
          delete_job(j, first_job); 
          j = nextJob;
        }
      }
      else j = j->next;
    }

    curt_job->first_process->completed = true;
    delete_job(curt_job, first_job);
    return true;
  }

  else if (!strcmp("cd", argv[0])) {
            /* Your code here */
    chdir(argv[1]);
    curt_job->first_process->completed = true;
    delete_job(curt_job, first_job); // remove curt job
    return true;
  }

  else if (!strcmp("bg", argv[0])) {
            /* Your code here */
    //print_job(first_job);
    if(curt_job == first_job) {
      delete_job(curt_job, first_job); // finished, then deleted
      first_job = NULL;
    }
    else delete_job(curt_job, first_job); // finished, then deleted

    job_t * target_job;
    if(argc == 1) {
      target_job = find_last_job(first_job);
    }
    else {
      int target_job_pgid = atoi(argv[1]);
      target_job = findJob((pid_t) target_job_pgid);
    }
    
    if (target_job != NULL) {
      activateJob(target_job); // set status
      continue_job(target_job); // send signal to wake up stopped job; 
      // "kill(SIGCONT): Operation not permitted" output if no argument
      target_job->bg = true;
    }
    else {
      unixError("Error: No job to background!");
    }
    
    curt_job->first_process->completed = true;
    return true;
  }

  else if (!strcmp("fg", argv[0])) {
    // /* Your code here */
    if(curt_job == first_job) {
      delete_job(curt_job, first_job); // finished, then deleted
      first_job = NULL;
    }
    else delete_job(curt_job, first_job); // finished, then deleted

    job_t * target_job;
    dsh_pgid = tcgetpgrp(STDIN_FILENO); // record curt process/job on terminal
    if(argc == 1) {
      target_job = find_last_job(first_job);
    }
    else {
      int target_job_pgid = atoi(argv[1]);
      target_job = findJob((pid_t) target_job_pgid);
    }

    if (target_job != NULL) {
      activateJob(target_job);
      continue_job(target_job);
      target_job->bg = false;
    }
    else {
      unixError("Error: No job to foreground!");
      curt_job->first_process->completed = true;
      return true;
    }

    seize_tty(target_job->pgid);
    int status;
    waitpid(target_job->pgid, &status, WUNTRACED); // any child process not reported, will report
    setCompleted(target_job, "Completed");

    curt_job->first_process->completed = true;

    seize_tty(dsh_pgid); // return the terminal back to the previous job
    return true;
  }

  return false;       /* not a builtin command */
}


/* Build prompt messaage */
char* promptmsg() 
{
  /* Modify this to include pid */
  pid = getpid();
  prompt = (char*)malloc(100);
  sprintf(prompt,"dsh-%d$ ",pid);
  return prompt;
}

int main() 
{
  init_dsh();
  DEBUG("Successfully initialized\n");

  ttyfd = open("/dev/tty", O_RDWR); // a new fd to terminal
  dsh_pgid = getpid();
  logfd = open("dsh.log",O_CREAT | O_WRONLY | O_APPEND); /* write log file */
  dup2(logfd, STDERR_FILENO);

  while(1) {
    job_t *j = NULL;
    if(!(j = readcmdline(promptmsg()))) {
      if (feof(stdin)) { /* End of file (ctrl-d) */
        fflush(stdout);
        printf("\n");
        exit(EXIT_SUCCESS);
      }
      continue; /* NOOP; user entered return or spaces with return */
    }

    if(first_job == NULL) {
        first_job = j; /* a pointer to always record the first job */
    }
    else {
      job_t * lastJob = find_last_job(first_job);
      lastJob->next = j;
    }

    job_t * cur_job = j; /* assign current job */

    while(cur_job != NULL) {
      bool isBuiltin = builtin_cmd(cur_job, cur_job->first_process->argc, cur_job->first_process->argv);
      if(!isBuiltin) {
          /* if current job is not background */
        if(!cur_job->bg) {
          spawn_job(cur_job, true);
        }
        else {
          spawn_job(cur_job, false);
        }
      }

      cur_job = cur_job->next;
    }  
  
  } // end readcmd

  close(logfd);
}
