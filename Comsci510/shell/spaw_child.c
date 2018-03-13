void spawn_job(job_t *j, bool fg) 
 {

   pid_t pid;
   process_t *p;

   int pipefd[2];
   int status;

  int in = 0; // the first process has input fd 0
  // every process run as a child process
  // if curt process has next process, then create a pipe, update stdout to pipe write end; 
  // mark "in" as pipe read end
  // if cat Makefile | wc | ls, three child process to run each one, the parent will wait for them all

  for(p = j->first_process; p; p = p->next) {
	  /* Builtin commands are already taken care earlier */

    // handle process input or output file other thand stdin, stdout
    int oldin, oldout; // remember the old in and out fd
    if(p->ifile != NULL) {
      oldin = dup(STDIN_FILENO);
      int fd = open(p->ifile, O_RDONLY);
      dup2(fd, STDIN_FILENO);
      close(fd);
    }
    if(p->ofile != NULL) {
      oldout = dup(STDOUT_FILENO);
      int fd = open(p->file, O_WRONLY);
      dup2(fd, STDOUT_FILENO);
      close(fd);
    }

    int curtin = in; // input fd
    int curtout = 1; // input fd
    // create an pipe
    if(p->next != NULL) { // last process in list, don't need create pipe
      if (pipe(pipefd) == -1) {
        perror("parent: Failed to create pipe\n");
        exit(EXIT_FAILURE);
      }
      in = pipefd[0]; // set up input fd for next process
      curtout = pipefd[1]; // set up output of curt proces to the write end of pipe
    }
    else curtout = mystdout;

    switch (pid = fork()) {

      case -1: /* fork failure */
      perror("fork");
      exit(EXIT_FAILURE);

      case 0: /* child process  */
      p->pid = getpid();	    
      new_child(j, p, fg);

      if(curtin != 0) {
        dup2(curtin, STDIN_FILENO);
        close(curtin);
      } 
      close(pipefd[0]); // close unused read end
      if(curtout != 1) {
        dup2(curtout, STDOUT_FILENO); 
        close(curtout);
      }

      if (p->argv[0] != NULL) {
        p->stopped=false;
        if (execvp(p->argv[0], p->argv)<0) { // only return when error
          p->stopped=true;
          p->completed=true;
          perror("New child should have done an exec");
        }
      }

      exit(EXIT_FAILURE);  /* NOT REACHED */
      break;    /* NOT REACHED */

      default: /* parent */
        /* establish child process group */
        p->pid = pid;
        set_child_pgid(j, p);
        close(pipefd[0]); 
        close(pipefd[1]);
        fprintf("%d (Launched): %s \n", j->pgid, j->commandinfo);
      }

      if (curtin != j->mystdin)
        close (curtin);
      if (curtout != j->mystdout)
        close (curtout);
      infile = filedes[0];
      int stat;
      if(fg) {
        if (pid != 0) {
          waitpid(pid, &stat, WUNTRACED);
        }
        if (stat == 0) {
          p->completed = true;
          p->stopped = true;
        }
        else {
          p->completed = false;
          p->stopped = true;
          fprintf(stderr, "%d: Stopped\n", j->pgid);
        }
        /* Transfer control back to the shell */
        tcsetpgrp(shell_terminal, dsh_pgid);
      }

    /* YOUR CODE HERE?  Parent-side code for new job.*/
	   seize_tty(getpid()); // assign the terminal back to dsh

  }
}