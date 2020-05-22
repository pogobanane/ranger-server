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
#include <string.h>
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

int ranger_predict(std::vector<uint32_t> data) {
  // predict a .forest trained by the following:
  // ./ranger ranger --treetype=3 --file=data2.dat --write --outprefix=data2 --depvarname="result"

  std::ostream& verbose_out = std::cout;
  verbose_out << "Starting Ranger." << std::endl;
  
  // ranger parameters
  TreeType treetype = TREE_REGRESSION;
  bool probability = false;
  std::string depvarname = "";
  MemoryMode memmode = MEM_DOUBLE;

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
  //bool write = false;

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
  forest->initCppFast(depvarname, memmode, data, mtry,
      outprefix, ntree, &verbose_out, seed, nthreads,
      predict, impmeasure, targetpartitionsize, splitweights,
      alwayssplitvars, statusvarname, replace, catvars,
      savemem, splitrule, caseweights, predall, fraction,
      alpha, minprop, holdout, predictiontype,
      randomsplits, maxdepth);

  forest->run(true, !skipoob);
  //if (write) {
    //forest->saveToFile();
  //}
  //forest->writeOutput();
  int ret = forest->getSinglePrediction();
  verbose_out << "Ranger Result: " << ret << std::endl;

  return ret;
}

// duration in seconds
// return exit code
int ipcdump(int duration, std::string outpath, uint32_t poll1, uint32_t udelay, uint32_t poll2, uint32_t usleep_time, uint32_t poll3, uint32_t use_interrupt, uint32_t poll4) {
  sample_ipc_main_t ipc;
  sample_ipc_open(&ipc); // TODO error check
  int sleeptime = 3; // in seconds
  int n = duration / sleeptime;

  //sample_ringbuffer_t *requests = malloc(sizeof(sample_ringbuffer_t) * n);
  //if (requests = NULL) {
  //  std::cout << std::unitbuf << "out of memory?\n"
  //  return -1;
  //}
  std::vector<sample_ringbuffer_t> requests;
  std::unique_ptr<sample_ringbuffer_t> request = make_unique<sample_ringbuffer_t>();
  sample_ipc_for_client_t response;
  response.poll1 = poll1;
  response.udelay = udelay;
  response.poll2 = poll2;
  response.usleep = usleep_time;
  response.poll3 = poll3;
  response.use_interrupt = use_interrupt;
  response.poll4 = poll4;
   
  for (int i = 0; i < n; i++) {
    std::cout << std::unitbuf << "exporting sample: ";
    //sample_ringbuffer_t *bucket = (sample_ringbuffer_t *) malloc(sizeof(sample_ringbuffer_t));
    sample_ipc_communicate_to_client(&ipc, &response, request.get());
    requests.push_back(*request);
    std::cout << std::unitbuf << sample_ringbuf_count(request.get()) << " / " << SAMPLE_RINGBUF_SIZE << "\n";
    //std::cout << std::unitbuf << ".";
    usleep(sleeptime * 1000000); // n sec
  }
  std::cout << std::unitbuf << "Recording stopped.\n";
  //std::cout << std::nounitbuf;

  // List n_rx_packets starting with the oldest one
  const char *filepath = outpath.c_str();
  std::ofstream outfile;
  outfile.open(filepath);
  sample_ringbuffer_data_t data;
  for (sample_ringbuffer_t &rbuf : requests) {
    for (uint32_t i = 0; i < rbuf.sizeOfBuffer; i++) {
      if (!sample_ringbuf_pop(&rbuf, &data)) {
        break;
      }
      outfile << data.port_id << "," << data.queue_id << "," << data.n_rx_packets << "\n";
    }
  }
  outfile.close();
  std::cout << "Wrote data to " << filepath << "\n";

  sample_ipc_close(&ipc); 
  return 0;
}

/*
 * 1. get info from client
 * 2. do prediction with info for port 0 queue 0
 * 3. send response to client
 *
 * return exit code
 */
int doai() {
  sample_ipc_main_t ipc;
  sample_ipc_open(&ipc);
  std::unique_ptr<sample_ringbuffer_t> request = make_unique<sample_ringbuffer_t>();
  std::vector<uint32_t> values = {};

  sample_ipc_for_client_t response;
  response.poll1 = 32;
  response.udelay = 0;
  response.poll2 = 0;
  response.usleep = 1;
  response.poll3 = 32;
  response.use_interrupt = 0;
  response.poll4 = 0;
   
  //while(true) {
    sample_ipc_communicate_to_client(&ipc, &response, request.get());

    // read as many datapoints as we need
    // TODO: always keep the 100 latest datapoints and use them
    int i = 0;
    while (i < 100) {
      sample_ringbuffer_data_t d;
      if (!sample_ringbuf_pop(request.get(), &d)) {
        std::cout << "Not enough datapoints received!\n";
        sample_ipc_close(&ipc);
        return 1;
      }
      if (d.port_id == 0 && d.queue_id == 0) {
        values.push_back(d.n_rx_packets);
        i++;
      }
    }

    // run prediction
    try {
      response.usleep = ranger_predict(values);
    } catch (std::runtime_error& e) {
      std::cerr << "Error: " << e.what() << " Ranger will EXIT now." << std::endl;
      sample_ipc_close(&ipc);
      return -1;
    }
  //}
  
  // communicate last prediction to client
  sample_ipc_communicate_to_client(&ipc, &response, request.get());

  sample_ipc_close(&ipc);
  return 0;
}

int respond(uint32_t poll1, uint32_t udelay, uint32_t poll2, uint32_t usleep, uint32_t poll3, uint32_t use_interrupt, uint32_t poll4) {
  sample_ipc_main_t ipc;
  sample_ipc_open(&ipc); // TODO error check
  std::vector<sample_ringbuffer_t> requests;
  std::unique_ptr<sample_ringbuffer_t> request = make_unique<sample_ringbuffer_t>();

  sample_ipc_for_client_t response;
  response.poll1 = poll1;
  response.udelay = udelay;
  response.poll2 = poll2;
  response.usleep = usleep;
  response.poll3 = poll3;
  response.use_interrupt = use_interrupt;
  response.poll4 = poll4;
  
  sample_ipc_communicate_to_client(&ipc, &response, request.get());
  requests.push_back(*request);

  sample_ipc_close(&ipc); 
  return 0;
}

// original ranger main
int main_old(int argc, char **argv) {

  //sample_ipc_main_t ipc;
  //sample_ipc_open(&ipc);
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
    //sample_ipc_close(&ipc);
    return -1;
  }

  //sample_ipc_close(&ipc);
  return 0;
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

  //return tests();

  int dump_seconds = 10;
  std::string dump_path = "outfile.csv";

  if ( argc >= 2) {
    if ( argc == 4 && strcmp(argv[1], "ipcdump") == 0) {
      dump_seconds = std::stoi(argv[2]);
      dump_path = argv[3];
      return ipcdump(dump_seconds, dump_path, 32, 0, 0, 0, 0, 0, 0);
    }
    if ( argc == 11 && strcmp(argv[1], "ipcdump") == 0) {
      return ipcdump(
          std::stoi(argv[2]),
          argv[3],
          std::stoi(argv[4]),
          std::stoi(argv[5]),
          std::stoi(argv[6]),
          std::stoi(argv[7]),
          std::stoi(argv[8]),
          std::stoi(argv[9]),
          std::stoi(argv[10])
          );
    }
    if ( strcmp(argv[1], "doai") == 0) {
      return doai();
    }
    if ( strcmp(argv[1], "respond") == 0) {
      return respond(
          std::stoi(argv[2]),
          std::stoi(argv[3]),
          std::stoi(argv[4]),
          std::stoi(argv[5]),
          std::stoi(argv[6]),
          std::stoi(argv[7]),
          std::stoi(argv[8])
          );
    }
    if ( strcmp(argv[1], "ranger") == 0) {
      return main_old(argc, argv);
    }
    if ( strcmp(argv[1], "predict") == 0) {
      std::vector<uint32_t> data = {};
      for (int i = 0; i < 100; i++) { data.push_back(i); }
      ranger_predict(data);
      return 0;
    }
  }
  std::cout << "Invalid arguments. Showing valid arguments:\n";
  std::cout << "\n";
  std::cout << "ipcdump <seconds> <outfile> [<poll1> <udelay> <poll2> <usleep> <poll3> <use_interrupt> <poll4>]\n";
  std::cout << "                                             dump ipc for approx. seconds\n";
  std::cout << "doai                                         doing ai stuff\n";
  std::cout << "respond <poll1> <udelay> <poll2> <usleep> <poll3> <use_interrupt> <poll4>\n";
  std::cout << "                                             send response\n";
  std::cout << "ranger [args...]                             run original ranger with args...\n";
  std::cout << "predict                                      manual prediction\n";

  return 1;
}

