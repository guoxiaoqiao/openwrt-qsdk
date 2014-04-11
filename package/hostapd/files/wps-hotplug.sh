#!/bin/sh

if [ "$ACTION" = "pressed" -a "$BUTTON" = "wps" ]; then
	for dir in /var/run/hostapd-*; do
		[ -d "$dir" ] || continue
		hostapd_cli -p "$dir" wps_pbc
	done
	for dir in /var/run/wpa_supplicant-*; do
		[ -d "$dir" ] || continue
		wpa_cli -p "$dir" wps_pbc
	done
fi
