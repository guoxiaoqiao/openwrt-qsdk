# Copyright (c) 2013 Qualcomm Atheros, Inc.
# allow for local directory containing source to be used

LOCAL_SRC ?= $(TOPDIR)/qca/src/$(PKG_NAME)

ifeq (exists, $(shell [ -d $(LOCAL_SRC) ] && echo exists))
PKG_VERSION:=g$(shell cd $(LOCAL_SRC)/; git describe --dirty --long --always | sed 's/.*-g//g')
PKG_SOURCE_URL:=
PKG_UNPACK=mkdir -p $(PKG_BUILD_DIR); $(CP) $(LOCAL_SRC)/* $(PKG_BUILD_DIR)/
endif
