//
// NekoSU: compatibility shims for supercalls the kernel module does not provide.
//
// The manager UI is inherited from BakaSU and speaks the newer downstream
// supercall ABI (uapi v6+): full version, hook type, dynamic manager, managers
// list and kernel patch implement. The Samsung kernel module built in this
// repository implements uapi v5 only, so those commands are declared here for
// the JNI layer to compile.
//
// On this kernel the shimmed ioctls simply return -ENOTTY. Every caller in
// ksu.c treats a failed ioctl as "feature unavailable", so the manager degrades
// gracefully instead of breaking the build.
//
// When the kernel side grows these supercalls, drop the definitions and set
// KSU_UAPI_DOWNSTREAM_SUPERCALLS to 1 in uapi/supercall.h.
//

#ifndef KERNELSU_KSU_COMPAT_H
#define KERNELSU_KSU_COMPAT_H

#include "uapi/ksu.h"

#ifndef KSU_UAPI_DOWNSTREAM_SUPERCALLS
#define KSU_UAPI_DOWNSTREAM_SUPERCALLS 0
#endif

#if !KSU_UAPI_DOWNSTREAM_SUPERCALLS

#define KSU_FULL_VERSION_STRING 255

struct ksu_get_full_version_cmd {
    char version_full[KSU_FULL_VERSION_STRING]; // Output: full version string
};

struct ksu_hook_type_cmd {
    char hook_type[32]; // Output: hook type string
};

static const __u8 DYNAMIC_MANAGER_OP_SET = 0;
static const __u8 DYNAMIC_MANAGER_OP_GET = 1;
static const __u8 DYNAMIC_MANAGER_OP_WIPE = 2;
static const __u8 DYNAMIC_MANAGER_OP_SET_SYNCHRONOUS = 3;

struct ksu_dynamic_manager_cmd {
    __u8 operation;
    unsigned int size;
    __u8 hash[64];
};

struct ksu_manager_entry {
    __u32 uid;
    __u8 signature_index;
} __attribute__((packed));

struct ksu_get_managers_cmd {
    __u16 count; // Input / Output: number of managers in array
    __u16 total_count; // Output: total number of managers in requested list
    struct ksu_manager_entry managers[]; // Output: array of active managers
} __attribute__((packed));

static const __u8 KERNEL_PATCH_NOT_FOUND = 0;
static const __u8 KERNEL_PATCH_ORIGINAL = 1;
static const __u8 KERNEL_PATCH_KPN = 2;
static const __u8 KERNEL_PATCH_SUKISU = 3;

struct ksu_get_kernel_patch_implement {
    __u8 type; // Output: current kernel patch implementation
};

/* Downstream supercall IOCTLs, opcodes match the upstream allocation. */
static const __u32 KSU_IOCTL_GET_FULL_VERSION = _IOC(_IOC_READ, 'K', 100, 0);
static const __u32 KSU_IOCTL_HOOK_TYPE = _IOC(_IOC_READ, 'K', 101, 0);
static const __u32 KSU_IOCTL_DYNAMIC_MANAGER = _IOC(_IOC_READ | _IOC_WRITE, 'K', 103, 0);
static const __u32 KSU_IOCTL_GET_MANAGERS = _IOC(_IOC_READ | _IOC_WRITE, 'K', 105, 0);
static const __u32 KSU_IOCTL_GET_KERNEL_PATCH_IMPLEMENT = _IOC(_IOC_READ, 'K', 106, 0);

#endif // !KSU_UAPI_DOWNSTREAM_SUPERCALLS

#endif // KERNELSU_KSU_COMPAT_H
