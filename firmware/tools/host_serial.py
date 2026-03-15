#!/usr/bin/env python3
"""Host-side serial tool for the Scintillator Probe.

Uses COBS-framed packed structs over USB CDC (virtual serial port).

Requirements: pip install pyserial cobs
"""

import argparse
import struct
import sys
import time

import serial
from cobs import cobs


# Message IDs (must match firmware messages.h)
MSG_TARGET_POWER  = 0x01
MSG_VOLTAGE_SENSE = 0x02


class ScintillatorProbe:
    def __init__(self, port: str, baudrate: int = 115200, timeout: float = 0.1):
        self.ser = serial.Serial(port, baudrate=baudrate, timeout=timeout)
        self._rx_buf = bytearray()

    def close(self):
        self.ser.close()

    def send_target_power(self, voltage: int):
        """Send a TargetPower message (PC -> MCU)."""
        payload = struct.pack("<BH", MSG_TARGET_POWER, voltage)
        frame = cobs.encode(payload) + b"\x00"
        self.ser.write(frame)

    def read_messages(self):
        """Read and yield decoded messages from the serial port."""
        data = self.ser.read(256)
        if data:
            self._rx_buf.extend(data)

        while b"\x00" in self._rx_buf:
            idx = self._rx_buf.index(b"\x00")
            frame = bytes(self._rx_buf[:idx])
            self._rx_buf = self._rx_buf[idx + 1:]

            if len(frame) == 0:
                continue

            try:
                payload = cobs.decode(frame)
            except cobs.DecodeError:
                continue

            if len(payload) < 1:
                continue

            msg_id = payload[0]
            if msg_id == MSG_VOLTAGE_SENSE and len(payload) >= 3:
                voltage, = struct.unpack_from("<H", payload, 1)
                yield {"type": "voltage_sense", "voltage": voltage}
            elif msg_id == MSG_TARGET_POWER and len(payload) >= 3:
                voltage, = struct.unpack_from("<H", payload, 1)
                yield {"type": "target_power", "voltage": voltage}
            else:
                yield {"type": "unknown", "id": msg_id, "data": payload}


def main():
    parser = argparse.ArgumentParser(description="Scintillator Probe host tool")
    parser.add_argument("port", help="Serial port (e.g. /dev/ttyACM0)")
    parser.add_argument("--baudrate", type=int, default=115200)
    parser.add_argument("--send-power", type=int, default=None,
                        help="Send a target power command (volts)")
    args = parser.parse_args()

    probe = ScintillatorProbe(args.port, args.baudrate)

    if args.send_power is not None:
        print(f"Sending TargetPower: {args.send_power} V")
        probe.send_target_power(args.send_power)

    print(f"Listening on {args.port}... (Ctrl+C to quit)")
    try:
        while True:
            for msg in probe.read_messages():
                if msg["type"] == "voltage_sense":
                    print(f"  VoltageSense: {msg['voltage']} V")
                else:
                    print(f"  {msg}")
            time.sleep(0.01)
    except KeyboardInterrupt:
        pass
    finally:
        probe.close()


if __name__ == "__main__":
    main()
