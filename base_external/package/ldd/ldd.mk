##############################################################
#
# LDD-MODULES
#
##############################################################

# TODO: Reference the most recent commit hash from your ldd repository
LDD_VERSION = a5278560e904533bcd5ed59ffe5d70d35d44b79e

# Note: Be sure to reference the *ssh* repository URL here (not https) to work properly
# with ssh keys and the automated build/test system.
# Your site should start with git@github.com:
LDD_SITE = git@github.com:cu-ecen-aeld/assignment-7-asrao24.git
LDD_SITE_METHOD = git
LDD_GIT_SUBMODULES = YES

# Specify the subdirectories containing the kernel modules
LDD_MODULE_SUBDIRS = scull misc-modules

# Use the kernel-module infrastructure to build and install the modules
$(eval $(kernel-module))
$(eval $(generic-package))
