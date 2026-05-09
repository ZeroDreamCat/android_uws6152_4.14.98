/* Copyright (C) MicroArray
 * MicroArray Fprint Driver Code
 * sprd-settings.c
 * Date: 2026-5-9
 * Version: v4.0.06_sprd
 * Author: SoraNeko
 */

#include "sprd-settings.h"

static int ret;

// pinctrl 状态指针
struct pinctrl *mas_finger_pinctrl;
struct pinctrl_state     *mas_finger_eint_on, *mas_finger_eint_off,
                        *mas_spi_ck_on,  *mas_spi_ck_off,
                        *mas_spi_cs_on,  *mas_spi_cs_off,
                        *mas_spi_mi_on,  *mas_spi_mi_off,
                        *mas_spi_mo_on,  *mas_spi_mo_off,
                        *mas_spi_default;

// 匹配表
static const struct of_device_id sof_match[] = {
    { .compatible = MA_DTS_NAME, },
    { }
};
MODULE_DEVICE_TABLE(of, sof_match);

// SPI 驱动结构体
static struct spi_device_id sdev_id = {MA_DRV_NAME, 0};
static struct spi_driver sdrv = {
    .driver = {
        .name   = MA_DRV_NAME,
        .bus    = &spi_bus_type,
        .owner  = THIS_MODULE,
        .of_match_table = sof_match,
    },
    .probe    = mas_probe,
    .remove   = mas_remove,
    .id_table = &sdev_id,
};

/* ========== 接口实现 ========== */

void mas_select_transfer(struct spi_device *spi, int len)
{
    // 紫光 SPI 驱动自动选择传输模式，无需干预
}

void ma_spi_change(struct spi_device *spi, unsigned int speed, int flag)
{
    spi->max_speed_hz = speed;
    if (spi_setup(spi) < 0) {
        MALOGE("change the spi speed error!\n");
    }
}

void mas_enable_spi_clock(struct spi_device *spi)
{
    // 紫光 SPI 控制器自动管理时钟
}

void mas_disable_spi_clock(struct spi_device *spi)
{
    // 紫光 SPI 控制器自动管理时钟
}

int __init mas_get_platform(void)
{
    MALOGD("start!");
    ret = spi_register_driver(&sdrv);
    if (ret) {
        MALOGE("spi_register_driver failed");
    }
    return ret;
}

int mas_remove_platform(void)
{
    spi_unregister_driver(&sdrv);
    return 0;
}

int mas_finger_get_gpio_info(struct spi_device *spi)
{
    MALOGD("start!");

    // 尝试获取 pinctrl，如果系统没有为指纹配置 pinctrl，
    // 那么引脚功能已经在平台初始化中设置好了，不会影响中断和 SPI。
    mas_finger_pinctrl = devm_pinctrl_get(&spi->dev);
    if (IS_ERR(mas_finger_pinctrl)) {
        dev_warn(&spi->dev, "No pinctrl defined for fingerprint, assuming pins are already configured.\n");
        mas_finger_pinctrl = NULL;
        // 所有 pinctrl state 都置为 NULL，后续判空跳过
        mas_spi_mi_on = mas_spi_mi_off = NULL;
        mas_spi_mo_on = mas_spi_mo_off = NULL;
        mas_spi_ck_on = mas_spi_ck_off = NULL;
        mas_spi_cs_on = mas_spi_cs_off = NULL;
        mas_finger_eint_on = mas_finger_eint_off = NULL;
        mas_spi_default = NULL;
        MALOGD("end! (no pinctrl)");
        return 0;
    }

    // 依次查找各状态，查找失败也继续，并设为 NULL
    mas_spi_mi_on = pinctrl_lookup_state(mas_finger_pinctrl, "fpc_mode_as_mi");
    if (IS_ERR(mas_spi_mi_on)) mas_spi_mi_on = NULL;

    mas_spi_mi_off = pinctrl_lookup_state(mas_finger_pinctrl, "fpc_miso_pull_down");
    if (IS_ERR(mas_spi_mi_off)) mas_spi_mi_off = NULL;

    mas_spi_mo_on = pinctrl_lookup_state(mas_finger_pinctrl, "fpc_mode_as_mo");
    if (IS_ERR(mas_spi_mo_on)) mas_spi_mo_on = NULL;

    mas_spi_mo_off = pinctrl_lookup_state(mas_finger_pinctrl, "fpc_mosi_pull_down");
    if (IS_ERR(mas_spi_mo_off)) mas_spi_mo_off = NULL;

    mas_spi_ck_on = pinctrl_lookup_state(mas_finger_pinctrl, "fpc_mode_as_ck");
    if (IS_ERR(mas_spi_ck_on)) mas_spi_ck_on = NULL;

    mas_spi_ck_off = pinctrl_lookup_state(mas_finger_pinctrl, "fpc_ck_pull_down");
    if (IS_ERR(mas_spi_ck_off)) mas_spi_ck_off = NULL;

    mas_spi_cs_on = pinctrl_lookup_state(mas_finger_pinctrl, "fpc_mode_as_cs");
    if (IS_ERR(mas_spi_cs_on)) mas_spi_cs_on = NULL;

    mas_spi_cs_off = pinctrl_lookup_state(mas_finger_pinctrl, "fpc_cs_pull_down");
    if (IS_ERR(mas_spi_cs_off)) mas_spi_cs_off = NULL;

    mas_finger_eint_on = pinctrl_lookup_state(mas_finger_pinctrl, "fpc_eint_as_int");
    if (IS_ERR(mas_finger_eint_on)) {
        dev_warn(&spi->dev, "fpc_eint_as_int state not found, using default eint config\n");
        mas_finger_eint_on = NULL;
    }

    mas_finger_eint_off = pinctrl_lookup_state(mas_finger_pinctrl, "fpc_eint_as_int_off");
    if (IS_ERR(mas_finger_eint_off)) mas_finger_eint_off = NULL;

    MALOGD("end!");
    return 0;
}

int mas_finger_set_spi(int cmd)
{
    // 只有对应的 state 存在时才设置
    switch (cmd) {
    case 0:
        if (mas_spi_mi_off) pinctrl_select_state(mas_finger_pinctrl, mas_spi_mi_off);
        if (mas_spi_mo_off) pinctrl_select_state(mas_finger_pinctrl, mas_spi_mo_off);
        if (mas_spi_ck_off) pinctrl_select_state(mas_finger_pinctrl, mas_spi_ck_off);
        if (mas_spi_cs_off) pinctrl_select_state(mas_finger_pinctrl, mas_spi_cs_off);
        break;
    case 1:
        if (mas_spi_cs_on) pinctrl_select_state(mas_finger_pinctrl, mas_spi_cs_on);
        if (mas_spi_ck_on) pinctrl_select_state(mas_finger_pinctrl, mas_spi_ck_on);
        if (mas_spi_mi_on) pinctrl_select_state(mas_finger_pinctrl, mas_spi_mi_on);
        if (mas_spi_mo_on) pinctrl_select_state(mas_finger_pinctrl, mas_spi_mo_on);
        break;
    }
    return 0;
}

int mas_finger_set_power(int cmd)
{
    // 未使用 pinctrl 控制电源，直接返回成功
    return 0;
}

int mas_switch_power(unsigned int on_off)
{
    return mas_finger_set_power(on_off);
}

int mas_finger_set_eint(int cmd)
{
    switch (cmd) {
    case 0:
        if (mas_finger_eint_off)
            pinctrl_select_state(mas_finger_pinctrl, mas_finger_eint_off);
        break;
    case 1:
        if (mas_finger_eint_on)
            pinctrl_select_state(mas_finger_pinctrl, mas_finger_eint_on);
        break;
    }
    return 0;
}

int mas_finger_set_gpio_info(int cmd)
{
    ret = 0;
    ret |= mas_finger_set_spi(cmd);
    ret |= mas_finger_set_power(cmd);
    ret |= mas_finger_set_eint(cmd);
    return ret;
}

int mas_get_irq(struct device *dev)
{
    struct device_node *np = dev->of_node;
    int gpio, irq;
    enum of_gpio_flags flags;

    if (!np) {
        MALOGE("No device node for interrupt\n");
        return -EINVAL;
    }

    gpio = of_get_named_gpio_flags(np, "fpint-gpios", 0, &flags);
    if (!gpio_is_valid(gpio)) {
        MALOGE("Failed to get fpint-gpios\n");
        return -EINVAL;
    }

    // 规范申请 GPIO
    if (devm_gpio_request_one(dev, gpio, GPIOF_IN, "fp-int")) {
        MALOGE("Failed to request fpint gpio\n");
        return -EINVAL;
    }

    irq = gpio_to_irq(gpio);
    if (irq < 0) {
        MALOGE("Failed to map gpio to irq\n");
        return irq;
    }

    MALOGD("Got irq %d from gpio %d\n", irq, gpio);
    return irq;
}

int mas_get_interrupt_gpio(unsigned int index)
{
    // 原实现为空，直接返回 1 表示高电平（可根据需要实现）
    return 1;
}

int mas_do_some_for_probe(struct spi_device *spi)
{
    return 0;
}
