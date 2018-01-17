// REQUIRES: atleast_llvm309
// REQUIRES: target_NVPTX
// RUN: %ldc -c -mdcompute-targets=cuda-350 -m64 -mdcompute-file-prefix=addrspace -output-ll -output-o %s && FileCheck %s --check-prefix=LL < addrspace_cuda350_64.ll && FileCheck %s --check-prefix=PTX < addrspace_cuda350_64.ptx
@compute(CompileFor.deviceOnly) module dcompute_cu_addrspaces;
import ldc.dcompute;

// LL: %"ldc.dcompute.Pointer!(cast(AddrSpace)0u, float).Pointer" = type { float addrspace(5)* }
// LL: %"ldc.dcompute.Pointer!(cast(AddrSpace)1u, float).Pointer" = type { float addrspace(1)* }
// LL: %"ldc.dcompute.Pointer!(cast(AddrSpace)2u, float).Pointer" = type { float addrspace(3)* }
// LL: %"ldc.dcompute.Pointer!(cast(AddrSpace)3u, immutable(float)).Pointer" = type { float addrspace(4)* }
// LL: %"ldc.dcompute.Pointer!(cast(AddrSpace)4u, float).Pointer" = type { float* }

// LL: @_D{{.*}}bar1{{.*}} = addrspace(1)
Global!float bar1;
// LL: @_D{{.*}}bar2{{.*}} = addrspace(3)
Shared!float bar2;
// LL: @_D{{.*}}bar3{{.*}} = addrspace(4)
Constant!float bar3;


void foo(PrivatePointer!float f) {
    // LL: load float, float addrspace(5)*
    // PTX: ld.local.f32
    float g = *f;
}
void foo(GlobalPointer!float f) {
    // LL: load float, float addrspace(1)*
    // PTX: ld.global.f32
    float g = *f;
}
void foo(SharedPointer!float f) {
    // LL: load float, float addrspace(3)*
    // PTX: ld.shared.f32
    float g = *f;
}
void foo(ConstantPointer!float f) {
    // LL: load float, float addrspace(4)*
    // PTX: ld.const.f32
    float g = *f;
}
void foo(GenericPointer!float f) {
    // LL: load float, float*
    // PTX: ld.f32
    float g = *f;
}

