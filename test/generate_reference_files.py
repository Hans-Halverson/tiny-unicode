#! python3

def in_surrogate_range(codepoint):
  return i >= 0xD800 and i <= 0xDFFF

str = ""
for i in range(0, 0x110000):
  if not in_surrogate_range(i):
    str += chr(i)

with open('all-utf-8.txt', 'wb') as f:
  f.write(str.encode('utf-8-'))

with open('all-utf-16.txt', 'wb') as f:
  f.write(str.encode('utf-16-be'))