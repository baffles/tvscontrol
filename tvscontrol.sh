#! /bin/sh

### BEGIN INIT INFO
# Provides:          tvscontrol
# Required-Start:    $network $local_fs $syslog dbus
# Required-Stop:     $network $local_fs $syslog dbus
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: tvscontrol
# Description:       tvscontrol
### END INIT INFO

#http://blog.scphillips.com/2013/07/getting-a-python-script-to-run-in-the-background-as-a-service-on-boot/

ARGS=192.168.2.99

COMMAND=/usr/bin/tvscontrol
USER=root # necessary for usb device access
PIDFILE=/var/run/tvscontrol.pid

. /lib/lsb/init-functions

case "$1" in
    start)
		log_daemon_msg "Starting tvscontrol" "tvscontrol"
		if start-stop-daemon --start --quiet --oknodo --background --pidfile $PIDFILE --make-pidfile --user $USER --exec $COMMAND -- $ARGS; then
			log_end_msg 0
		else
			log_end_msg 1
		fi
		;;
    stop)
		log_daemon_msg "Shutting down tvscontrol" "tvscontrol"
		if start-stop-daemon --stop --pidfile $PIDFILE --quiet --oknodo --exec $COMMAND; then
			log_end_msg 0
		else
			log_end_msg 1
		fi
		;;
    restart)
		$0 stop
		$0 start
		;;
    *)
		log_action_msg "Usage: $0 {start|stop|restart}"
		exit 1
		;;
esac
 
exit 0
