#!/usr/bin/python
import os
import subprocess
import threading
import multiprocessing

# lock to synchronize the message printing
lock = threading.Lock()

def tprint(msg):
  global lock
  lock.acquire()
  print msg
  lock.release()
  
class threadPool():# contains as many threads as "multiprocessing.cpu_count"
  def __init__(self, batchID, program, target_path, outpattern, outextension):
    self.threads = []
    self.batchID = batchID
    self.program = program
    self.target_path = target_path
    self.outpattern = outpattern
    self.outextension = outextension
    for k in xrange(multiprocessing.cpu_count()):
      self.threads.append(covThread(batchID+k, program, target_path, outpattern, outextension)) #initialise the threads with the program and its arguments
  def run(self):
    for th in self.threads:
      th.start()
    for th in self.threads:#block the current thread (that of the main function) until every thread is finished
      th.join() #join to the current thread and wait for threads to finish

class covThread (threading.Thread):#override the constructor to save the program and the and its arguments in the threads
  def __init__(self, threadID, program, target_path, outpattern, outextension):
    threading.Thread.__init__(self)
    self.threadID = threadID
    self.program = program
    self.target_path = target_path
    self.outpattern = outpattern
    self.outextension = outextension
    
  def run(self):
    tprint(" ".join([self.program, self.target_path, self.outpattern+str(self.threadID)+self.outextension]))
    devnull = open(os.devnull, 'w')#to hide the output of the program
    proc = subprocess.Popen([self.program, self.target_path, self.outpattern+str(self.threadID)+self.outextension], stdout = devnull)
    proc.wait()

def Generate(program, target_path, outpattern, outextension, iteration):
  threads = []
  for k in xrange(iteration/multiprocessing.cpu_count()):
    pool = threadPool(1000*(k+1), program, target_path, outpattern, outextension)#create a batch thread of n threads
    pool.run()
  
def Compare(program, target_path, outfile):
  proc = subprocess.Popen([program, target_path, outfile])
  proc.wait()

def main():
  
  Generate("./Covariance","/home/vsibille/Documents/dchooz/cosmogenic/Simulations/Helium_gaus/Gd/Output", "test", ".root", 12)
  Compare("CompareCans", ".", "comp.root")
  

if __name__ == "__main__":
  main()
