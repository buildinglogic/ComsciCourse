package edu.duke.raft;
import java.util.Timer;
import java.util.Arrays;

public class LeaderMode extends RaftMode {
  private Timer heartbeatTimer;
  private int[] nextIndex = new int[mConfig.getNumServers() + 1];
  private int timerid = 0;

  private void initiate_heartbeatTimer() {
    timerid = (mID+29)*mConfig.getCurrentTerm();
    heartbeatTimer = scheduleTimer(HEARTBEAT_INTERVAL, timerid);
  }

  private void sendEntries (int i) {
    Entry[] entries = null;
      if(nextIndex[i] == nextIndex[mID]) {
        entries = new Entry[0]; 
      }
      if(nextIndex[i] < nextIndex[mID]) {
        entries = new Entry[ (nextIndex[mID]- nextIndex[i]+1) ]; 
        for(int j=nextIndex[i]; j<nextIndex[mID]; j++) {
          entries[(j-nextIndex[i])] = mLog.getEntry(j);
        }
      }
      remoteAppendEntries(i, mConfig.getCurrentTerm(), mID, nextIndex[i]-1, 
        mLog.getEntry(nextIndex[i]-1).term, entries, mCommitIndex); 
  }

  public void go () {
    synchronized (mLock) {
        initiate_heartbeatTimer();
        for (int i=1; i<nextIndex.length; i++) { nextIndex[i] = mLog.getLastIndex() + 1; }
        RaftResponses.clearAppendResponses(mConfig.getCurrentTerm());

        for (int i = 1; i <= mConfig.getNumServers(); i++) {
          sendEntries(i);         
        } 

        System.out.println ("S" + 
         mID + 
         "." + 
         mConfig.getCurrentTerm () + 
         ": switched to leader mode.");
      

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

      if (candidateTerm < mConfig.getCurrentTerm ()) {
        return mConfig.getCurrentTerm ();     
      } 

      if ( candidateTerm > my_current_term ) {
        //when candidateterm > my_current_term
        if (lastLogTerm > my_last_term ) { 
          heartbeatTimer.cancel();             
          mConfig.setCurrentTerm(candidateTerm, candidateID);
          RaftServerImpl.setMode(new FollowerMode());
          return 0;
        }
        if(lastLogTerm == my_last_term){  
          if(lastLogIndex >= my_last_index) { 
            heartbeatTimer.cancel();
            mConfig.setCurrentTerm(candidateTerm, candidateID);
            RaftServerImpl.setMode(new FollowerMode());
            return 0;
          } 
        }
        else { 
            heartbeatTimer.cancel();             
            mConfig.setCurrentTerm(candidateTerm, 0);   // none
            RaftServerImpl.setMode(new FollowerMode());
            return my_current_term;
        }
      }
        
      if( my_votefor == 0 || my_votefor == candidateID) {
        //when candidateTerm == my_current_term
        //my_votefor == candidateID happen only when candidate receive their own requestvote
        if (lastLogTerm > my_last_term ) { //Figure 7, b,e,f
          heartbeatTimer.cancel();
          mConfig.setCurrentTerm(candidateTerm, candidateID);
          RaftServerImpl.setMode(new FollowerMode());
          return 0;
        }
        if(lastLogTerm == my_last_term){  
          if(lastLogIndex > my_last_index) { //Figure 7, a
            heartbeatTimer.cancel();
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
    }//end of syn  
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

      if (leaderID == mID) { return -2;      }     // itself, return same value as heartbeat, handleTimeout do nothing
      
      if (leaderTerm < my_current_term) {
        return mConfig.getCurrentTerm();
      }
      
      if (leaderTerm > my_current_term) {
        heartbeatTimer.cancel(); 
        mConfig.setCurrentTerm(leaderTerm, 0);     // or leaderID?
        RaftServerImpl.setMode(new FollowerMode());
        return -2;
      }

      else {                // equal term
        if (prevLogTerm > my_current_term) {       // or >=
          heartbeatTimer.cancel(); 
          mConfig.setCurrentTerm(leaderTerm, 0);     // or leaderID?
          RaftServerImpl.setMode(new FollowerMode());
          return -2;             
        } else {
          if(prevLogTerm == my_last_term){  
            if(prevLogTerm > my_last_index) { //Figure 7, a
              heartbeatTimer.cancel();
              mConfig.setCurrentTerm(leaderTerm, 0);
              RaftServerImpl.setMode(new FollowerMode());
              return -2;
            }
          }
        }
        return my_current_term; 
      }

    }//end of synchronization
  }

  // @param id of the timer that timed out
  public void handleTimeout (int timerID) {
    synchronized (mLock) {
      if (timerID == timerid) {
        int[] appendResponses = RaftResponses.getAppendResponses(mConfig.getCurrentTerm());
        // System.out.println("leader is: "+mID);
        // System.out.println("RaftResponses");
        // System.out.println(Arrays.toString(appendResponses));
        for(int i = 1; i < appendResponses.length; i++) {
          if (appendResponses[i] > mConfig.getCurrentTerm()) {
            heartbeatTimer.cancel(); 
            mConfig.setCurrentTerm(appendResponses[i], 0);     // or leaderID?
            RaftServerImpl.setMode(new FollowerMode());
            return; 
          }
          if (appendResponses[i] == 0) {    // append success
            nextIndex[i]=mLog.getLastIndex() + 1;
          }
          if (appendResponses[i] == -4) {
            nextIndex[i]--;
          } 
        }
        // System.out.println("updated nextIndex");
        // System.out.println(Arrays.toString(nextIndex));
        initiate_heartbeatTimer();
        RaftResponses.clearAppendResponses(mConfig.getCurrentTerm());
        for (int i = 1; i <= mConfig.getNumServers(); i++) {
          sendEntries(i);         
        } 
      }
      // send heartbeat?
      
    }
  }
}
