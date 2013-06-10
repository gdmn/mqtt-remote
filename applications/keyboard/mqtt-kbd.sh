#! /usr/bin/env bash

MQTT_HOST='192.168.1.3'
MQTT_TOPIC='/input'
. ../../remotes/keyboard

K_CODE=
K_TYPE=
K_VALUE=

handle_code() {
	#echo "handling $1"

	case $1 in
		$code_space )
			mocp --toggle-pause & ;;

		$code_n )
			mocp --next & ;;
		$code_p )
			mocp --previous & ;;

		$code_left )
			mocp --seek '-10' & ;;
		$code_right )
			mocp --seek '+10' & ;;

		$code_up )
			mocp --volume '+10' & ;;
		$code_down )
			mocp --volume '-10' & ;;

		$code_i )
			( info="`mocp --info`" ; \
				mosquitto_pub -h $MQTT_HOST -t $MQTT_TOPIC -m "$info" ) & ;;
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

mocp -S

mosquitto_sub -h $MQTT_HOST -t $MQTT_TOPIC | read_sub

