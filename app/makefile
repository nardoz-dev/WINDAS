# name of your application
APPLICATION = WINDFORME

# If no BOARD is found in the environment, use this default:
BOARD ?= nucleo-f401re

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../RIOT

# Provide network connectivity via the USB

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6
USEMODULE += gnrc_ipv6_default
# Optimize network stack to for use with a single network interface
USEMODULE += gnrc_netif_single
USEMODULE += stdio_ethos gnrc_uhcpc
# Include MQTT-SN
USEMODULE += emcute

# EMCUTE_ID ?= 01
ETHOS_BAUDRATE ?= 115200
TAP ?= tap0
USE_DHCPV6 ?= 0
IPV6_PREFIX ?= fe80:2::/64
# IPv6 and port of the MQTT broker
SERVER_ADDR = fe80::1
SERVER_PORT = 1885

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 0

# Modules to include:
USEMODULE += dht
USEMODULE += fmt
USEMODULE += xtimer
USEMODULE += periph_gpio

include $(CURDIR)/Makefile.ethos.conf
include $(RIOTBASE)/Makefile.include