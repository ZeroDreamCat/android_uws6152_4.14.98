/* Copyright (C) SoraNeko
 * MicroArray Fprint Driver Code
 * sprd-settings.h
 * Date: 2026-5-9
 * Version: v4.0.06_sprd
 * Author: porting
 */

#ifndef __SPRD_SETTINGS_H_
#define __SPRD_SETTINGS_H_

#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/of_irq.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/clk.h>
#include <linux/spi/spi.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <linux/spi/spi.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/time.h>
#include <linux/input.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/gpio.h>
#include <linux/spi/spi.h>

// 不再需要 mtk_spi.h，不需要 mt_chip_conf 等私有结构体

#include "madev.h"

#define MA_DRV_NAME             "madev"

#define MA_DTS_NAME            "microarray,mafp"

// 中断将直接从 SPI 设备节点获取，不再需要独立的中断节点名
#define MA_EINT_DTS_NAME        "microarray,mafp"

// 外部函数声明，这些函数在 madev.c 中定义
extern int mas_plat_probe(struct platform_device *pdev);
extern int mas_plat_remove(struct platform_device *pdev);
extern int mas_probe(struct spi_device *spi);
extern int mas_remove(struct spi_device *spi);

// SPI 时钟开关函数（紫光上为空）
void mas_enable_spi_clock(struct spi_device *spi);
void mas_disable_spi_clock(struct spi_device *spi);

// SPI 传输模式选择（紫光上可留空）
void mas_select_transfer(struct spi_device *spi, int len);

// 获取 GPIO 信息，参数改为 spi_device
int mas_finger_get_gpio_info(struct spi_device *spi);

// 设置 GPIO 状态（Pinctrl 控制）
int mas_finger_set_gpio_info(int cmd);

// 获取中断号
int mas_get_irq(struct device *dev);

// 获取/移除平台资源（精简版）
int mas_get_platform(void);
int mas_remove_platform(void);

// 电源控制
int mas_power(int cmd);
int get_screen(void);

// SPI 速度修改
void ma_spi_change(struct spi_device *spi, unsigned int speed, int flag);

// 获取中断 GPIO 电平（原函数保留）
int mas_get_interrupt_gpio(unsigned int index);

// 电源开关
int mas_switch_power(unsigned int on_off);

// 在 probe 中额外操作（当前为空）
int mas_do_some_for_probe(struct spi_device *spi);

#endif /* __SPRD_SETTINGS_H_ */
