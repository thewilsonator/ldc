//===-- driver/timetrace.cpp ------------------------------------*- C++ -*-===//
//
//                         LDC – the LLVM D compiler
//
// This file is distributed under the BSD-style LDC license. See the LICENSE
// file for details.
//
//===----------------------------------------------------------------------===//
//
// Compilation time tracing support implementation, --ftime-trace.
//
//===----------------------------------------------------------------------===//

#include "driver/timetrace.h"

#if LDC_WITH_TIMETRACER

#include "dmd/errors.h"
#include "driver/cl_options.h"
#include "llvm/Support/TimeProfiler.h"
#if LDC_LLVM_VER >= 1300
#include "llvm/Support/FileSystem.h"
#endif

void initializeTimeTracer() {
  if (opts::fTimeTrace) {
    llvm::timeTraceProfilerInitialize(opts::fTimeTraceGranularity,
                                      opts::allArguments[0]);
  }
}

void deinitializeTimeTracer() {
  if (opts::fTimeTrace) {
    llvm::timeTraceProfilerCleanup();
  }
}

void writeTimeTraceProfile() {
  if (llvm::timeTraceProfilerEnabled()) {
    std::string filename = opts::fTimeTraceFile;
    if (filename.empty()) {
      filename = global.params.objfiles[0] ? global.params.objfiles[0] : "out";
      filename += ".time-trace";
    }

    std::error_code err;
    llvm::raw_fd_ostream outputstream(filename, err,
#if LDC_LLVM_VER >= 900
                                      llvm::sys::fs::OF_Text
#else
                                      llvm::sys::fs::F_Text
#endif
                                      );
    if (err) {
      error(Loc(), "Error writing Time Trace profile: could not open %s",
            filename.c_str());
      return;
    }

    llvm::timeTraceProfilerWrite(outputstream);
  }
}

void timeTraceProfilerBegin(size_t name_length, const char *name_ptr,
                            size_t detail_length, const char *detail_ptr) {
  llvm::timeTraceProfilerBegin(llvm::StringRef(name_ptr, name_length),
                               llvm::StringRef(detail_ptr, detail_length));
}

#endif
