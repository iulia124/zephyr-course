if(CONFIG_IULIA_LED_SENSOR)
  zephyr_library()
  zephyr_library_sources(iulia_led_sensor.c)
endif()