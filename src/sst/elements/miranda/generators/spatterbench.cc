// Copyright 2009-2023 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2023, NTESS
// All rights reserved.
//
// Portions are copyright of other developers:
// See the file CONTRIBUTORS.TXT in the top level directory
// of the distribution for more information.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#include <sst_config.h>
#include <sst/elements/miranda/generators/spatterbench.h>

#include <sst/core/params.h>

using namespace SST::Miranda;


SpatterBenchGenerator::SpatterBenchGenerator(ComponentId_t id, Params& params) : RequestGenerator(id, params)
{
    build(params);
}

void SpatterBenchGenerator::build(Params& params)
{
    const uint32_t verbose = params.find<uint32_t>("verbose", 0);
    const std::string args = "./Spatter " + params.find<std::string>("args", "");

    char **argv = nullptr;
    int argc = 0;
    int res  = 0;

    out = new Output("SpatterBenchGenerator[@p:@l]: ", verbose, 0, Output::STDOUT);

    datawidth  = params.find<uint32_t>("datawidth", 8);

    patternIdx = 0;
    countIdx   = 0;
    configIdx  = 0;

    configFin = false;

    statReqs[READ]            = registerStatistic<uint64_t>("read_reqs");
    statReqs[WRITE]           = registerStatistic<uint64_t>("write_reqs");
    statReqs[CUSTOM]          = registerStatistic<uint64_t>("custom_reqs");
    statSplitReqs[READ]       = registerStatistic<uint64_t>("split_read_reqs");
    statSplitReqs[WRITE]      = registerStatistic<uint64_t>("split_write_reqs");
    statSplitReqs[CUSTOM]     = registerStatistic<uint64_t>("split_custom_reqs");
    statCyclesWithIssue       = registerStatistic<uint64_t>("cycles_with_issue");
    statCyclesWithoutIssue    = registerStatistic<uint64_t>("cycles_no_issue");
    statBytes[READ]           = registerStatistic<uint64_t>("total_bytes_read");
    statBytes[WRITE]          = registerStatistic<uint64_t>("total_bytes_write");
    statBytes[CUSTOM]         = registerStatistic<uint64_t>("total_bytes_custom");
    statReqLatency            = registerStatistic<uint64_t>("req_latency");
    statTime                  = registerStatistic<uint64_t>("time");
    statCyclesHitFence        = registerStatistic<uint64_t>("cycles_hit_fence");
    statMaxIssuePerCycle      = registerStatistic<uint64_t>("cycles_max_issue");
    statCyclesHitReorderLimit = registerStatistic<uint64_t>("cycles_max_reorder");
    statCycles                = registerStatistic<uint64_t>("cycles");

    statReqs[READ]->setFlagClearDataOnOutput(true);
    statReqs[WRITE]->setFlagClearDataOnOutput(true);
    statReqs[CUSTOM]->setFlagClearDataOnOutput(true);
    statSplitReqs[READ]->setFlagClearDataOnOutput(true);
    statSplitReqs[WRITE]->setFlagClearDataOnOutput(true);
    statSplitReqs[CUSTOM]->setFlagClearDataOnOutput(true);
    statCyclesWithIssue->setFlagClearDataOnOutput(true);
    statCyclesWithoutIssue->setFlagClearDataOnOutput(true);
    statBytes[READ]->setFlagClearDataOnOutput(true);
    statBytes[WRITE]->setFlagClearDataOnOutput(true);
    statBytes[CUSTOM]->setFlagClearDataOnOutput(true);
    statReqLatency->setFlagClearDataOnOutput(true);
    statTime->setFlagClearDataOnOutput(true);
    statCyclesHitFence->setFlagClearDataOnOutput(true);
    statMaxIssuePerCycle->setFlagClearDataOnOutput(true);
    statCyclesHitReorderLimit->setFlagClearDataOnOutput(true);
    statCycles->setFlagClearDataOnOutput(true);

    statReqs[READ]->setFlagOutputAtEndOfSim(false);
    statReqs[WRITE]->setFlagOutputAtEndOfSim(false);
    statReqs[CUSTOM]->setFlagOutputAtEndOfSim(false);
    statSplitReqs[READ]->setFlagOutputAtEndOfSim(false);
    statSplitReqs[WRITE]->setFlagOutputAtEndOfSim(false);
    statSplitReqs[CUSTOM]->setFlagOutputAtEndOfSim(false);
    statCyclesWithIssue->setFlagOutputAtEndOfSim(false);
    statCyclesWithoutIssue->setFlagOutputAtEndOfSim(false);
    statBytes[READ]->setFlagOutputAtEndOfSim(false);
    statBytes[WRITE]->setFlagOutputAtEndOfSim(false);
    statBytes[CUSTOM]->setFlagOutputAtEndOfSim(false);
    statReqLatency->setFlagOutputAtEndOfSim(false);
    statTime->setFlagOutputAtEndOfSim(false);
    statCyclesHitFence->setFlagOutputAtEndOfSim(false);
    statMaxIssuePerCycle->setFlagOutputAtEndOfSim(false);
    statCyclesHitReorderLimit->setFlagOutputAtEndOfSim(false);
    statCycles->setFlagOutputAtEndOfSim(false);

    // Convert the arguments to a compatible format before parsing them.
    countArgs(args, argc);
    tokenizeArgs(args, argc, &argv);

    res = Spatter::parse_input(argc, argv, cl);


    // The allocated memory is no longer needed.
    for (int i = 0; i < argc; ++i) {
        delete [] argv[i];
    }
    delete [] argv;


    if (0 != res) {
        out->fatal(CALL_INFO, -1, "Error: failed to parse provided arguments.\n");
    }

    startSource = params.find<uint32_t>("start_source", 0);
    startTarget = params.find<uint32_t>("start_target", std::max(cl.sparse_size, cl.sparse_gather_size));

    if (startTarget > startSource) {
        if (startTarget <= (startSource + std::max(cl.sparse_size, cl.sparse_gather_size) - 1)) {
            out->verbose(CALL_INFO, 0, 0, "Warning: source and target arrays will overlap.\n");
        }
    } else if (startSource < startTarget){
        if (startSource <= (startTarget + std::max(cl.dense_size, cl.sparse_scatter_size) - 1)) {
            out->verbose(CALL_INFO, 0, 0, "Warning: source and target arrays will overlap.\n");
        }
    } else {
        out->verbose(CALL_INFO, 0, 0, "Warning: source and target arrays will overlap.\n");
    }
}

SpatterBenchGenerator::~SpatterBenchGenerator()
{
    delete out;
}

void SpatterBenchGenerator::generate(MirandaRequestQueue<GeneratorRequest*>* q)
{
    if (!configFin) {
        const Spatter::ConfigurationBase *config = cl.configs[configIdx].get();
        queue = q;

        if (0 == config->kernel.compare("gather")) {
            gather();
        } else if (0 == config->kernel.compare("scatter")) {
            scatter();
        } else if (0 == config->kernel.compare("gs")) {
            gatherScatter();
        } else if (0 == config->kernel.compare("multigather")) {
            multiGather();
        } else if (0 == config->kernel.compare("multiscatter")) {
            multiScatter();
        } else {
            out->fatal(CALL_INFO, -1, "Error: invalid kernel.\n");
        }

        updateIndices();
    }
}

bool SpatterBenchGenerator::isFinished()
{
    if (configFin) {
        const Spatter::ConfigurationBase *prevConfig = cl.configs[configIdx-1].get();
        uint64_t expectedBytes = getPatternSize(prevConfig) * prevConfig->count * datawidth;
        uint64_t recordedBytes = calcBytes(prevConfig);

        if (0 == prevConfig->kernel.compare("gs")) {
            // GS patterns expect twice the number of bytes.
            expectedBytes <<= 1;
        }

        // Check if the requests associated with the previous run-configuration have been executed.
        if (recordedBytes == expectedBytes) {
            // The requests associated with the previous run-configuration have finished executing.
            performGlobalStatisticOutput();
            configFin = false;
        }
    }

    return (configIdx == cl.configs.size());
}

void SpatterBenchGenerator::completed()
{
}

/**
   * @brief Counts the number of arguments in a string.
   *
   * @param args The string of arguments to be counted.
   * @param argc Number of arguments found in the string.
   */
void SpatterBenchGenerator::countArgs(const std::string &args, int32_t &argc)
{
    std::istringstream iss(args);
    std::string tok;

    while (iss >> tok) {
        ++argc;
    }
}

/**
   * @brief Tokenize a string of arguments into an array of arguments
            and allocates memory for the array of arguments.
   *
   * @param args String of arguments to be tokenized.
   * @param argc Number of arguments in the string.
   * @param argv Destination array for the arguments.
   */
void SpatterBenchGenerator::tokenizeArgs(const std::string &args, const int32_t &argc, char ***argv)
{
    std::istringstream iss(args);
    std::string tok;

    char **argvPtr = new char *[argc + 1];
    int argvIdx = 0;

    while (iss >> tok) {
        int arg_size = tok.size() + 1;

        argvPtr[argvIdx] = new char[arg_size];
        strncpy(argvPtr[argvIdx], tok.c_str(), arg_size);

        ++argvIdx;
    }
    argvPtr[argvIdx] = nullptr;

    *argv = argvPtr;
}

/**
   * @brief Calculate the number of bytes read or written by memory requests.
   *
   * @param config Run-configuration used to determine the kernel type.
   * @return Number of bytes read or written by memory requests.
   */
uint64_t SpatterBenchGenerator::calcBytes(const Spatter::ConfigurationBase *config)
{
    uint64_t numBytes = 0;

    if ((0 == config->kernel.compare("gather")) || (0 == config->kernel.compare("multigather"))) {
        numBytes = statBytes[READ]->getCollectionCount() * datawidth;
    } else if ((0 == config->kernel.compare("scatter")) || (0 == config->kernel.compare("multiscatter"))) {
        numBytes = statBytes[WRITE]->getCollectionCount() * datawidth;
    } else if (0 == config->kernel.compare("gs")) {
        numBytes = (statBytes[WRITE]->getCollectionCount() + statBytes[READ]->getCollectionCount()) * datawidth;
    }

    return numBytes;
}

/**
   * @brief Return the number of elements in the pattern.
   *
   * @param config Run-configuration used to determine the kernel type.
   * @return Number of elements in the pattern.
   */
size_t SpatterBenchGenerator::getPatternSize(const Spatter::ConfigurationBase *config)
{
    size_t patternSize = 0;

    if ((0 == config->kernel.compare("gather")) || (0 == config->kernel.compare("scatter"))) {
        patternSize = config->pattern.size();
    } else if ((0 == config->kernel.compare("gs"))) {
        assert(config->pattern_scatter.size() == config->pattern_gather.size());
        patternSize = config->pattern_scatter.size();
    } else if (0 == config->kernel.compare("multigather")) {
        patternSize = config->pattern_gather.size();
    } else if (0 == config->kernel.compare("multiscatter")) {
        patternSize = config->pattern_scatter.size();
    }

    return patternSize;
}

/**
   * @brief Update the pattern, count, and config indices.
   *
   */
void SpatterBenchGenerator::updateIndices()
{
    const Spatter::ConfigurationBase *config = cl.configs[configIdx].get();
    size_t patternSize = getPatternSize(config);

    if (patternIdx == (patternSize - 1)) {
        patternIdx = 0;
        if (countIdx == (config->count - 1)) {
            countIdx = 0;

            // Finished issuing requests for the current run-configuration.
            configFin = true;
            ++configIdx;
        } else {
            ++countIdx;
        }
    } else {
        ++patternIdx;
    }
}

/**
   * @brief Generate a memory request for a Gather pattern.
   *
   */
void SpatterBenchGenerator::gather()
{
    const Spatter::ConfigurationBase *config = cl.configs[configIdx].get();

    uint64_t sourceOffset = config->pattern[patternIdx] + config->delta * countIdx;
    uint64_t sourceAddr = startSource + sourceOffset;

    out->verbose(CALL_INFO, 8, 0, "Issuing READ request for address %" PRIu64 "\n", sourceAddr);
    queue->push_back(new MemoryOpRequest(sourceAddr, datawidth, READ));
}

/**
   * @brief Generate a memory request for a Scatter pattern.
   *
   */
void SpatterBenchGenerator::scatter()
{
    const Spatter::ConfigurationBase *config = cl.configs[configIdx].get();

    uint64_t targetOffset = config->pattern[patternIdx] + config->delta * countIdx;
    uint64_t targetAddr = startTarget + targetOffset;

    out->verbose(CALL_INFO, 8, 0, "Issuing WRITE request for address %" PRIu64 "\n", targetAddr);
    queue->push_back(new MemoryOpRequest(targetAddr, datawidth, WRITE));
}

/**
   * @brief Generate memory requests for a GS pattern.
   *
   */
void SpatterBenchGenerator::gatherScatter()
{
    const Spatter::ConfigurationBase *config = cl.configs[configIdx].get();

    uint64_t sourceOffset = config->pattern_gather[patternIdx] + config->delta_gather * countIdx;
    uint64_t sourceAddr = startSource + sourceOffset;

    uint64_t targetOffset = config->pattern_scatter[patternIdx] + config->delta_scatter * countIdx;
    uint64_t targetAddr = startTarget + targetOffset;

    MemoryOpRequest* readReq  = new MemoryOpRequest(sourceAddr, datawidth, READ);
    MemoryOpRequest* writeReq = new MemoryOpRequest(targetAddr, datawidth, WRITE);

    writeReq->addDependency(readReq->getRequestID());

    out->verbose(CALL_INFO, 8, 0, "Issuing READ request for address %" PRIu64 "\n", sourceAddr);
    queue->push_back(readReq);

    out->verbose(CALL_INFO, 8, 0, "Issuing WRITE request for address %" PRIu64 "\n", targetAddr);
    queue->push_back(writeReq);
}

/**
   * @brief Generate a memory request for a MultiGather pattern.
   *
   */
void SpatterBenchGenerator::multiGather()
{
    const Spatter::ConfigurationBase *config = cl.configs[configIdx].get();

    uint64_t sourceOffset = config->pattern[config->pattern_gather[patternIdx]] + config->delta * countIdx;
    uint64_t sourceAddr = startSource + sourceOffset;

    out->verbose(CALL_INFO, 8, 0, "Issuing READ request for address %" PRIu64 "\n", sourceAddr);
    queue->push_back(new MemoryOpRequest(sourceAddr, datawidth, READ));
}

/**
   * @brief Generate a memory request for a MultiScatter pattern.
   *
   */
void SpatterBenchGenerator::multiScatter()
{
    const Spatter::ConfigurationBase *config = cl.configs[configIdx].get();

    uint64_t targetOffset = config->pattern[config->pattern_scatter[patternIdx]] + config->delta * countIdx;
    uint64_t targetAddr = startTarget + targetOffset;

    out->verbose(CALL_INFO, 8, 0, "Issuing WRITE request for address %" PRIu64 "\n", targetAddr);
    queue->push_back(new MemoryOpRequest(targetAddr, datawidth, WRITE));
}
