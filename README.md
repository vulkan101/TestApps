# TestApps
### TestSignatures
Test code to verify encode/decode of function signatures into a 16bit value. 
This is for compatibility with .NET runtime.

Encodes whether parameters are 4 (0b01) or 8 (0b10) bytes as 2 bit values (0b01 and 0b10 respectively), shifts the first parameter code is always shifted left 12 bits, with up to 6 parameters. The last 2 bits are used to indicate the return type.

Will be adding  enum generation code - probably python
