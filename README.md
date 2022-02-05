# COP-3404-System-Software-SIC-Assembler
Pass1: In this pass 1 the SIC file get read and after that the sysmbol table get created. 

Pass2: The SIC file get read and after that the H,T,M,E record get created for each line depending if it is symbol, directive, instruction, and end it created recods from there.

Clock: This is a SIC clock, whcih can show the output for 24 hours or 12 hors depending on what is the starting number is. For example it is 800000 then its a 24 hour format, if it is 000000 then its a 12 hours format.

SIC/XE pass: In this SIC/XE pass the code has an ability to handle n,i,x,b,p,e bit. Depending on which bits are flipped it can choose format 3 or 4. Otherwise it can select format 2 or 1 depending on how many register the following instruction and directive has. In this code it created H,T,M,E records. The only M recods that are created in this are, when b,p bit fails to flipped in format 3 addressing mode. In that case the SIC/XE code has an ability to got back to simple addressing mode SIC. 
