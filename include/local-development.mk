# allow for local directory containing source to be used

LOCAL_SRC ?= $(TOPDIR)/qca/src/$(PKG_NAME)

ifeq (exists, $(shell [ -d $(LOCAL_SRC) ] && echo exists))
PKG_VERSION:=$(shell GIT_DIR=$(LOCAL_SRC)/.git git describe --dirty --always | sed 's/.*-g/g/g')
PKG_SOURCE_URL:=
PKG_UNPACK=mkdir -p $(PKG_BUILD_DIR); $(CP) $(LOCAL_SRC)/* $(PKG_BUILD_DIR)/
endif
