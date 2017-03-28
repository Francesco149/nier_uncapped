@echo off

cl 	-D_CRT_SECURE_NO_WARNINGS=1 ^
	-DNOMINMAX=1 ^
	-nologo -MT -LD -Gm- -GR- -EHsc -W4 -WX ^
	-wd4201 ^
	-wd4100 ^
	-wd4054 ^
	-wd4047 ^
	-wd4152 ^
	-wd4055 ^
	-wd4459 ^
	nier_uncapped.c ^
	USER32.LIB ^
	-Fenier_uncapped.dll
