#! /usr/bin/env bash

MQTT_HOST='127.0.0.1'
MQTT_TOPIC='/input'

K_CODE=
K_TYPE=
K_VALUE=

handle_code() {
	#echo "handling $1"

	case $1 in
		* )
			echo "Unhandled $1" ;;
	esac

}

read_next_line() {
	read K_CODE K_TYPE K_VALUE
	local RET=$?
	return $RET
}

read_sub() {
	while read_next_line; do
		if [ "1" == "$K_TYPE" ]; then
			t=
			case $K_VALUE in
				0 )
					t="up" ;;
				1 )
					t="down" ;;
				2 )
					t="hold" ;;
			esac
			echo "$K_CODE [${t}]"
			if [ "1" == "$K_VALUE" ]; then
				handle_code $K_CODE
			fi
		fi
	done
}

mosquitto_sub -h $MQTT_HOST -t $MQTT_TOPIC | read_sub

