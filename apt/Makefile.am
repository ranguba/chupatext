DISTRIBUTIONS = debian ubuntu
CHROOT_BASE = /var/lib/chroot
ARCHITECTURES = i386 amd64
CODE_NAMES = squeeze unstable lucid maverick

release: dist build update sign upload

remove-existing-packages:
	for distribution in $(DISTRIBUTIONS); do	\
	  find $${distribution}/pool -type f -delete;	\
	done

update:
	$(srcdir)/update-repository.sh \
	  '$(PROJECT_NAME)' '$(PACKAGE_TITLE)' '$(CODE_NAMES)'

sign:
	$(srcdir)/sign-repository.sh '$(CODE_NAMES)'

upload:
	for distribution in $(DISTRIBUTIONS); do			\
	  (cd $${distribution};						\
	   rsync -avz dists pool $(RSYNC_PATH)/$${distribution};	\
	  );								\
	done

download:
	for distribution in $(DISTRIBUTIONS); do			\
	  (cd $${distribution};						\
	   rsync -avz $(RSYNC_PATH)/$${distribution}/pool/ pool;	\
	  );								\
	done

build:
	$(srcdir)/build-in-chroot.sh \
	  $(PACKAGE) $(VERSION) $(CHROOT_BASE) \
	  '$(ARCHITECTURES)' '$(CODE_NAMES)'

dist:
	cd $(top_builddir) && $(MAKE) $@
