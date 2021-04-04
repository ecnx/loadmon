How to use?
-----------
```
$ loadmon
A: 15.1% U: 12.1% K: 3.0% M: 54.6%
A: 8.5% U: 6.0% K: 2.5% M: 54.6%
A: 11.7% U: 9.6% K: 2.0% M: 54.6%

$ loadmon -g
###XXXXXXXX.........................................................................................
#######XXXXXXXXXXX..................................................................................
###XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX............................................
(# - kernel mode load, X - user mode load)
```

Usage message:
--------------
```
usage: loadmon [-3gh]

options:  -3  three check only
  -g  print graph
  -h  print help

symbols explanation:
  A - total cpu usage for both modes
  U or X - user mode cpu usage
  K or # - kernel mode cpu usage
```
