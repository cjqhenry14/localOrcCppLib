/**
 *  * Licensed to the Apache Software Foundation (ASF) under one
 *   * or more contributor license agreements.  See the NOTICE file
 *    * distributed with this work for additional information
 *     * regarding copyright ownership.  The ASF licenses this file
 *      * to you under the Apache License, Version 2.0 (the
 *       * "License"); you may not use this file except in compliance
 *        * with the License.  You may obtain a copy of the License at
 *         *
 *          *     http://www.apache.org/licenses/LICENSE-2.0
 *           *
 *            * Unless required by applicable law or agreed to in writing, software
 *             * distributed under the License is distributed on an "AS IS" BASIS,
 *              * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *               * See the License for the specific language governing permissions and
 *                * limitations under the License.
 *                 */

#include "orc/ColumnPrinter.hh"

#include <memory>
#include <string>
#include <iostream>
#include <string>
#define UNUSED(x) (void)x

#ifdef __GNUC__

//#pragma GCC diagnostic ignored "-Wmissing-variable-declarations"
//#pragma GCC diagnostic ignored "-Wexit-time-destructors"
//#pragma GCC diagnostic ignored "-Wglobal-constructors"


#endif

unsigned int colNum = 0;
unsigned int maxRowPerBatch = 1000;
std::string line;
unsigned long curRow = 0;
/*char fakefilename[100] ="/Users/chenjiaqi/524/orcFileCppLib/build/tools/src/city.orc";*/
char fakefilename[50] = "/usr/pgsql-9.4/city.orc";

orc::ReaderOptions opts;
/*init first, avoid init with abstract obj. Then change in initOrcReader()*/
std::unique_ptr<orc::Reader> reader = orc::createReader(orc::readLocalFile(std::string(fakefilename)), opts);
std::unique_ptr<orc::ColumnVectorBatch> batch = reader->createRowBatch(maxRowPerBatch);
std::unique_ptr<orc::ColumnPrinter> printer = createColumnPrinter(line, reader->getType());

/*allocate memory space, and init with NULL ptr*/
void initTuple(char** tuple) {
  tuple = new char*[colNum];
  for (unsigned int i=0; i<colNum; i++)
  {
    tuple[i] = NULL;
  }
}

void deleteTuple(char** tuple) {
  for (unsigned int i=0; i<colNum; i++)
  {
    if(tuple[i]!=NULL) {
      delete[] tuple[i];
    }
  }
  delete [] tuple;
}

void clearTuple(char** tuple) {
  for (unsigned int i=0; i<colNum; i++)
  {
    if(tuple[i] != NULL) {
      delete [] tuple[i];
      tuple[i] = NULL;
    }
  }
}

void printNextTuple(char** nextTuple) {
  for (unsigned int i=0; i<colNum; i++)
  {
    if(nextTuple[i]!=NULL) {
      printf("%s,  ", nextTuple[i]);
    }
  }
  printf("\n");
}


/*For fdw: init tuple memory, and other global var, should be used in BeginForeignScan() */
void initOrcReader( const char* filename, unsigned int fdwColNum, unsigned int fdwMaxRowPerBatch) {
  colNum = fdwColNum;
  curRow = 0;
  maxRowPerBatch = fdwMaxRowPerBatch;

  reader = orc::createReader(orc::readLocalFile(std::string(filename)), opts);
  batch = reader->createRowBatch(maxRowPerBatch);
  printer = createColumnPrinter(line, reader->getType());

  reader->next(*batch);
  printer->reset(*batch);
}

/*For fdw: iteratively get one line record, for using
 *  * return: NULL means no next record.
 *   * */
bool getNextOrcTuple(char ** tuple) {
  clearTuple(tuple);

  if(batch->numElements == 0)
    return false;

  if(curRow == batch->numElements) {
    if (reader->next(*batch)) {

      printer->reset(*batch);

      if(batch->numElements == 0)
        return false;

      curRow = 0;
      /* my modified printRow(int rowId, char** tuple, int curColId) */
      printer->printRow(curRow, tuple, 0);
      curRow++;

      return true;
    }
    else
      return false;
  }
    /*curRow < batch->numElements*/
  else {
    printer->printRow(curRow, tuple, 0);
    curRow++;
    return true;
  }
}

/*For fdw: release tuple memory, should be used in EndForeignScan() */
void releaseOrcBridgeMem(char **tuple) {
  deleteTuple(tuple);
}

int main(int argc, char* argv[]) {
  UNUSED(argc);
  initOrcReader(argv[1], 5, 1000);

  char** nextTuple = new char*[colNum];
  for (unsigned int i=0; i<colNum; i++)
  {
    nextTuple[i] = NULL;
  }

  while(getNextOrcTuple(nextTuple)) {

      printNextTuple(nextTuple);

  }
  releaseOrcBridgeMem(nextTuple);

  return 0;
}



