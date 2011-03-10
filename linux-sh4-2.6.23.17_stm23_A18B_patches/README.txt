Description of generic patches 
------------------------------	

* generic_001_arch-sh-kernel-setup.c.patch
  Fix for u-boot for 32 bits support 

* generic_002_kernel-resource.c.patch
  Remove annoying "Trying to free inexistant resource.." message
  
* generic_100_board_hmp7109.patch
  Add HMP7109_7109 Board support 

* generic_101_board_sat7111.patch
  Add SAT7111_7111 Board support
  
* generic_102_board_idtv7109.patch
  Add IDTV7109_7109 Board support 

* generic_103_board_mtv7109.patch
  Add MTV7109_7109 Board support 

* generic_104_board_mb628.patch
  Modify MB628 Board support to add cable modem attach and use GMAC1 as default

* generic_105_board_mb628e.patch
  Add MB628E Board support 

Exclusive patch to be installed for board support 
-------------------------------------------------

* board_custom001XXX.patch
  Add support for all boards custom001XXX
  
How to apply generic patchs to the kernel ?
------------------------------------------
cd /opt/STM/STLinux-2.3/devkit/sources/kernel/linux-sh4
for i in linux-sh4-2.6.23.17_stm23_A18B_patches/generic_*.patch; do patch -p0 -b < "$i"; done

How to apply board patchs to the kernel ?
-----------------------------------------
Install only the patch required for your board and do :
patch -p0 -b < linux-sh4-2.6.23.17_stm23_A18B_patches/board_<BOARDNAME>.patch
