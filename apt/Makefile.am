DISTRIBUTIONS = debian ubuntu
CHROOT_BASE = /var/lib/chroot
ARCHITECTURES = i386 amd64
CODE_NAMES = lenny squeeze unstable hardy lucid

release: build update sign upload

remove-existing-packages:
	for distribution in $(DISTRIBUTIONS); do	\
	  find $${distribution}/pool -type f -delete;	\
	done

update:
	$(srcdir)/update-repository.sh \
	  '$(SF_PROJECT_NAME)' '$(PACKAGE_TITLE)' '$(CODE_NAMES)'

sign:
	$(srcdir)/sign-repository.sh '$(CODE_NAMES)'

ensure-rsync-path:
	@if test -z "$(RSYNC_PATH)"; then				\
	  echo "--with-rsync-path configure option must be specified.";	\
	  false;							\
	fi

upload: ensure-rsync-path
	for distribution in $(DISTRIBUTIONS); do			\
	  (cd $${distribution};						\
	   rsync -avz dists pool $(RSYNC_PATH)/$${distribution};	\
	  );								\
	done

download: ensure-rsync-path
	for distribution in $(DISTRIBUTIONS); do			\
	  (cd $${distribution};						\
	   rsync -avz $(RSYNC_PATH)/$${distribution}/pool/ pool;	\
	  );								\
	done

build:
	$(srcdir)/build-in-chroot.sh \
	  $(PACKAGE) $(VERSION) $(CHROOT_BASE) \
	  '$(ARCHITECTURES)' '$(CODE_NAMES)'
