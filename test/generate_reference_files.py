#! python3

UTF8_FILE = 'all-utf-8.txt'
UTF16_FILE = 'all-utf-16.txt'
UTF32_FILE = 'all-utf-32.txt'

def in_surrogate_range(codepoint):
  return i >= 0xD800 and i <= 0xDFFF

str = ""
for i in range(0, 0x110000):
  if not in_surrogate_range(i):
    str += chr(i)

with open(UTF8_FILE, 'wb') as f:
  f.write(str.encode('utf-8'))

with open(UTF16_FILE, 'wb') as f:
  # Exclude byte order marker
  f.write(str.encode('utf-16')[2:])

with open(UTF32_FILE, 'wb') as f:
  f.write(str.encode('utf-32')[4:])