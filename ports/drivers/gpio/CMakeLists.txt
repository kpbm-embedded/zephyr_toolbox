# SPDX-License-Identifier: Apache-2.0
if(CONFIG_GPIO_ALLWINNER OR CONFIG_GPIO_ROCKCHIP_V2)
  zephyr_library_named(soc_ports_gpio)
  zephyr_library_sources_ifdef(CONFIG_GPIO_ALLWINNER gpio_allwinner.c)
  zephyr_library_sources_ifdef(CONFIG_GPIO_ROCKCHIP_V2 gpio_rockchip_v2.c)
endif()
