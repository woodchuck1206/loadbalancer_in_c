import subprocess
import time
import signal
import socket

LBSERVER = './loadbalancer.out'
LBENDPOINT = ('127.0.0.1', 8888)

def timer(func):
    def inner(*args, **kwargs):
        tic = time.time()
        func(args[0])
        print('function {} complete, {} sec taken...'.format(func.__name__, time.time()-tic))
    return inner

@timer
def test1(*args, **kwargs):
    tests = 0
    tic = time.time()
    duration = 1
    while time.time() - tic < duration:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            tests += 1
            sock.connect(LBENDPOINT)
            time.sleep(args[0])
    print('{} tests run in {} sec...'.format(tests, duration))
        
tests = [test1, ]

if __name__ == '__main__':
    init = 0.05
    while True:
        test1(init)
        init /= 2
"""
    for idx, test in enumerate(tests):
        print('Test {} on function {}'.format(idx+1, test.__name__))
        test()
        print('-'*20)
""" 
