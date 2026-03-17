#!/usr/bin/env python3
"""Scintillator Probe Control App — PyQt5 GUI."""

import collections
import struct
import sys

import numpy as np
import pyqtgraph as pg
import serial
from cobs import cobs
from PyQt5.QtCore import QTimer
from PyQt5.QtWidgets import (
    QApplication,
    QHBoxLayout,
    QLabel,
    QMessageBox,
    QPushButton,
    QSpinBox,
    QVBoxLayout,
    QWidget,
)

# ---------------------------------------------------------------------------
# Serial protocol (copied from firmware/tools/host_serial.py)
# ---------------------------------------------------------------------------

MSG_TARGET_POWER = 0x01
MSG_VOLTAGE_SENSE = 0x02

SERIAL_PORT = "/dev/ttyACM1"


class ScintillatorProbe:
    def __init__(self, port: str, baudrate: int = 115200, timeout: float = 0.1):
        self.ser = serial.Serial(port, baudrate=baudrate, timeout=timeout)
        self._rx_buf = bytearray()

    def close(self):
        self.ser.close()

    def send_target_power(self, voltage: int):
        payload = struct.pack("<BH", MSG_TARGET_POWER, voltage)
        frame = cobs.encode(payload) + b"\x00"
        self.ser.write(frame)

    def read_messages(self):
        data = self.ser.read(256)
        if data:
            self._rx_buf.extend(data)

        while b"\x00" in self._rx_buf:
            idx = self._rx_buf.index(b"\x00")
            frame = bytes(self._rx_buf[:idx])
            self._rx_buf = self._rx_buf[idx + 1 :]

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
                (voltage,) = struct.unpack_from("<H", payload, 1)
                yield {"type": "voltage_sense", "voltage": voltage}
            elif msg_id == MSG_TARGET_POWER and len(payload) >= 3:
                (voltage,) = struct.unpack_from("<H", payload, 1)
                yield {"type": "target_power", "voltage": voltage}
            else:
                yield {"type": "unknown", "id": msg_id, "data": payload}


# ---------------------------------------------------------------------------
# GUI
# ---------------------------------------------------------------------------


class MainWindow(QWidget):
    def __init__(self, probe: ScintillatorProbe):
        super().__init__()
        self.probe = probe
        self.setWindowTitle("Scintillator Probe")

        # --- Target power row ---
        power_label = QLabel("Target Power:")
        self.power_spin = QSpinBox()
        self.power_spin.setRange(0, 65535)
        self.send_btn = QPushButton("Send")
        self.send_btn.clicked.connect(self._send_power)

        power_row = QHBoxLayout()
        power_row.addWidget(power_label)
        power_row.addWidget(self.power_spin)
        power_row.addWidget(self.send_btn)

        # --- Voltage sense chart ---
        self.history = collections.deque(maxlen=2000)
        self.plot_widget = pg.PlotWidget(title="Voltage Sense")
        self.plot_widget.setLabel("left", "Voltage")
        self.plot_widget.setLabel("bottom", "Sample")
        self.curve = self.plot_widget.plot(pen="y")

        # --- Main layout ---
        layout = QVBoxLayout()
        layout.addLayout(power_row)
        layout.addWidget(self.plot_widget)
        self.setLayout(layout)

        # --- Poll timer ---
        self.timer = QTimer(self)
        self.timer.timeout.connect(self._poll)
        self.timer.start(100)

    def _send_power(self):
        self.probe.send_target_power(self.power_spin.value())

    def _poll(self):
        updated = False
        for msg in self.probe.read_messages():
            if msg["type"] == "voltage_sense":
                self.history.append(msg["voltage"])
                updated = True
        if updated:
            self.curve.setData(np.array(self.history))

    def closeEvent(self, event):
        self.timer.stop()
        self.probe.close()
        event.accept()


def main():
    app = QApplication(sys.argv)

    try:
        probe = ScintillatorProbe(SERIAL_PORT)
    except serial.SerialException as e:
        QMessageBox.critical(None, "Serial Error", f"Cannot open {SERIAL_PORT}:\n{e}")
        sys.exit(1)

    window = MainWindow(probe)
    window.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
