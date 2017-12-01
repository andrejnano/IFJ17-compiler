# Quick tests for ifj2017 compilator
# xmarko15 Peter Marko
# xmechl00 Stanislav Mechl
# xnanoa00 Andrej Nano
# xsvand06 Švanda Jan

from testc import *

def loadTests (tests):

    tests.add("SEM - Short", "scope\nend scope", [0])

    tests.add("LEX - Unknown char", "{", [1])
    tests.add("LEX - Unknown char", "%", [1])
    tests.add("LEX - Unknown char", "´", [1])
    tests.add("LEX - Unknown char", "ě", [1])
    tests.add("LEX - Float error", ".14", [1])
    tests.add("LEX - Float error", "13.", [1])
    tests.add("LEX - Float error", "1e", [1])
    tests.add("LEX - Float error", "1e+", [1])
    tests.add("LEX - Float error", "1e-", [1])
    tests.add("LEX - Float error", "1.e", [1])
    tests.add("LEX - Float error", "1.e-", [1])
    tests.add("LEX - Float error", "1.e+", [1])
    tests.add("LEX - Float error", "1..2", [1])
    tests.add("LEX - Single line comment", "scope'comment\nend scope", [0])
    tests.add("LEX - Single line comment", "scope'''' '' ' ¨¨%¨'\nend scope     '--!", [0])
    tests.add("LEX - Single line comment", "'scope scope scope\nscope 'scope\nend scope\n'end", [0])
    tests.add("LEX - Multiline comment", "/'comment'/scope/'c'/\nend/'space'/ scope", [0])

    tests.add("SEM - Dim x", "scope\nDiM X as INTEger\nx    = 7  \n\nend scope", [0])

    tests.add("SYN - min source code", "'minimal source code test\nscope\nend scope", [0])
    tests.add("SYN - variable declarations", "scope\nDim x as Integer\nDim y as String\nDim z as Double\nend scope", [0])
    tests.add("SYN - variable definitions", "scope\nDim a as Integer = 42\nDim b as Double = 4.2\nDim c as Double = 3e-5\nend scope", [0])

    tests.add("SYN - function definition","function func() as Integer\n\nend function\n\nscope\nend scope", [0])
    tests.add("SYN - function definition with parameter","function func(n as Integer) as Integer\n\nend function\n\nscope\nend scope", [0])

    tests.add("SYN - function definition", "function func() as Integer\n\nend function\n\nscope\nend scope", [0])
