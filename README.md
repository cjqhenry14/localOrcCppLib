Modified orc lib based on https://github.com/apache/orc

##Building

-To compile:  
% export TZ=America/Los_Angeles  
% mkdir build  
% cd build  
% cmake ..  
% make  

---
##Exmaple of using the lib:  
c++/src/FileScan.cc reads an ORC file and counts the number of rows and stripes.  
c++/src/FileContents.cc expands the above functionality and print the contents of an ORC file in JSON format  
c++/src/FileStatistics.cc reads and displays column statistics at the file- and stripe-level granularity.  
c++/src/FileMemory.cc evaluates dynamic memory usage. It also shows how to read only specific columns from an ORC file.    

---  
##Current changes made by me
1) Core lib source code:  
c++/src/ColumnPrinter.cc:  
&emsp;&emsp;Main change: replace ColumnPrinter::printRow() with void ColumnPrinter::printRow(uint64_t rowId, char** tuple, unsigned int curColId), because I want to return one record as char** to fdw (which will be used in iterativeForeignScan() function). The parameter char** tuple (one line record) will be passed from fdw, and then filled with values parsed by orc lib.  

c++/include/orc/ColumnPrinter.hh  

2) test usage code:  
tools/src/FileContents.cc  

3) compile flags files:  
add -fPIC option to these files:  

     build/c++/libs/zlib-1.2.8/CMakeFiles/zlib.dir/flags.make
     build/c++/libs/snappy-1.1.2/CMakeFiles/snappy.dir/flags.make
     build/c++/libs/protobuf-2.6.0/CMakeFiles/protobuf.dir/flags.make
     build/c++/libs/gmock-1.7.0/CMakeFiles/gmock.dir/flags.make
     build/c++/src/CMakeFiles/orc.dir/flags.make
     
Then move them to /hdfs_orc_fdw/orcLib/  

---
##Steps to modify the core source code of lib for your own use:  
1) Modify with your own code.  

2) Build the lib, although maybe there will be errors (build wiil stop somewhere), doesn't matter if at least these 5 static lib files are generated:   libprotobuf.a,  libz.a, libgmock.a, libsnappy.a, liborc.a  
command:  
&emsp;cd build  
&emsp;&emsp;make  
 
3) move the updated .a (mainly liborc.a) files to hdfs_orc_fdw/orcLib/  

---
##Steps to modify the test usage code (eg. hdfsOrcCppLib/build/tools/src/FileContents.cc) for your own use:  
1) Modify with your own code.  

2) command:  
&emsp;cd build/tools/src/  
&emsp;&emsp;make  

3) run the code: ./file-contents /usr/pgsql-9.4/test_data1.orc
