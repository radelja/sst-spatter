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


#ifndef _H_SST_MIRANDA_SPATTER_BENCH_GEN
#define _H_SST_MIRANDA_SPATTER_BENCH_GEN

#include <sst/elements/sstSpatter/mirandaGenerator.h>
#include <sst/core/output.h>

#include <queue>

#include <Spatter/Input.hh>

namespace SST {
namespace SST_Spatter {

class SpatterBenchGenerator : public RequestGenerator {

public:
    SpatterBenchGenerator( ComponentId_t id, Params& params );
    void build(Params& params);
    ~SpatterBenchGenerator();
    void generate(MirandaRequestQueue<GeneratorRequest*>* q);
    bool isFinished();
    void completed();

    SST_ELI_REGISTER_SUBCOMPONENT(
        SpatterBenchGenerator,
        "sstSpatter",
        "SpatterBenchGenerator",
        SST_ELI_ELEMENT_VERSION(1,0,0),
        "Creates a stream of gather/scatter operations based on a Spatter pattern",
        SST::SST_Spatter::RequestGenerator
    )

    SST_ELI_DOCUMENT_PARAMS(
        { "verbose",             "Sets the verbosity of the output", "0" },
        { "args",                "Sets the arguments to describe Spatter pattern(s)", "" },
        { "datawidth",           "Sets the width of the memory operation", "8" },
        { "start_source",        "Sets the start address of the source array", "0" },
        { "start_target",        "Sets the start address of the target array", "0" }
    )

    SST_ELI_DOCUMENT_STATISTICS(
        { "split_read_reqs",     "Number of read requests split over a cache line boundary", "requests", 2 },
        { "split_write_reqs",    "Number of write requests split over a cache line boundary", "requests", 2 },
        { "split_custom_reqs",   "NUmber of custom requests split over a cache line boundary", "requests", 2 },
        { "read_reqs",           "Number of read requests issued", "requests", 1 },
        { "write_reqs",          "Number of write requests issued", "requests", 1 },
        { "custom_reqs",         "Number of custom requests issued", "requests", 1 },
        { "completed_reqs",      "Number of completed requests", "requests", 2 },
        { "total_bytes_read",    "Count the total bytes requested by read operations", "bytes", 1 },
        { "total_bytes_write",   "Count the total bytes requested by write operations", "bytes", 1 },
        { "total_bytes_custom",  "Count the total bytes requested by custom operations", "bytes", 1 },
        { "req_latency",         "Running total of all latency for all requests", "ns", 2 },
        { "cycles_with_issue",   "Number of cycles which CPU was able to issue requests", "cycles", 1 },
        { "cycles_no_issue",     "Number of cycles which CPU was not able to issue requests", "cycles", 1 },
        { "time",                "Nanoseconds spent issuing requests", "ns", 1 },
        { "cycles_hit_fence",    "Number of issue cycles which stop issue at a fence", "cycles", 2 },
        { "cycles_max_reorder",  "Number of issue cycles which hit maximum reorder lookup",	"cycles", 2 },
        { "cycles_max_issue",    "Cycles with maximum operation issue", "cycles", 2 },
        { "cycles",              "Cycles executed", "cycles", 1 }
    )

private:
    void countArgs(const std::string &args, int32_t &argc);
    void tokenizeArgs(const std::string &args, const int32_t &argc, char ***argv);
    void setStatFlags(Statistic<uint64_t>* stat);
    void updateIndices();

    size_t getPatternSize(const Spatter::ConfigurationBase *config);

    void gather();
    void scatter();
    void gatherScatter();
    void multiGather();
    void multiScatter();

    uint32_t datawidth;
    uint32_t startSource;
    uint32_t startTarget;

    size_t patternIdx;
    size_t countIdx;
    size_t configIdx;

    bool configFin;

    Statistic<uint64_t>* statReqs[OPCOUNT];
    Statistic<uint64_t>* statSplitReqs[OPCOUNT];
    Statistic<uint64_t>* statCompletedReqs;
    Statistic<uint64_t>* statCyclesWithIssue;
    Statistic<uint64_t>* statMaxIssuePerCycle;
    Statistic<uint64_t>* statCyclesWithoutIssue;
    Statistic<uint64_t>* statBytes[OPCOUNT];
    Statistic<uint64_t>* statReqLatency;
    Statistic<uint64_t>* statTime;
    Statistic<uint64_t>* statCyclesHitFence;
    Statistic<uint64_t>* statCyclesHitReorderLimit;
    Statistic<uint64_t>* statCycles;

    MirandaRequestQueue<GeneratorRequest*>* queue;

    Output* out;

    Spatter::ClArgs cl;
};

}
}

#endif
