
# Example MicroPython code
import sys
import json

with open('out.txt', 'w') as f:
    o = sys.implementation
    s = json.dumps(dict(info=repr(o)))
    print(s)
    f.write(s)
