#!/usr/bin/env python3

# Quick tests for ifj2017 compilator
# xmarko15 Peter Marko
# xmechl00 Stanislav Mechl
# xnanoa00 Andrej Nano
# xsvand06 Švanda Jan

from subprocess import * # CompletedProcess, run, PIPE, TimeoutExpired
from platform import system
from os import path
from testc import TestList
from tests import loadTests

# Default compiler and interpreter for linux
compiler = './ifj2017'
interpreter = './ic17int'
if not path.exists(interpreter):
    interpreter = 'tests/ic17int'

# Switch to windows interpreter
if system() in ['Windows']:
    compiler = '../ifj2017.exe'
    interpreter = 'ic17int.exe'

# Load tests from tests file
test_list = TestList()
loadTests(test_list)

# Create process variable
proc = CompletedProcess

# Run all tests
for test in test_list:
    try:
    
        # Try to run compiler
        proc = run([compiler], stdout=PIPE, stderr=PIPE, timeout=3, encoding='utf-8', input=test.stdin)

        # Check compiler return code
        if proc.returncode in test.expret:

            # Should be valid code, run interpreter
            if proc.returncode == 0:

                # Store compiled code to test output
                with open('testout', 'w') as file:
                    file.write(proc.stdout)
                
                try:

                    # Try to run interpreter
                    proc = run([interpreter, 'testout'], stdout=PIPE, stderr=PIPE, timeout=5)

                    # Check success
                    if proc.returncode == 0:
                        test_list.success()
                    else:
                        
                        # Interpreter returned non zero value
                        print('[{}] Interpret error: {}'.format(test.desc, proc.returncode))
                        print(proc.stdout, '\n', ' -'*40)
                        
                except TimeoutExpired:
                    print('[{}] Interpretr timeout expired'.format(test.desc))
                except:
                    print('[{}] Interpreter coding error?'.format(test.desc))
            
            else:
                # Not to be interpreted
                test_list.success()

        else:
            
            # Compiler test failed
            print('[{}] Compiler failed: expected {} got {}'.format(
                test.desc, ' or '.join(str(el) for el in test.expret), proc.returncode))
            print(test.stdin, '\n', ' -'*40)

    except TimeoutExpired:
        print('[{}] Compiler timeout expired'.format(test.desc))
    except:
        print('[{}] Compiler coding error?'.format(test.desc))

# Print results
print(test_list)
with open('testout', 'w') as file:
    file.write(str(test_list))

# Windows wait
if system() in ['Windows']:
    input('Press enter to exit.')
