On Error Resume Next

Function X86orX64()
    'Author: Demon
    'Date: 2011/11/12
    'Website: http://demon.tw
    On Error Resume Next
    strComputer = "."
    Set objWMIService = GetObject("winmgmts:\\" & strComputer & "\root\cimv2")
    Set colItems = objWMIService.ExecQuery("Select * from Win32_ComputerSystem",,48)
    
    For Each objItem in colItems
        If InStr(objItem.SystemType, "86") <> 0 Then
            X86orX64 = "x86"
        ElseIf InStr(objItem.SystemType, "64") <> 0 Then
            X86orX64 = "x64"
        Else
            X86orX64 = objItem.SystemType
        End If
    Next
    
End Function

Dim url
url=InputBox("Please input file full path or URL","OpenHTMLFile","")
If url="" Then
    Wscript.Quit
End If

Set ws=CreateObject("Wscript.Shell")

if X86orX64() = "x64" Then
    fname="OpenHTMLFile64"
Else 
    fname="OpenHTMLFile32"
End If

ws.Run fname&" --con "&url ,1,false
