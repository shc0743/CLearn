Set sh=wscript.createobject("Wscript.Shell")
s=Inputbox("Service name")
sh.run "MyProcControl64.exe --ui --uninstall --slient --service-name="&s,0