LOCINC=include/
MAKE=make -s

EXPORT=LOCINC=../$(LOCINC) 
###############################################
all:
	@cd src && $(EXPORT) $(MAKE)

clean:
	@cd src && $(EXPORT) $(MAKE) clean
