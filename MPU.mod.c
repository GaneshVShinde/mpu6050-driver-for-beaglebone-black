#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x29f3b7ed, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x91f3f90, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0xc2408e23, __VMLINUX_SYMBOL_STR(i2c_del_driver) },
	{ 0xb6a5f7ae, __VMLINUX_SYMBOL_STR(i2c_register_driver) },
	{ 0x72b0b6c6, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0x46fd63f0, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xd82e017b, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0xf00f24e7, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0x8ff8b01c, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x1f61c126, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x4570217c, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x68dd0614, __VMLINUX_SYMBOL_STR(devm_kmalloc) },
	{ 0x67c2fa54, __VMLINUX_SYMBOL_STR(__copy_to_user) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0x14b5c9b6, __VMLINUX_SYMBOL_STR(i2c_smbus_read_byte_data) },
	{ 0x80bb0202, __VMLINUX_SYMBOL_STR(i2c_smbus_write_byte_data) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
	{ 0x5ac9553b, __VMLINUX_SYMBOL_STR(_dev_info) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

