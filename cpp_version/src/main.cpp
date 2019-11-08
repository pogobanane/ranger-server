/*-------------------------------------------------------------------------------
 This file is part of ranger.

 Copyright (c) [2014-2018] [Marvin N. Wright]

 This software may be modified and distributed under the terms of the MIT license.

 Please note that the C++ core of ranger is distributed under MIT license and the
 R package "ranger" under GPL3 license.
 #-------------------------------------------------------------------------------*/

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <memory>
#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>

#include "globals.h"
#include "ArgumentHandler.h"
#include "ForestClassification.h"
#include "ForestRegression.h"
#include "ForestSurvival.h"
#include "ForestProbability.h"
#include "utility.h"

#include "ipc.h"

using namespace ranger;

void run_ranger(const ArgumentHandler& arg_handler, std::ostream& verbose_out) {
  verbose_out << "Starting Ranger." << std::endl;

  
  // Create forest object
  std::unique_ptr<Forest> forest { };
  switch (arg_handler.treetype) {
  case TREE_CLASSIFICATION:
    if (arg_handler.probability) {
      forest = make_unique<ForestProbability>();
    } else {
      forest = make_unique<ForestClassification>();
    }
    break;
  case TREE_REGRESSION:
    forest = make_unique<ForestRegression>();
    break;
  case TREE_SURVIVAL:
    forest = make_unique<ForestSurvival>();
    break;
  case TREE_PROBABILITY:
    forest = make_unique<ForestProbability>();
    break;
  }

  // Call Ranger
  forest->initCpp(arg_handler.depvarname, arg_handler.memmode, arg_handler.file, arg_handler.mtry,
      arg_handler.outprefix, arg_handler.ntree, &verbose_out, arg_handler.seed, arg_handler.nthreads,
      arg_handler.predict, arg_handler.impmeasure, arg_handler.targetpartitionsize, arg_handler.splitweights,
      arg_handler.alwayssplitvars, arg_handler.statusvarname, arg_handler.replace, arg_handler.catvars,
      arg_handler.savemem, arg_handler.splitrule, arg_handler.caseweights, arg_handler.predall, arg_handler.fraction,
      arg_handler.alpha, arg_handler.minprop, arg_handler.holdout, arg_handler.predictiontype,
      arg_handler.randomsplits, arg_handler.maxdepth);

  forest->run(true, !arg_handler.skipoob);
  if (arg_handler.write) {
    forest->saveToFile();
  }
  forest->writeOutput();
  verbose_out << "Finished Ranger." << std::endl;
}

std::string ranger_predict() {
  std::ostream& verbose_out = std::cout;
  verbose_out << "Starting Ranger." << std::endl;
  
  // ranger parameters
  TreeType treetype = TREE_CLASSIFICATION;
  bool probability = false;
  std::string depvarname = "";
  MemoryMode memmode = MEM_DOUBLE;
  std::string file = "data2.dat";
  uint mtry = 0;
  std::string outprefix = "data2-ipced";
  uint ntree = DEFAULT_NUM_TREE;
  uint seed = 0;
  uint nthreads = 1; // DEFAULT_NUM_THREADS;
  std::string predict = "data2.forest"; 
  ImportanceMode impmeasure = DEFAULT_IMPORTANCE_MODE;
  uint targetpartitionsize = 0;
  std::string splitweights = "";
  std::vector<std::string> alwayssplitvars;
  std::string statusvarname = "";
  bool replace = true;
  std::vector<std::string> catvars;
  bool savemem = false;
  SplitRule splitrule = DEFAULT_SPLITRULE;
  std::string caseweights = "";
  bool predall = false;
  double fraction = 1;
  double alpha = DEFAULT_ALPHA;
  double minprop = DEFAULT_MINPROP;
  bool holdout = false;
  PredictionType predictiontype = DEFAULT_PREDICTIONTYPE;
  uint randomsplits = DEFAULT_NUM_RANDOM_SPLITS;
  uint maxdepth = DEFAULT_MAXDEPTH;
  bool skipoob = false;
  bool write = false;

  // Create forest object
  std::unique_ptr<Forest> forest { };
  switch (treetype) {
  case TREE_CLASSIFICATION:
    if (probability) {
      forest = make_unique<ForestProbability>();
    } else {
      forest = make_unique<ForestClassification>();
    }
    break;
  case TREE_REGRESSION:
    forest = make_unique<ForestRegression>();
    break;
  case TREE_SURVIVAL:
    forest = make_unique<ForestSurvival>();
    break;
  case TREE_PROBABILITY:
    forest = make_unique<ForestProbability>();
    break;
  }

  // Call Ranger
  forest->initCpp(depvarname, memmode, file, mtry,
      outprefix, ntree, &verbose_out, seed, nthreads,
      predict, impmeasure, targetpartitionsize, splitweights,
      alwayssplitvars, statusvarname, replace, catvars,
      savemem, splitrule, caseweights, predall, fraction,
      alpha, minprop, holdout, predictiontype,
      randomsplits, maxdepth);

  forest->run(true, !skipoob);
  if (write) {
    forest->saveToFile();
  }
  forest->writeOutput();
  verbose_out << "Finished Ranger." << std::endl;

  return "return str";
}

/*
* remember to adjust the SAMPLE_RINGBUF_SIZE to 8 and SAMPLE_RINGBUF_MAP to 7 before using this test
*/
#include "ringbuf-test.h"
int tests() {
  int ret = ringbuf_test();
  std::cout << std::unitbuf << ret << "\n";
  return ret;
}

int main(int argc, char **argv) {
  sample_ipc_main_t ipc;
  sample_ipc_open(&ipc); // TODO error check
  int n = 10;

  //sample_ringbuffer_t *requests = malloc(sizeof(sample_ringbuffer_t) * n);
  //if (requests = NULL) {
  //  std::cout << std::unitbuf << "out of memory?\n"
  //  return -1;
  //}
  std::vector<sample_ringbuffer_t> requests;
  std::unique_ptr<sample_ringbuffer_t> request = make_unique<sample_ringbuffer_t>();
  
  for (int i = 0; i < n; i++) {
    std::cout << std::unitbuf << "exporting sample\n";
    //sample_ringbuffer_t *bucket = (sample_ringbuffer_t *) malloc(sizeof(sample_ringbuffer_t));
    sample_ipc_communicate_to_client(&ipc, i, request.get());
    requests.push_back(*request);
    //std::cout << std::unitbuf << ".";
    usleep(3000000); // 3s
  }
  std::cout << std::unitbuf << "Recording stopped.\n";
  //std::cout << std::nounitbuf;

  // List n_rx_packets starting with the oldest one
  const char *filepath = "badge_sizes.csv";
  std::ofstream outfile;
  outfile.open(filepath);
  uint32_t n_rx_packets = 0;
  for (sample_ringbuffer_t &rbuf : requests) {
    for (uint32_t i = 0; i < rbuf.sizeOfBuffer; i++) {
      if (!sample_ringbuf_pop(&rbuf, &n_rx_packets)) {
        break;
      }
      outfile << n_rx_packets << "\n";
    }
  }
  outfile.close();
  std::cout << "Wrote data to " << filepath << "\n";

  std::string prediction;
  
  try {
    prediction = ranger_predict();
  } catch (std::runtime_error& e) {
    std::cerr << "Error: " << e.what() << " Ranger will EXIT now." << std::endl;
    sample_ipc_close(&ipc);
    return -1;
  }
  
  sample_ipc_close(&ipc); // TODO error check
  return 0;
}

int main_old(int argc, char **argv) {

  sample_ipc_main_t ipc;
  sample_ipc_open(&ipc);
  try {
    // Handle command line arguments
    ArgumentHandler arg_handler(argc, argv);
    if (arg_handler.processArguments() != 0) {
      return 0;
    }
    arg_handler.checkArguments();

    if (arg_handler.verbose) {
      run_ranger(arg_handler, std::cout);
    } else {
      std::ofstream logfile { arg_handler.outprefix + ".log" };
      if (!logfile.good()) {
        throw std::runtime_error("Could not write to logfile.");
      }
      run_ranger(arg_handler, logfile);
    }
  } catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << " Ranger will EXIT now." << std::endl;
    sample_ipc_close(&ipc);
    return -1;
  }

  sample_ipc_close(&ipc);
  return 0;
}
