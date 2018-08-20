package edu.duke.raft;
import java.util.Timer;

public class CandidateMode extends RaftMode {
  private Timer checkVoteTimer;
  private Timer newElectionTimer;
  private int timerid = 0;
  private int checkVote_timerid = 0;

  private void initiate_checkvoteTimer() {
      timerid = (mID+29)*mConfig.getCurrentTerm();
      if(mConfig.getTimeoutOverride() == -1) {
        newElectionTimer = scheduleTimer((long) (Math.random() * (ELECTION_TIMEOUT_MAX - ELECTION_TIMEOUT_MIN) 
          + ELECTION_TIMEOUT_MIN), timerid);
      } else {
        newElectionTimer = scheduleTimer( (long)mConfig.getTimeoutOverride(), timerid);
      }
  }

  private void initiate_checknewElectionTimer() {
     checkVote_timerid = (mID+29)*mConfig.getCurrentTerm() + 5;
     checkVoteTimer = scheduleTimer(5,checkVote_timerid);
  }

  public void go () {
    synchronized (mLock) {
      mConfig.setCurrentTerm(mConfig.getCurrentTerm() + 1, mID);//vote for self
      initiate_checknewElectionTimer();
      initiate_checkvoteTimer();  

      RaftResponses.setTerm(mConfig.getCurrentTerm());         // *
      RaftResponses.clearVotes(mConfig.getCurrentTerm());

      for (int i = 1; i <= mConfig.getNumServers(); i++) {
          remoteRequestVote(i, mConfig.getCurrentTerm(), mID, mLog.getLastIndex(), mLog.getLastTerm());
      }   

      System.out.println ("S" + 
        mID + 
        "." + 
        mConfig.getCurrentTerm() + 
        ": switched to candidate mode.");
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

      if (candidateID == mID) {            // itself, grant vote
        return 0;
      } 

      if (candidateTerm < mConfig.getCurrentTerm ()) {
        return mConfig.getCurrentTerm ();      // also update votedFor?
      }

      if ( candidateTerm > my_current_term ) {
        //when candidateterm > my_current_term
        if (lastLogTerm > my_last_term ) { //Figure 7, b,e,f
          checkVoteTimer.cancel();              // also compare last index when term equal?
          newElectionTimer.cancel();
          mConfig.setCurrentTerm(candidateTerm, candidateID);
          RaftServerImpl.setMode(new FollowerMode());
          return 0;
        }
        if(lastLogTerm == my_last_term){  
          if(lastLogIndex >= my_last_index) { //Figure 7, a
            checkVoteTimer.cancel();              // also compare last index when term equal?
            newElectionTimer.cancel();
            mConfig.setCurrentTerm(candidateTerm, candidateID);
            RaftServerImpl.setMode(new FollowerMode());
            return 0;
          } 
        }
        else { //Figure 7, c
            checkVoteTimer.cancel();             
            newElectionTimer.cancel();
            mConfig.setCurrentTerm(candidateTerm, 0);   // none
            RaftServerImpl.setMode(new FollowerMode());
            return my_current_term;
        }
      }
        
      else {
        //when candidateTerm == my_current_term
        //my_votefor == candidateID happen only when candidate receive their own requestvote
        if (lastLogTerm > my_last_term ) { //Figure 7, b,e,f
          checkVoteTimer.cancel();
          newElectionTimer.cancel();
          mConfig.setCurrentTerm(candidateTerm, candidateID);
          RaftServerImpl.setMode(new FollowerMode());
          return 0;
        }
        if(lastLogTerm == my_last_term){  
          if(lastLogIndex > my_last_index) { //Figure 7, a
            checkVoteTimer.cancel();
            newElectionTimer.cancel();
            mConfig.setCurrentTerm(candidateTerm, candidateID);
            RaftServerImpl.setMode(new FollowerMode());
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
      if (leaderTerm < mConfig.getCurrentTerm()) {
        return mConfig.getCurrentTerm();
      } else {
        mConfig.setCurrentTerm(leaderTerm, leaderID);
        checkVoteTimer.cancel();
        newElectionTimer.cancel();
        RaftServerImpl.setMode(new FollowerMode());
        return -3;      // state change
      }
      // }
      // if(leaderTerm >= mConfig.getCurrentTerm()) {
        
      // }
      // else {
      //   //when leaderTerm == mConfig.getCurrentTerm()
      
      // }
      // return mConfig.getCurrentTerm();     // never reach
    }//end of synchronization
  }

  // @param id of the timer that timed out
  public void handleTimeout (int timerID) {
    synchronized (mLock) {
      if (timerID == checkVote_timerid) {     // it's checkVote timer
        int[] votes = RaftResponses.getVotes(mConfig.getCurrentTerm());
        int count = 0;
        for(int i = 1; i < votes.length; i++){
          if (votes[i] > mConfig.getCurrentTerm()) {
            checkVoteTimer.cancel();
            newElectionTimer.cancel();
            mConfig.setCurrentTerm(votes[i], i);
            RaftServerImpl.setMode(new FollowerMode());
            return;
          }
          if (votes[i] == 0) {
            count++;
          }
        }
        if (count*2 > mConfig.getNumServers()) {
          checkVoteTimer.cancel();
          newElectionTimer.cancel();
          RaftServerImpl.setMode(new LeaderMode());
          return;
        } else {
          checkVoteTimer.cancel();
          initiate_checknewElectionTimer();
        }
      }

      if (timerID == timerid) {    // it's new election timer
        checkVoteTimer.cancel();
        newElectionTimer.cancel();
        go();
      }
    }//end of syn
  }

}
