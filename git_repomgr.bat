@echo off
if "%1"=="" goto help
if "%1"=="/?" goto help
if "%1"=="--help" goto help
if "%1"=="add" goto add
if "%1"=="commit" goto commit
if "%1"=="push" goto push
if "%1"=="acp" goto add_commit_push

:error
echo Invalid arguments: %*
exit /b 1

:help
echo Usage:
echo     %0 [/?] (add^|commit^|push^|acp)
echo;
echo --help                        Show this text.
echo add                           git add
echo commit [CommitDescription]    git commit [-m "CommitDescription"]
echo push                          git push    (auto reconnect if error)
echo acp [CommitDescription]       Simultaneous execution `%0 add` `%0 commit [-m "CommitDescription"]` and `%0 push`
exit /b 0

:add
git add .
git add -u .
git add -A .
exit /b %ERRORLEVEL%

:commit
if "%2"=="" (
git commit
exit /b %ERRORLEVEL%
)
git commit -m "%2"
exit /b %ERRORLEVEL%

:push_retry
echo Push failed, retry...
:push
git push
set "el=%ERRORLEVEL%"
if "%el%"=="128" goto push_retry
exit /b %el%

:add_commit_push
cmd /c %0 add
cmd /c %0 commit "%2"
cmd /c %0 push
exit /b %ERRORLEVEL%
