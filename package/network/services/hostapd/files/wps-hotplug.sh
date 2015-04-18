#!/bin/sh

if [ "$ACTION" = "pressed" -a "$BUTTON" = "wps" ]; then
	cd /var/run/hostapd
	for socket in *; do
		[ -S "$socket" ] || continue
		hostapd_cli -i "$socket" wps_pbc
	done
	cd /var/run/wpa_supplicant
	for socket in *; do
		[ -S "$socket" ] || continue
		wpa_cli -i "$socket" wps_pbc
	done
fi

if [ "$ACTION" = "released" -a "$BUTTON" = "wps" ]; then
	if [ "$SEEN" -gt 3 ]; then
		echo "FACTORY RESET" > /dev/console
		jffs2reset -y && reboot &
	fi
fi
