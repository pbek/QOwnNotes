' Unzips a zip file to a folder
' cscript //B unzip.vbs <zip-file> <destination-folder>
'
' example:
' cscript //B unzip.vbs C:\path\to\QOwnNotes.zip C:\folder\to\extract

Set argObj = WScript.Arguments

' The location of the zip file.
If (Wscript.Arguments.Count > 0) Then
 zipFile = argObj(0)
Else
 zipFile = ""
End if

' The folder the contents should be extracted to.
If (Wscript.Arguments.Count > 1) Then
 extractTo = argObj(1)
Else
 extractTo = ""
End if

'WScript.Echo ( "zipFile: " & zipFile )
'WScript.Echo ( "extractTo: " & extractTo )

' If the extraction location does not exist create it.
Set fso = CreateObject("Scripting.FileSystemObject")
If NOT fso.FolderExists(extractTo) Then
   fso.CreateFolder(extractTo)
End If

' Extract the contants of the zip file.
set objShell = CreateObject("Shell.Application")
set FilesInZip = objShell.NameSpace(zipFile).Items()
objShell.NameSpace(extractTo).CopyHere(FilesInZip)
Set fso = Nothing
Set objShell = Nothing
