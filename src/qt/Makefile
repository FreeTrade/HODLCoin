.PHONY: FORCE
all: FORCE
	$(MAKE) -C .. hodlcoin_qt test_hodlcoin_qt
clean: FORCE
	$(MAKE) -C .. hodlcoin_qt_clean test_hodlcoin_qt_clean
check: FORCE
	$(MAKE) -C .. test_hodlcoin_qt_check
hodlcoin-qt hodlcoin-qt.exe: FORCE
         $(MAKE) -C .. hodlcoin_qt
