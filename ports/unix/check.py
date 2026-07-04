
# Example MicroPython code
import sys
import json

out_file = 'out.txt'
with open(out_file, 'w') as f:
    o = sys.implementation
    s = json.dumps(dict(info=repr(o)))
    print(s)
    f.write(s)

print('Wrote', out_file)
