//===- Test.cpp -----------------------------------------------------------===//
//
//                     The SkyPat Team
//
// This file is distributed under the New BSD License. 
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//
#include <skypat/skypat.h>
#include <skypat/Listeners/PrettyResultPrinter.h>
#include <skypat/Listeners/CSVResultPrinter.h>
#include <skypat/Support/Path.h>
#include <time.h>
#include <cassert>
#include <unistd.h>
#include <string>
#include <cstdlib>

using namespace skypat;

//===----------------------------------------------------------------------===//
// Helper Functions
//===----------------------------------------------------------------------===//
static inline void help(int pArgc, char* pArgv[])
{
  testing::Log::getOStream() << "Usage:\n"
                             << "\t" << pArgv[0] << " [options...]\n\n"
                             << "Options:\n"
                             << "\t-c [file]  toutput CSV to [file]\n"
                             << "\t-h         Show this help manual\n";
}

//===----------------------------------------------------------------------===//
// Test
//===----------------------------------------------------------------------===//
Test::~Test()
{
  // MUST KEEP THIS DESTRUCTOR
}

void Test::run()
{
  this->TestBody();
}

void Test::Initialize(int* pArgc, char* pArgv[])
{
  // Choose user's printer
  int opt;
  std::string csvFile;
  while ((opt = getopt(*pArgc, pArgv, "c:h")) != -1 ) {
    switch (opt) {
      case 'c':
        csvFile = optarg;
        break;
      case 'h':
      default:
        help(*pArgc, pArgv);
        return;
    }
  }

  if (!csvFile.empty()) {
    CSVResultPrinter* printer = new CSVResultPrinter();
    if (printer->open(csvFile)) {
      testing::UnitTest::self()->repeater().add(printer);
    }
    else {
      testing::Log::getOStream() << "Failed to open file `" << csvFile << "`\n";
      delete printer;
    }
  }
  else
    testing::UnitTest::self()->repeater().add(new PrettyResultPrinter());

  // Choose runnable tests
  Path progname(pArgv[0]);
  progname = progname.filename();

  if (!testing::UnitTest::self()->addRunCase(progname.native()))
    testing::UnitTest::self()->addAllRunCases();
}

void Test::RunAll()
{
  testing::UnitTest::self()->RunAll();
}

void Test::Sleep(int pMS)
{
  assert(pMS > 0 && "Cannot sleep zero milliseconds");

  struct timespec ts = { pMS / 1000, (pMS % 1000) * 1000 * 1000 };
  nanosleep(&ts, NULL);
}

