@echo off
rem
rem BAT script to launch BASH file
rem Circuitous, but it sped up my recompile cycle within XEmacs
rem
bash -c ". ~/.bashrc; ./compile.bash"
