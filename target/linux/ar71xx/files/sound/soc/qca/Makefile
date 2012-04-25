#
# Makefile for I2S
#
snd-soc-wasp-objs := wasp-pcm.o
snd-soc-wasp-i2s-objs := wasp-i2s.o #wasp-pll.o
snd-soc-db12x-objs := qca-db12x.o

obj-$(CONFIG_SND_WASP_SOC) += snd-soc-wasp.o
obj-$(CONFIG_SND_WASP_SOC_I2S) += snd-soc-wasp-i2s.o
obj-$(CONFIG_SND_WASP_SOC_DB120) += snd-soc-db12x.o

ccflags-y := -DDEBUG

