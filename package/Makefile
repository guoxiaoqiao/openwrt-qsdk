#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

curdir:=package

-include $(TMP_DIR)/.packagedeps
$(curdir)/builddirs:=$(sort $(package-) $(package-y) $(package-m))
ifeq ($(SDK),1)
  $(curdir)/builddirs-install:=.
else
  $(curdir)/builddirs-default:=. $(sort $(package-y) $(package-m))
  $(curdir)/builddirs-prereq:=. $(sort $(prereq-y) $(prereq-m))
  $(curdir)/builddirs-install:=. $(filter-out base-files,$(sort $(package-y))) $(filter base-files,$(package-y))
endif
ifneq ($(IGNORE_ERRORS),)
  $(curdir)/builddirs-ignore-compile:= $(if $(filter n m y, $(IGNORE_ERRORS)),$(foreach m,$(IGNORE_ERRORS),$(package-$(subst n,,$(m)))),$(package-m) $(package-))
endif

$(curdir)/install:=$(curdir)/install-cleanup

$(curdir)/cleanup: $(TMP_DIR)/.build
	- find $(STAGING_DIR_ROOT) -type d | $(XARGS) chmod 0755
	rm -rf $(TARGET_DIR) $(STAGING_DIR_ROOT)

ifdef CONFIG_USE_MKLIBS
  define mklibs
	rm -rf $(TMP_DIR)/mklibs-progs $(TMP_DIR)/mklibs-out
	# first find all programs and add them to the mklibs list
	find $(STAGING_DIR_ROOT) -type f -perm +100 -exec \
		file -r -N -F '' {} + | \
		awk ' /executable.*dynamically/ { print $$1 }' > $(TMP_DIR)/mklibs-progs
	# find all loadable objects that are not regular libraries and add them to the list as well
	find $(STAGING_DIR_ROOT) -type f -name \*.so\* -exec \
		file -r -N -F '' {} + | \
		awk ' /shared object/ { print $$1 }' >> $(TMP_DIR)/mklibs-progs
	mkdir -p $(TMP_DIR)/mklibs-out
	$(STAGING_DIR_HOST)/bin/mklibs -D \
		-d $(TMP_DIR)/mklibs-out \
		--sysroot $(STAGING_DIR_ROOT) \
		-L /lib \
		-L /usr/lib \
		-L /usr/lib/ebtables \
		--ldlib $(patsubst $(STAGING_DIR_ROOT)/%,/%,$(firstword $(wildcard \
			$(foreach name,ld-uClibc.so.* ld-linux.so.* ld-*.so, \
			  $(STAGING_DIR_ROOT)/lib/$(name) \
			)))) \
		--target $(REAL_GNU_TARGET_NAME) \
		`cat $(TMP_DIR)/mklibs-progs` 2>&1
	$(RSTRIP) $(TMP_DIR)/mklibs-out
	for lib in `ls $(TMP_DIR)/mklibs-out/*.so.* 2>/dev/null`; do \
		LIB="$${lib##*/}"; \
		DEST="`ls "$(TARGET_DIR)/lib/$$LIB" "$(TARGET_DIR)/usr/lib/$$LIB" 2>/dev/null`"; \
		[ -n "$$DEST" ] || continue; \
		echo "Copying stripped library $$lib to $$DEST"; \
		cp "$$lib" "$$DEST" || exit 1; \
	done
  endef
endif

define push_git_build_stats
	GIT_CONFIG="--git-dir=$(BIN_DIR)/manifest/.git --work-tree=$(BIN_DIR)/manifest"; \
	[ -e "$(BIN_DIR)/manifest/.git" ] || { \
		  git $${GIT_CONFIG} init; \
	}; \
	git $${GIT_CONFIG} remote add origin $(CONFIG_GIT_BUILD_STATS_REPO); \
	git $${GIT_CONFIG} fetch --all; \
	git $${GIT_CONFIG} reset origin/$(CONFIG_GIT_BUILD_STATS_BRANCH); \
	git $${GIT_CONFIG} add manifest.txt; \
	git $${GIT_CONFIG} commit -asm "build on $$(date)"; \
	git $${GIT_CONFIG} push origin HEAD:$(CONFIG_GIT_BUILD_STATS_BRANCH);
endef

$(curdir)/rootfs-prepare: $(TMP_DIR)/.build
	@-$(MAKE) package/preconfig
	@if [ -d $(TOPDIR)/files ]; then \
		( cd $(TOPDIR)/files; find -type f ) | \
			( cd $(TARGET_DIR); while :; do \
				read FILE; \
				[ -z "$$FILE" ] && break; \
				[ -L "$$FILE" ] || continue; \
				echo "Removing symlink $(TARGET_DIR)/$$FILE"; \
				rm -f "$$FILE"; \
			done; ); \
		$(CP) $(TOPDIR)/files/. $(TARGET_DIR); \
	fi
	@mkdir -p $(TARGET_DIR)/etc/rc.d
	@( \
		cd $(TARGET_DIR); \
		for script in ./etc/init.d/*; do \
			grep '#!/bin/sh /etc/rc.common' $$script >/dev/null || continue; \
			IPKG_INSTROOT=$(TARGET_DIR) $$(which bash) ./etc/rc.common $$script enable; \
		done || true \
	)
	@-find $(TARGET_DIR) -name CVS   | $(XARGS) rm -rf
	@-find $(TARGET_DIR) -name .svn  | $(XARGS) rm -rf
	@-find $(TARGET_DIR) -name '.#*' | $(XARGS) rm -f
	rm -f $(TARGET_DIR)/usr/lib/opkg/info/*.postinst

	mkdir -p $(BIN_DIR)/manifest
	OPKG_T="$(STAGING_DIR_HOST)/bin/opkg \
		--offline-root=$(TARGET_DIR) \
		--add-arch $(if $(ARCH_PACKAGES),$(ARCH_PACKAGES),$(BOARD)):200 \
		--add-arch all:100"; \
	pkgs="`$${OPKG_T} list-installed | awk '{print $$1}'`"; \
	for pkg in $${pkgs}; do \
		$${OPKG_T} info $${pkg} | grep -v Installed-Time; \
		f=$$($${OPKG_T} files $${pkg} | sort | grep ^/); \
		[ -n "$$f" ] && { du -ch $$f | sed 's,$(TARGET_DIR),,g'; echo; } \
	done > $(BIN_DIR)/manifest/manifest.txt

	$(if $(CONFIG_GIT_BUILD_STATS_REPO),$(call push_git_build_stats))

	$(if $(CONFIG_CLEAN_IPKG),rm -rf $(TARGET_DIR)/usr/lib/opkg)
	$(call mklibs)

$(curdir)/index: FORCE
	@(cd $(PACKAGE_DIR); $(SCRIPT_DIR)/ipkg-make-index.sh . 2>&1 > Packages && \
		gzip -9c Packages > Packages.gz \
	)

$(curdir)/flags-install:= -j1

$(eval $(call stampfile,$(curdir),package,prereq,.config))
$(eval $(call stampfile,$(curdir),package,cleanup,$(TMP_DIR)/.build))
$(eval $(call stampfile,$(curdir),package,compile,$(TMP_DIR)/.build))
$(eval $(call stampfile,$(curdir),package,install,$(TMP_DIR)/.build))
$(eval $(call stampfile,$(curdir),package,rootfs-prepare,$(TMP_DIR)/.build))

$(eval $(call subdir,$(curdir)))
