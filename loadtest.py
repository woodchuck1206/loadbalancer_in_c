import subprocess
import time
import signal
import requests

LBSERVER = './loadbalancer.out'
LBENDPOINT = 'http://localhost:8888'

def test1():
    tests = 0
    tic = time.time()
    while time.time() - tic < 1:
        test += 1
        requests.get(LBENDPOINT)
        
tests = [test1, ]
if __name__ == '__main__':
