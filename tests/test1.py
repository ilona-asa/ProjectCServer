#!/usr/bin/python
# -*- coding: utf-8 -*-

##################################################################
# test-{1,2,3}.py
#
# Perform a bunch of tests on the webserver
#
# Course: 1DT032
# (c) 2015, 2016 German Ceballos <german.ceballos@it.uu.se>
##################################################################

import time, os, sys, errno, getopt, shutil, getpass
import threading

import subprocess
import socket
thread_dict = {}


# Gobal variables defaults
DEVNULL = open(os.devnull, 'wb', 0)

options = {
    'clients' : 10,
    'port' : 2020,
    'server_dir' : '../server-mt/',
    'testfile_dir' : '../server-mt/' + getpass.getuser() + '/testfiles/',
    'testfile_name' : 'output.'
    }
    
filenames = {}

def usage():
    print "./test{1,2,3}.py -p=port -c=clients -d=server-directory"
    print "-p|--port DEFAULT: 2020"
    print "-c|--clients DEFAULT: 1"
    print "-d|--server-dir DEFAULT: ../server-mt/"

def recvall(sock):
    data = ""
    part = None
    while part != "":
        part = sock.recv(1000)
        data += part
    return data


def check_valid_if_start():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hp:c:d:", ['help', 'port=', 'client=', 'server-dir='])
    except getopt.GetoptError, e:
        # print help information and exit:
        print e
        usage()
        sys.exit(1)

    for o, a in opts:
        if o in ('-h', '--help'):
            usage()
            sys.exit(0)
        if o in ('-p', '--port'):
            options['port'] = int(a)
        if o in ('-c', '--client'):
            if a < 1:
                print "ERROR: Number of clients should be greater than 0."
                sys.exit(1)
            options['clients'] = int(a)
        if o in ("-d", "--client-dir"):
            options['server_dir'] = a           
    
def setup():
    """
    Prepares the temp files
    """
    clients = int(options['clients'])
    server_dir = options['server_dir']
    testfile_dir = options['testfile_dir']
    testfile_name = options['testfile_name']

    if not os.path.exists(testfile_dir):
        os.makedirs(testfile_dir)

    for i in range(clients):
        filename = '/' + str(testfile_dir) + '/' + str(testfile_name) + str(i+1)
        filenames[i] = filename
        count = 10
        command = ['dd', 'if=/dev/zero', 'of=' + str(server_dir) + str(filename), 'bs=1M', 'count=' + str(count)]
        p = subprocess.Popen(command, stdout=DEVNULL, stderr=subprocess.STDOUT)
        p.wait()
        # TODO (German): Fix error handling here.

        print "Creating dummy file: %s with size %d MB." % (testfile_name + str(i+1), count)

def teardown():
    """
    Cleans up the temp files
    """
    testfile_dir = options['testfile_dir']

    rmdir = str(testfile_dir)
    print "Removing temp dir: %s" % rmdir
    shutil.rmtree(rmdir)

def client(thread_id, filename):
    """
    Spawns @clients threads 
    """
    port = options['port']
    server_dir = options['server_dir']

    cmd = server_dir + '/client'
    command = [cmd, 'localhost', str(port), filename, "no"]
    
    start = time.time()
    p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    ru = os.wait4(p.pid, 0)[2]
    streamdata = p.communicate()[0]
    if 'refused' in streamdata:
        print "ERROR: Client %d failed to connect. Test failed. Please try again." % thread_id
    else:
        print "Finished: ", filename

    elapsed = time.time() - start
    
    thread_dict[thread_id] = ru.ru_utime;

def main():
    """
    Main entry point
    """
    
    # Check input options
    check_valid_if_start()

    clients = int(options['clients'])
    port = options['port']
    server_dir = options['server_dir']
    testfile_dir = options['testfile_dir']
    testfile_name = options['testfile_name']

    print "Server test script"
    print "Server working dir: ", server_dir
    print "Working on port: ", port
    print "Number of clients to be spawned: ", clients

    # Prepare the dummy files
    setup()

    # Create clients
    threads = []
    for i in range(clients):
        t = threading.Thread(target = client, args = (i, filenames[i],))
        threads.append(t)

    start = time.time()

    # Start threads
    for t in threads:
        t.start() 

    for t in threads:
        t.join() 
        
    end = time.time()

    # Print statistics of the thread
    print "The time was ", str(end - start)
    print "Average client processing time was ", (end-start)/clients

    # Cleanup
    teardown()

        
if __name__ == '__main__':
    main()


