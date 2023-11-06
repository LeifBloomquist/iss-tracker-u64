@echo off
set PRG=iss-u64.prg
cls
del %PRG%
make
u64remote 192.168.7.64 run %PRG%
