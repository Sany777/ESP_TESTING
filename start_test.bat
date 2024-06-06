@echo off
cd "./test/"
idf.py set-target esp32
idf.py build   
@REM pytest
idf.py -p COM11 flash monitor