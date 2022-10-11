' Unzips a zip file to a folder
' cscript //B unzip.vbs <zip-file> <destination-folder>
'
' example:
' cscript //B unzip.vbs C:\path\to\QOwnNotes.zip C:\folder\to\extract

Set argObj = WScript.Arguments

If (Wscript.Arguments.Count < 2) Then
    WScript.Echo ( "Please enter both parameters!" )
    WScript.Quit
End If

' The location of the zip file.
zipFile = argObj(0)

' The folder the contents should be extracted to.
extractTo = argObj(1)

WScript.Echo ( "zipFile: " & zipFile )
WScript.Echo ( "extractTo: " & extractTo )

' If the extraction location does not exist create it.
Set fso = CreateObject("Scripting.FileSystemObject")
If NOT fso.FolderExists(extractTo) Then
   WScript.Echo ( "creating folder" )
   fso.CreateFolder(extractTo)
End If

' Extract the contents of the zip file.
set objShell = CreateObject("Shell.Application")
set FilesInZip = objShell.NameSpace(zipFile).Items()
objShell.NameSpace(extractTo).CopyHere(FilesInZip)

'WScript.Echo ( "objShell: " & objShell )
'WScript.Echo ( "FilesInZip: " & FilesInZip )

Set fso = Nothing
Set objShell = Nothing

WScript.Echo ( "Done" )
'WScript.Sleep 50000
