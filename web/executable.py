from subprocess import Popen, PIPE, STDOUT
from threading import Thread
from queue import Queue, Empty


class Executable:
    def __init__(self, s):
        self._exe = Popen(s, stdin=PIPE, stdout=PIPE, stderr=STDOUT, encoding='utf-8')
        self._q = Queue()

        def __read(stream, q):
            while True:
                line = stream.readline()
                if line:
                    q.put(line)
                else:
                    raise EndOfStream

        self._t = Thread(target=__read,
                         args=(self._exe.stdout, self._q))
        self._t.daemon = True
        self._t.start()

    def __readline(self, timeout=None):
        try:
            return self._q.get(block=timeout is not None,
                               timeout=timeout)
        except Empty:
            return None

    def exec(self, s, timeout=0.1):
        self._exe.stdin.write('{}\n'.format(' '.join(s)))
        self._exe.stdin.flush()
        ret = []
        while True:
            ln = self.__readline(0.1)
            if not ln:
                break
            ret.append(ln.rstrip('\n'))
        return ret


class EndOfStream:
    pass
