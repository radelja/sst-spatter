import sst
import sys
import argparse

# Parse commandline arguments
parser = argparse.ArgumentParser()
parser.add_argument("--statfile", help="statistics file", default="./stats.csv")
parser.add_argument("--statlevel", help="statistics level", type=int, default=16)

args, unknown = parser.parse_known_args()

statFile = args.statfile
statLevel = args.statlevel

# Define SST core options
sst.setProgramOption("timebase", "1ps")

# Define the simulation components
comp_cpu = sst.Component("cpu", "sstSpatter.BaseCPU")
comp_cpu.addParams({
    "verbose" : 1,
    "printStats" : 1
})

gen = comp_cpu.setSubComponent("generator", "sstSpatter.SpatterGenerator")
gen.addParams({
    "verbose" : 2,
    "args" : " ".join(unknown)
})

comp_l1cache = sst.Component("l1cache", "memHierarchy.Cache")
comp_l1cache.addParams({
      "access_latency_cycles" : "2",
      "cache_frequency" : "2 Ghz",
      "coherence_protocol" : "MESI",
      "associativity" : "4",
      "cache_line_size" : "64",
      "debug" : "0",
      "L1" : "1",
      "cache_size" : "32KB"
})
comp_l1cache.setSubComponent("prefetcher", "cassini.StridePrefetcher")
comp_l1cache.setSubComponent("replacement", "memHierarchy.replacement.lru")

comp_memctrl = sst.Component("memory", "memHierarchy.MemController")
comp_memctrl.addParams({
      "clock" : "1GHz",
      "addr_range_end" : 4096 * 1024 * 1024 - 1
})
memory = comp_memctrl.setSubComponent("backend", "memHierarchy.simpleMem")
memory.addParams({
      "access_time" : "100 ns",
      "mem_size" : "4096MiB",
})

# Define the simulation links
link_cpu_cache_link = sst.Link("link_cpu_cache_link")
link_cpu_cache_link.connect( (comp_cpu, "cache_link", "1000ps"), (comp_l1cache, "high_network_0", "1000ps") )
link_cpu_cache_link.setNoCut()

link_mem_bus_link = sst.Link("link_mem_bus_link")
link_mem_bus_link.connect( (comp_l1cache, "low_network_0", "50ps"), (comp_memctrl, "direct_link", "50ps") )

# Enable statistics
sst.setStatisticLoadLevel(statLevel)
sst.setStatisticOutput("sst.statOutputCSV", {"filepath": statFile})
sst.enableAllStatisticsForAllComponents({"type":"sst.AccumulatorStatistic"})
