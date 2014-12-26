@ECHO OFF
@set INCLUDE=E:\Applications\vc2008\include;E:\Applications\vc2008\PlatformSDK\Include;E:\Applications\vc2008\atlmfc\include;E:\Applications\vc2008\DirectX_SDK\Include;
@set LIB=E:\Applications\vc2008\lib;E:\Applications\vc2008\PlatformSDK\Lib;E:\Applications\vc2008\atlmfc\lib;E:\Applications\vc2008\DirectX_SDK\Lib\x86;
RC ..\RES\ELEEYE.RC
CL /DNDEBUG /O2 /W3 /Fe..\BIN\ELEEYE.EXE ..\BASE\PIPE.CPP UCCI.CPP PREGEN.CPP POSITION.CPP GENMOVES.CPP HASH.CPP BOOK.CPP MOVESORT.CPP PREEVAL.CPP EVALUATE.CPP SEARCH.CPP ELEEYE.CPP SHLWAPI.LIB ..\RES\ELEEYE.RES
DEL ..\RES\ELEEYE.RES
DEL *.OBJ
