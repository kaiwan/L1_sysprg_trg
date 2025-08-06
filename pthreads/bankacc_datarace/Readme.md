The classic / pedagogical bank account synchronization example in code !

Boils down to:
a) good case: we do use a sync primitive - the pthread mutex - when depositing
   & withdrawing money from the bank account.
b) bad case: we don't use any sync primitive when depositing & withdrawing
   money from the bank account

Usage:
$ ./bankacc_datarace_dbg
Usage: ./bankacc_datarace_dbg opt
opt=0 : good case - NO data race as a mutex is used
opt=1 : bad case - classic data race as no mutex's used
$ 

*NOTE* We MUST use the 'debug' binary executable to test this (else the compiler
simply optimizes out our repeated incr/decr rendering it useless).

Sample runs:
a) Good case:
$ ./bankacc_datarace_dbg 0
Final balance: 0
$ ./bankacc_datarace_dbg 0
Final balance: 0
$ 

b) Bad case:
$ ./bankacc_datarace_dbg 1
Final balance: 654852
$ ./bankacc_datarace_dbg 1
Final balance: 702021
$ ./bankacc_datarace_dbg 1
Final balance: -348543
$ 


