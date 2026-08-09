#!/usr/bin/env bash
# Renode + socat STDIO <-> USART1
# Usage: run-shell.sh <renode> <resc> <port>
set -euo pipefail

RENODE_BIN=$1
RESC=$2
PORT=$3
LOG=${TMPDIR:-/tmp}/stm32l100-renode.log
EXAMPLE_DIR=$(cd "$(dirname "$RESC")/.." && pwd)
PIDFILE=$EXAMPLE_DIR/.renode.pid
RENODE_PID=

cleanup() {
	trap - EXIT INT TERM HUP
	local pid=${RENODE_PID:-}
	if [[ -z $pid && -f $PIDFILE ]]; then
		pid=$(cat "$PIDFILE" 2>/dev/null || true)
	fi
	rm -f "$PIDFILE"
	if [[ -n ${pid:-} ]]; then
		kill "$pid" 2>/dev/null || true
		sleep 0.15
		kill -9 "$pid" 2>/dev/null || true
		pkill -9 -P "$pid" 2>/dev/null || true
	fi
}

trap cleanup EXIT INT TERM HUP

rm -f "$PIDFILE"
"$RENODE_BIN" --disable-xwt "$RESC" >"$LOG" 2>&1 &
RENODE_PID=$!
echo "$RENODE_PID" >"$PIDFILE"

for _ in $(seq 1 40); do
	if ss -ltn 2>/dev/null | grep -q ":${PORT} "; then
		break
	fi
	if ! kill -0 "$RENODE_PID" 2>/dev/null; then
		echo "Renode exited early:"; tail -40 "$LOG"
		exit 1
	fi
	sleep 0.25
done

if ! ss -ltn 2>/dev/null | grep -q ":${PORT} "; then
	echo "USART port ${PORT} not up:"; tail -40 "$LOG"
	exit 1
fi

# char-at-a-time; keep ISIG so Ctrl+C raises SIGINT (not literal ^C to guest)
opts=STDIO
if [[ -t 0 && -t 1 ]]; then
	opts=STDIO,echo=0,icanon=0,isig
fi
socat "$opts" "TCP:127.0.0.1:${PORT},retry=3,interval=0.2" || true
