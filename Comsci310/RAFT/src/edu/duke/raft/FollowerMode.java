package edu.duke.raft;
import java.util.Timer;

//import java.lang.Math;

public class FollowerMode extends RaftMode {

  private Timer followerTimer;     // check for failure
  private int timerid = 0;

  private void initiateTimer() {
      timerid = (mID+29)*mConfig.getCurrentTerm();
      if(mConfig.getTimeoutOverride() == -1) {
        followerTimer = scheduleTimer((long) (Math.random() * (ELECTION_TIMEOUT_MAX - ELECTION_TIMEOUT_MIN) 
          + ELECTION_TIMEOUT_MIN), timerid);
      } else {
        followerTimer = scheduleTimer( (long)mConfig.getTimeoutOverride(), timerid);
      }
  }

  public void go () {
    synchronized (mLock) {
  
      initiateTimer();
      System.out.println ("S" + 
        mID + 
        "." + 
        mConfig.getCurrentTerm () + 
        ": switched to follower mode.");
    }

  }
  
  // @param candidate’s term
  // @param candidate requesting vote
  // @param index of candidate’s last log entry
  // @param term of candidate’s last log entry
  // @return 0, if server votes for candidate; otherwise, server's
  // current term
  public int requestVote (int candidateTerm,
        int candidateID,
        int lastLogIndex,
        int lastLogTerm) {

    synchronized (mLock) {
      int my_current_term = mConfig.getCurrentTerm();
      int my_last_index = mLog.getLastIndex();
      int my_last_term = mLog.getLastTerm();
      int my_votefor = mConfig.getVotedFor();

      if (candidateTerm < my_current_term) {
        return my_current_term;      
      } 
   
      if ( candidateTerm > my_current_term ) {
        //when candidateterm > my_current_term
        if (lastLogTerm > my_last_term ) { //Figure 7, b,e,f
          followerTimer.cancel();
          mConfig.setCurrentTerm(candidateTerm, candidateID);
          initiateTimer();
          return 0;
        }
        if(lastLogTerm == my_last_term){  
          if(lastLogIndex >= my_last_index) { //Figure 7, a
            followerTimer.cancel();
            mConfig.setCurrentTerm(candidateTerm, candidateID);
            initiateTimer();
            return 0;
          } else { //Figure 7, c
              followerTimer.cancel();
              mConfig.setCurrentTerm(candidateTerm, 0);   // none
              initiateTimer();
              return my_current_term;
          }
        } 
        else {//Figure 7, d               
          followerTimer.cancel();
          mConfig.setCurrentTerm(candidateTerm, 0);   // none
          initiateTimer();
          return my_current_term;
        }
      }
      
      if( my_votefor == 0 || my_votefor == candidateID) {
        //when candidateTerm == my_current_term
        if (lastLogTerm > my_last_term ) { //Figure 7, b,e,f
          followerTimer.cancel();
          mConfig.setCurrentTerm(candidateTerm, candidateID);
          initiateTimer();
          return 0;
        }
        if(lastLogTerm == my_last_term){  
          if(lastLogIndex >= my_last_index) { //Figure 7, a
            followerTimer.cancel();
            mConfig.setCurrentTerm(candidateTerm, candidateID);
            initiateTimer();
            return 0;
          } 
        }
        else {
          return my_current_term;
        }
      }
      return my_current_term;
      
    }//end of synchronization
  }
  

  // @param leader’s term
  // @param current leader
  // @param index of log entry before entries to append
  // @param term of log entry before entries to append
  // @param entries to append (in order of 0 to append.length-1)
  // @param index of highest committed entry
  // @return 0, if server appended entries; otherwise, server's
  // current term
  public int appendEntries (int leaderTerm,
          int leaderID,
          int prevLogIndex,
          int prevLogTerm,
          Entry[] entries,
          int leaderCommit) {
    synchronized (mLock) {
      int my_current_term = mConfig.getCurrentTerm();
      int my_last_index = mLog.getLastIndex();
      int my_last_term = mLog.getLastTerm();
      int my_votefor = mConfig.getVotedFor();

      if (leaderTerm < my_current_term) {
        return my_current_term;
      }

      if(leaderTerm > my_current_term) {
        mConfig.setCurrentTerm(leaderTerm, 0);
      }
      // if (mID == 2) {
      //   System.out.println("server 2's entry[prevLogIndex]: " + mLog.getEntry(prevLogIndex));
      // }
      
      if (mLog.getEntry(prevLogIndex) == null || mLog.getEntry(prevLogIndex).term != prevLogTerm) {       
        followerTimer.cancel();
        initiateTimer();
        return -4;                 // mismatch
      } else if (entries != null){
        mLog.insert(entries, prevLogIndex, prevLogTerm);
        //If leaderCommit > commitIndex, set commitIndex =min(leaderCommit, index of last new entry)
        if(leaderCommit>mCommitIndex) {
          mCommitIndex = Math.min(leaderCommit, mLog.getLastIndex());
        }
        followerTimer.cancel();
        initiateTimer();
        return 0;  // leaderCommit
      } else {
        followerTimer.cancel();
        initiateTimer();
        return -2;        // heartbeat
      }

    }//end of synchronization
  }  

  // @param id of the timer that timed out
  public void handleTimeout (int timerID) {
    synchronized (mLock) {
      if (timerID == (mID+29)*mConfig.getCurrentTerm()) {
        followerTimer.cancel();
        RaftServerImpl.setMode(new CandidateMode());
      } 
    }//end of synchronization
  }

}

