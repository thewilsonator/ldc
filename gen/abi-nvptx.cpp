//===-- gen/abi-nvptx.cpp ---------------------------------------*- C++ -*-===//
//
//                         LDC – the LLVM D compiler
//
// This file is distributed under the BSD-style LDC license. See the LICENSE
// file for details.
//
//===----------------------------------------------------------------------===//

#include "ddmd/id.h"
#include "gen/abi.h"
#include "gen/dcompute/druntime.h"
#include "gen/uda.h"
#include "ddmd/declaration.h"
#include "gen/tollvm.h"
#include "gen/dcompute/abi-rewrites.h"

struct NVPTXTargetABI : TargetABI {
  DComputePointerRewrite pointerRewite;
  llvm::CallingConv::ID callingConv(LINK l, TypeFunction *tf = nullptr,
                                    FuncDeclaration *fdecl = nullptr) override {
    assert(fdecl);
    if (hasKernelAttr(fdecl))
        return llvm::CallingConv::PTX_Kernel;
    else
        return llvm::CallingConv::PTX_Device;
  }
  bool passByVal(Type *t) override {
    t = t->toBasetype();
    return ((t->ty == Tsarray || t->ty == Tstruct) && t->size() > 64);
  }
  void rewriteFunctionType(TypeFunction *t, IrFuncTy &fty) override {
    for (auto arg : fty.args) {
      if (!arg->byref)
        rewriteArgument(fty, *arg);
    }
  }
  bool returnInArg(TypeFunction *tf) override {
    return !tf->isref && DtoIsInMemoryOnly(tf->next);
  }
  void rewriteArgument(IrFuncTy &fty, IrFuncTyArg &arg) override {
    Type *ty = arg.type->toBasetype();
    llvm::Optional<DcomputeAddrspacedType> ptr;
    if (ty->ty == Tstruct &&
        (ptr = toDcomputeAddrspacedType(static_cast<TypeStruct*>(ty)->sym)) &&
        ptr->id == Id::dcPointer)
    {
        arg.rewrite = &pointerRewite;
        arg.ltype = pointerRewite.type(arg.type);
    }
  }
  // There are no exceptions at all, so no need for unwind tables.
  bool needsUnwindTables() override {
    return false;
  }
};

TargetABI *createNVPTXABI() { return new NVPTXTargetABI(); }
