'https://stackoverflow.com/questions/60034/how-can-you-find-and-replace-text-in-a-file-using-the-windows-command-line-envir
'Run from command line with the following: cscript replace.vbs "C:\Scripts\Text.txt" "Jim " "James "

Const ForReading = 1    
Const ForWriting = 2

strFileName = Wscript.Arguments(0)
strOldText = Wscript.Arguments(1)
strNewText = Wscript.Arguments(2)

'Process \r\n replacement in search and replace strings
strOldText = Replace(strOldText, "\r\n", vbNewLine)
strNewText = Replace(strNewText, "\r\n", vbNewLine)

'Process chr(34) replacement in seach and replace strings
'https://stackoverflow.com/questions/4192376/double-quotes-in-vbscript-argument
strOldText = Replace(strOldText, "chr(34)", chr(34))
strNewText = Replace(strNewText, "chr(34)", chr(34))
strOldText = Replace(strOldText, "&quot;", chr(34))
strNewText = Replace(strNewText, "&quot;", chr(34))

Set objFSO = CreateObject("Scripting.FileSystemObject")
Set objFile = objFSO.OpenTextFile(strFileName, ForReading)
strText = objFile.ReadAll
objFile.Close

strNewText = Replace(strText, strOldText, strNewText)
Set objFile = objFSO.OpenTextFile(strFileName, ForWriting)
objFile.Write strNewText  'WriteLine adds extra CR/LF
objFile.Close
