#!/bin/bash
cat components/cmd_router/cmd_router.c | grep -n "ESP_LOGI.*password"
cat components/cmd_router/cmd_router.c | grep -n "ESP_LOGI.*Password"
