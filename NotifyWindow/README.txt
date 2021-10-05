NotifyWindow

notify.exe
    Popup a window to show a notify (using WinAPI and not MFC)
    Usage:
        notify [content_from_file=] [content_font=] [content=] [title=] [close=]
    Paramters:
        content_from_file=<filename> The content will from <filename>
        content_font=<FontName>      Set font
        content=<text>               The content will be the <text>
        title=<text>                 The title will be the <text>
        close=<auto|dwMillSeconds>   If have this argument, the window will close after dwMillSeconds. If used "auto", dwMillSeconds is 15000.
    Example:
        notify
        notify content=test
        notify content_from_file=demo1.txt title=ovo
        notify content=2333 content_font=楷体 title=qwq
