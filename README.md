Modified orc lib based on https://github.com/apache/orc

##Building

-To compile:  
% export TZ=America/Los_Angeles  
% mkdir build  
% cd build  
% cmake ..  
% make  

---
##Current changes made by me
1) Core lib source code:  
c++/src/ColumnPrinter.cc:  
&emsp;&emsp;Main change: replace by void StringColumnPrinter::printRow(uint64_t rowId, char** tuple, unsigned int curColId)  
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
     mv …

---
##Steps to modify the core source code of lib for your own use:  
1) Modify with your own code.  

2) Build the lib, although maybe there will be errors (build wiil stop somewhere), but at least should ensure these 5 static lib files are generated:   libprotobuf.a,  libz.a, libgmock.a, libsnappy.a, liborc.a  
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
