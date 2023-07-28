#include "customchar/character/character.h"
#include "customchar/common/common.h"
#include "customchar/common/helpers.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <regex>
#include <string>
#include <thread>
#include <vector>

using namespace CC;

int main(int argc, char** argv) {
  // Parse command line arguments
  CCParams params;
  if (CCParamsParse(argc, argv, params) == false) {
    exit(1);
  }
  if (whisper_lang_id(params.language.c_str()) == -1) {
    fprintf(stderr, "error: unknown language '%s'\n", params.language.c_str());
    CCPrintUsage(argc, argv, params);
    exit(1);
  }

  // Create character
  Character character(params);

  // Run character
  character.Run();

  return 0;
}
