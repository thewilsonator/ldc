//===-- gen/dcompute/druntime.cpp -----------------------------------------===//
//
//                         LDC – the LLVM D compiler
//
// This file is distributed under the BSD-style LDC license. See the LICENSE
// file for details.
//
//===----------------------------------------------------------------------===//

#include "gen/dcompute/druntime.h"
#include "ddmd/dsymbol.h"
#include "ddmd/module.h"
#include "ddmd/identifier.h"
#include "ddmd/template.h"
#include "ddmd/declaration.h"
#include "ddmd/aggregate.h"
#include "id.h"

bool isFromLDC_DCompute(Dsymbol *sym) {
  auto mod = sym->getModule();
  if (!mod)
    return false;
  auto moduleDecl = mod->md;
  if (!moduleDecl)
    return false;
  if (!moduleDecl->packages)
    return false;

  if (moduleDecl->packages->dim != 1)
    return false;
  if ((*moduleDecl->packages)[0] != Id::ldc)
    return false;

  return moduleDecl->id == Id::dcompute;
}

llvm::Optional<DcomputeAddrspacedType> toDcomputeAddrspacedType(VarDeclaration *vd) {
  StructDeclaration *sd = nullptr;
  if (vd->isThis())
    sd = vd->isThis()->isStructDeclaration();
  return toDcomputeAddrspacedType(sd);
}

llvm::Optional<DcomputeAddrspacedType> toDcomputeAddrspacedType(StructDeclaration *sd) {
  if (!sd ||
      !(sd->ident == Id::dcPointer || sd->ident == Id::dcVariable) ||
      !isFromLDC_DCompute(sd))
  {
    return llvm::Optional<DcomputeAddrspacedType>(llvm::None);
  }
    
  TemplateInstance *ti = sd->isInstantiated();
  int addrspace = isExpression((*ti->tiargs)[0])->toInteger();
  Type *type = isType((*ti->tiargs)[1]);
  return llvm::Optional<DcomputeAddrspacedType>(DcomputeAddrspacedType(addrspace, type,sd->ident));
}

unsigned addressSpaceForVarDeclaration(VarDeclaration *vd) {
    auto isDComputeAddrspace = toDcomputeAddrspacedType(vd);
    if (isDComputeAddrspace && isDComputeAddrspace->id == Id::dcVariable)
      return isDComputeAddrspace->translate();
    return 0;
}
