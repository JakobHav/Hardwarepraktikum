"""
Exercise 04: Real-Time IMU Visualization

Complete all TODO sections.
"""

from collections import deque

import matplotlib.animation as animation
import matplotlib.pyplot as plt
import serial

# =====================
# TODO (1): Set correct serial port
# =====================
# HINT: Find your board's COM port (Windows) or /dev/ttyUSB* (Linux/Mac)
# Common Windows ports: COM3, COM4, COM5, COM6
PORT = "/dev/ttyACM0"  # TODO: Change to your board's port
BAUD = 115200

WINDOW_SIZE = 100

# =====================
# TODO (2): Initialize serial connection
# =====================
# HINT: Use serial.Serial(PORT, BAUD) to create connection
ser = serial.Serial(PORT, BAUD, timeout=1)

# 15:29:46.351 > ax: -0.02 | ay: 0.05 | az: 1.05 | gyrX: -0.07 | gyrY: -0.21 | gyrZ: 0.35 | Orientation: FACE UP | Accelerometer Gesture: NONE | Gyro Gesture: MOVE_RIGHT


# =====================
# TODO (3): Create buffers for ax, ay, az, gyrX, gyrY, gyrZ
# =====================
# HINT: Use deque(maxlen=WINDOW_SIZE) for each sensor axis
ax_data = deque(maxlen=WINDOW_SIZE)
ay_data = deque(maxlen=WINDOW_SIZE)
az_data = deque(maxlen=WINDOW_SIZE)
gyrX_data = deque(maxlen=WINDOW_SIZE)
gyrY_data = deque(maxlen=WINDOW_SIZE)
gyrZ_data = deque(maxlen=WINDOW_SIZE)


# =====================
# TODO (4): Initialize text variables
# =====================
# HINT: Store orientation and both gesture types
orientation_text = "NONE"
fsm_gesture_text = "NONE"
gyro_gesture_text = "NONE"


# =====================
# TODO (5): Setup plot with dual subplots
# =====================
# HINT: Create 2 vertical subplots, 3 lines per subplot, text displays, and configure axes
# HINT: Accelerometer subplot: y-range [-15, 15], title "Accelerometer Data (m/s²)"
# HINT: Gyroscope subplot: y-range [-500, 500], title "Gyroscope Data (°/s)"
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))

(line_ax,) = ax1.plot([], [], label="ax")
(line_ay,) = ax1.plot([], [], label="ay")
(line_az,) = ax1.plot([], [], label="az")

ax1.set_xlim(0, WINDOW_SIZE)
ax1.set_ylim(-15, 15)
ax1.set_title("Accelerometer Data (m/s²)")
ax1.legend(loc="upper right")

text_display1 = ax1.text(
    0.02, 0.95, "", transform=ax1.transAxes, verticalalignment="top"
)

(line_gyrX,) = ax2.plot([], [], label="gyrX")
(line_gyrY,) = ax2.plot([], [], label="gyrY")
(line_gyrZ,) = ax2.plot([], [], label="gyrZ")

ax2.set_xlim(0, WINDOW_SIZE)
ax2.set_ylim(-500, 500)
ax2.set_title("Gyroscope Data (°/s)")
ax2.legend(loc="upper right")

text_display2 = ax2.text(
    0.02, 0.95, "", transform=ax2.transAxes, verticalalignment="top"
)


# =====================
# TODO (6): Parse serial line
# =====================
# HINT: New serial format from Arduino:
# "ax: X | ay: Y | az: Z | gyrX: X | gyrY: Y | gyrZ: Z | Orientation: ... | FSM: ... | Gyro: ..."
#
def parse_line(line):
    global orientation_text, fsm_gesture_text, gyro_gesture_text

    try:
        # HINT: Serial format: "ax: X | ay: Y | az: Z | gyrX: X | gyrY: Y | gyrZ: Z | Orientation: ... | FSM: ... | Gyro: ..."
        # TODO: Split by '|' and extract 6 sensor values (ax, ay, az, gyrX, gyrY, gyrZ)
        # TODO: Extract 3 text strings (orientation_text, fsm_gesture_text, gyro_gesture_text)
        # TODO: Return tuple (ax_val, ay_val, az_val, gyrX_val, gyrY_val, gyrZ_val)

        metrics = line.split("|")

        fields = {}
        for metric in metrics:
            key, _, value = metric.strip().partition(":")
            fields[key.strip()] = value.strip()

        ax_val = float(fields["ax"])
        ay_val = float(fields["ay"])
        az_val = float(fields["az"])
        gyrX_val = float(fields["gyrX"])
        gyrY_val = float(fields["gyrY"])
        gyrZ_val = float(fields["gyrZ"])

        orientation_text = fields.get("Orientation", orientation_text)
        fsm_gesture_text = fields.get("Accelerometer Gesture", fsm_gesture_text)
        gyro_gesture_text = fields.get("Gyro Gesture", gyro_gesture_text)

        return (ax_val, ay_val, az_val, gyrX_val, gyrY_val, gyrZ_val)

    except Exception:
        return None


# =====================
# TODO (7): Update function
# =====================
def update(frame):
    global orientation_text, fsm_gesture_text, gyro_gesture_text

    # TODO: Read all available serial data, parse, and append to buffers
    while ser.in_waiting:
        raw_line = ser.readline().decode("utf-8", errors="ignore").strip()
        if not raw_line:
            continue

        parsed = parse_line(raw_line)
        if parsed is None:
            continue

        ax_val, ay_val, az_val, gyrX_val, gyrY_val, gyrZ_val = parsed
        ax_data.append(ax_val)
        ay_data.append(ay_val)
        az_data.append(az_val)
        gyrX_data.append(gyrX_val)
        gyrY_data.append(gyrY_val)
        gyrZ_data.append(gyrZ_val)

    # TODO: Update all 6 line objects (3 accel + 3 gyro)
    line_ax.set_data(range(len(ax_data)), ax_data)
    line_ay.set_data(range(len(ay_data)), ay_data)
    line_az.set_data(range(len(az_data)), az_data)

    line_gyrX.set_data(range(len(gyrX_data)), gyrX_data)
    line_gyrY.set_data(range(len(gyrY_data)), gyrY_data)
    line_gyrZ.set_data(range(len(gyrZ_data)), gyrZ_data)

    # TODO: Update both text displays
    text_display1.set_text(
        f"Orientation: {orientation_text}\nGesture: {fsm_gesture_text}"
    )
    text_display2.set_text(f"Gyro Gesture: {gyro_gesture_text}")

    # TODO: Return all plot objects to redraw
    return [
        line_ax,
        line_ay,
        line_az,
        line_gyrX,
        line_gyrY,
        line_gyrZ,
        text_display1,
        text_display2,
    ]


# =====================
# TODO (8): Create animation
# =====================
# TODO: Create animation with FuncAnimation(fig, update, interval=50)
# TODO: Call plt.tight_layout() for proper spacing
ani = animation.FuncAnimation(fig, update, interval=50, blit=False)
plt.tight_layout()
plt.show()
