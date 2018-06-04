CONTIKI_PROJECT=myogame
all: $(CONTIKI_PROJECT)

TARGET=srf06-cc26xx
BOARD=sensortag/cc2650

PROJECT_SOURCEFILES += adc.c udpclient.c

UNIFLASH = $(HOME)/uniflash_4.1/dslite.sh
CCXML_FILE = $(HOME)/contiki-examples/tools/sensortag.ccxml


CONTIKI = $(HOME)/contiki-git
CONTIKI_WITH_IPV6 = 1
#UIP_CONF_ROUTER = 0
# CFLAGS += -DUIP_CONF_ND6_SEND_NA=1
# CFLAGS += -DUIP_CONF_ND6_SEND_RA=0
# CFLAGS += -DUIP_CONF_ND6_SEND_NS=0
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
include $(CONTIKI)/Makefile.include
# DEFINES += UIP_CONF_ND6_SEND_NA
#DEFINES += UIP_CONF_ND6_SEND_NS



erase: 
	$(UNIFLASH) --config $(CCXML_FILE) --before Erase

prog:
	/home/csse4011/uniflash_4.1/deskdb/content/TICloudAgent/linux/ccs_base/DebugServer/bin/DSLite load -c $(CCXML_FILE) -f $(CONTIKI_PROJECT).elf
	
progcal: 
	$(HOME)/Downloads/DSLite/DebugServer/bin/DSLite load -c ~/Downloads/DSLite/CC2650F128_TIXDS110_Connection.ccxml -f $(CONTIKI_PROJECT).elf

#$(UNIFLASH) --config $(CCXML_FILE) $(CONTIKI_PROJECT).elf --before Erase
reset:
	$(UNIFLASH) --config $(CCXML_FILE) --post-flash-device-cmd PinReset

progu: 
	$(HOME)/uniflash_3.4/uniflash.sh -ccxml ~/Downloads/sensortag.ccxml -targetOp reset -operation Erase
	$(HOME)/uniflash_3.4/uniflash.sh -ccxml ~/Downloads/sensortag.ccxml -program $(CONTIKI_PROJECT).hex
