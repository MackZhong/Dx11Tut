https://msdn.microsoft.com/en-us/library/ee416788(v=vs.85).aspx


How do I debug using the Windows symbol files?

Microsoft publish stripped symbols for all system DLLs (plus a few others). To access them add the following to your symbol path in the project settings inside Visual Studio:


srv*http://msdl.microsoft.com/download/symbols

for caching symbols locally use the following syntax:


srv*c:\cache*http://msdl.microsoft.com/download/symbols

