#!/usr/bin/env python

from DIRAC.Core.Base import Script
Script.initialize()

import sys
import os
import getopt

from DIRAC.Core.DISET.RPCClient import RPCClient
from DIRAC.DataManagementSystem.Client.DataManager import DataManager
from DIRAC.Core.Security.ProxyInfo import getProxyInfo


class RecursiveRm(object):
  def __init__(self):
    self.__rpcclient = RPCClient( "DataManagement/FileCatalog" )
    self.__dm = DataManager()
    self.__n_dirs = 0
    self.__n_files = 0
    self.__space_freed = 0L

  def clear_directory(self, directory_path, se_name, dry_run=True):

    print "Clearing directory: %s" % directory_path
    
    dir_content =  self.__rpcclient.listDirectory(directory_path, False)

    if not dir_content["OK"]:
      print "Failed to contact DIRAC server for %s" % directory_path
      return
    
    if directory_path in dir_content['Value']['Failed']:
      print "Could not access %s, maybe it doesn't exist?" % directory_path
      return

    subdirs = dir_content['Value']['Successful'][directory_path]['SubDirs']
    for subdir in subdirs.keys():
      self.clear_directory(subdir, se_name, dry_run)


    # Now do files...
    files = dir_content['Value']['Successful'][directory_path]['Files']
    for filename in files.keys():
      fullpath = os.path.join(directory_path, filename)
      if self.clear_file(fullpath, se_name, dry_run):
        self.__n_files += 1
        self.__space_freed += files[filename]['MetaData']['Size']

    if self.remove_empty_dir(directory_path, dry_run):
      self.__n_dirs += 1
      

  def remove_empty_dir(self, directory_path, dry_run=True):
    # check if directory is now empty and the remove the directory
    dir_content =  self.__rpcclient.listDirectory(directory_path, False)

    if not dir_content["OK"]:
      print  "Could not access %s" % directory_path
      return False


    subdirs = dir_content['Value']['Successful'][directory_path]['SubDirs']
    files = dir_content['Value']['Successful'][directory_path]['Files']
  
    if not subdirs and not files:
      if not dry_run:
        self.__dm.fc.removeDirectory(directory_path, recursive=False)
      return True

    return False


  def clear_file(self, filename, se_name, dry_run=True):
  
    res = self.__rpcclient.getReplicas(filename, False)
    if not res["OK"]:
      print "Could not get replica status for %s" % filename
      return False
  
    ses = res['Value']['Successful'][filename].keys()
    
    # remove file regardless of number of replicas
    if se_name == "Any":
      print "%s" % filename
      if not dry_run:
        deleted = self.__dm.removeFile(filename)
        if not deleted["OK"]:
          print "Function call to removeFile failed, file not deleted: %s" \
                                                            % str(deleted)
          return False
        elif not deleted["Value"]["Successful"]:
          print "Failed to delete file: %s" % str(deleted)
          return False

      return True

    # file exists only at the chosen SE
    #     -> delete file and remove from file catalogue
    if len(ses) == 1 and se_name in ses:
      print "%s" % filename
      if not dry_run:
        deleted = self.__dm.removeFile(filename)
        if not deleted["OK"]:
          print "Function call to removeFile failed, file not deleted: %s" \
                                                            % str(deleted)
          return False
        elif not deleted["Value"]["Successful"]:
          print "Failed to delete file: %s" % str(deleted)
          return False
      return True
        
    # file exists at the chosen SE and elswhere -> delete replica at chosen SE
    if len(ses) > 1 and se_name in ses:
      print "%s" % filename
      if not dry_run:
        deleted = self.__dm.removeReplica(se_name, filename)
        if not deleted["OK"]:
          print "Function call to removeReplica failed, replica not deleted: %s" \
                                                                  % str(deleted)
          return False
        elif not deleted["Value"]["Successful"]:
          print "Failed to delete replica: %s" % str(deleted)
          return False


      return True

    return False
     

  def print_stats(self):
    print ""
    print "Number of files deleted: %s" % self.__n_files
    print "NUmber of (sub)directories deleted: %s" % self.__n_dirs
    space = self.__space_freed/(1024.0 * 1024.0 * 1024.0)
    print "Space freed: %0.3f GB" % space

 
def main():


  dry_run = False
  opts, args = getopt.getopt(sys.argv[1:], "n")  

  if len(args) < 2:
    print "Usage: recursiverm.py [-n] <SE> <LFN> [<LFN>...]"
    print "       -n: dryRun (list files to be deleted)"
    print "Use 'Any' for <SE> to delete all replicas in a directory"
    print "Example: recursiverm.py -n UKI-LT2-IC-HEP-disk /vo.londongrid.ac.uk/test2"
    sys.exit(1)

  proxy_info = getProxyInfo()   
  if not "VOMS" in proxy_info["Value"] or not proxy_info["Value"]["VOMS"]:
    print "Error: Your proxy does not contain a VOMS signature."
    print "(Try using dirac-proxy-init -g [voname]_user -M)"
    sys.exit(2)


  for opt, _ in opts:
    if opt == "-n":
      dry_run = True
      print "dryRun only: list files to be deleted"

  rrm = RecursiveRm()

  paths = []
  for tpath in args[1:]:
    tpath = os.path.normpath(tpath)
    if len(tpath.split("/")) < 3:
      print "Sorry, I can't let you do that Dave: Path %s is too broad." \
                                                                    % tpath
      sys.exit(3)
    paths.append(tpath)

  se_name = args[0]
  for tpath in paths:
    rrm.clear_directory(tpath, se_name, dry_run)

  rrm.print_stats()   


if __name__ == "__main__":
  main()


