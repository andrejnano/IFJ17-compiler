# Quick tests for ifj2017 compilator
# xmarko15 Peter Marko
# xmechl00 Stanislav Mechl
# xnanoa00 Andrej Nano
# xsvand06 Švanda Jan

class TestItem (object):

    def __init__(self):
        self.desc = 'Desc'
        self.stdin = 'scope\nend scope'
        self.expret = [0]

class TestList (list):

    def __init__(self):
        self.test_count = 0
        self.test_success = 0

    def add(self, desc, stdin, expret):
        test = TestItem()
        test.desc = str(desc)
        test.stdin = str(stdin)
        test.expret = expret
        self.append(test)
        self.test_count += 1

    def success(self):
        self.test_success += 1

    def __str__(self):
            return str('Test status: {}/{} OK ({:0.2f}%)'.format(
                self.test_success,
                self.test_count,
                self.test_success/self.test_count*100
                ))

    def __iter__(self):
        for i in range(0, self.test_count):
            yield self[i]
