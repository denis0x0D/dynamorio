/* **********************************************************
 * Copyright (c) 2015-2017 Google, Inc.  All rights reserved.
 * **********************************************************/

/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Google, Inc. nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL VMWARE, INC. OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#include <iostream>
#include <iomanip>
#include "cache_stats.h"

cache_stats_t::cache_stats_t(const std::string &miss_file, bool warmup_enabled,
                             bool is_coherent)
    : caching_device_stats_t(miss_file, warmup_enabled, is_coherent)
    , num_flushes(0)
    , num_prefetch_hits(0)
    , num_prefetch_misses(0)
{
}

void
cache_stats_t::access(const memref_t &memref, bool hit,
                      caching_device_block_t *cache_block)
{
    // handle prefetching requests
    if (type_is_prefetch(memref.data.type)) {
        if (hit)
            num_prefetch_hits++;
        else {
            num_prefetch_misses++;
            if (dump_misses && memref.data.type != TRACE_TYPE_HARDWARE_PREFETCH)
                dump_miss(memref);
        }
    } else { // handle regular memory accesses
        caching_device_stats_t::access(memref, hit, cache_block);
    }
}

void
cache_stats_t::flush(const memref_t &memref)
{
    num_flushes++;
}

void
cache_stats_t::print_counts(std::string prefix)
{
    caching_device_stats_t::print_counts(prefix);
    if (num_flushes != 0) {
        std::cerr << prefix << std::setw(18) << std::left << "Flushes:" << std::setw(20)
                  << std::right << num_flushes << std::endl;
    }
    if (num_prefetch_hits + num_prefetch_misses != 0) {
        std::cerr << prefix << std::setw(18) << std::left
                  << "Prefetch hits:" << std::setw(20) << std::right << num_prefetch_hits
                  << std::endl;
        std::cerr << prefix << std::setw(18) << std::left
                  << "Prefetch misses:" << std::setw(20) << std::right
                  << num_prefetch_misses << std::endl;
    }
}

void
cache_stats_t::reset()
{
    caching_device_stats_t::reset();
    num_flushes = 0;
    num_prefetch_hits = 0;
    num_prefetch_misses = 0;
}
