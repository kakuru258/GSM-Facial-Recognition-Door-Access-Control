import cv2
import os
import pandas as pd
from datetime import datetime
import numpy as np
from win32com.client import Dispatch

import serial
import sys
import time

import keyboard

# =========================  CONFIG  ========================= #
SERIAL_PORT   = "COM12"   # Arduino COM Port
BAUD_RATE     =  9600     # Baudrate for serial communication

# Open serial port once at startup
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"✔ Connected to Arduino on {SERIAL_PORT} at {BAUD_RATE} baud")
    time.sleep(2)  # Gives Arduino time to reset after port open
except Exception as e:
    print(f"✖ Failed to open {SERIAL_PORT}: {e}")
    print("Check port name, cable, and that the Arduino is plugged in.")
    sys.exit(1)

# ---------------------- CONFIG ---------------------- #
KNOWN_DIR = "known_faces"
EXCEL_FILE = "room_attendance.xlsx"
FACE_SIZE = (200, 200)

# ---------------------- SPEECH FUNCTION ---------------------- #
def speak(text):
    speaker = Dispatch(("SAPI.SpVoice"))
    speaker.Speak(text)

# --------------------- LOAD TRAINING DATA ---------------------#
faces = []
labels = []
label_map = {}
current_label = 0

for person in os.listdir(KNOWN_DIR):
    folder = os.path.join(KNOWN_DIR, person)
    label_map[current_label] = person
    for img_name in os.listdir(folder):
        img_path = os.path.join(folder, img_name)
        img = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
        if img is None:
            continue
        img = cv2.resize(img, FACE_SIZE)
        img = cv2.equalizeHist(img)  # improves image contrast
        faces.append(img)
        labels.append(current_label)
    current_label += 1

if len(faces) == 0:
    raise ValueError("No training faces found. Run capture_faces.py first!")

faces = np.array(faces)
labels = np.array(labels)
print("\n##### Program for Room Access Facial Recognition and Attendance #####")
print("\nTraining face-image dataset loaded......")

# ---------------------- TRAIN MODEL ---------------------- #
model = cv2.face.LBPHFaceRecognizer_create()
model.train(faces, labels)

# ----------------------------------------------
# CREATE EXCEL FILE IF DOESN'T ALREADY EXIST
# ----------------------------------------------
if not os.path.exists(EXCEL_FILE):
    df = pd.DataFrame(columns=["Name", "Date", "Time", "Status"])
    df.to_excel(EXCEL_FILE, index=False)

# ---------------------- ATTENDANCE LOGGER ---------------------- #
logged_people = set() # creates a set data structure to store known and unknown names

def log_attendance(name, status):

    logged_people.add(name)
    now = datetime.now()
    date = now.strftime("%Y-%m-%d")
    time = now.strftime("%I:%M:%S %p") # Converted to 12-hour format with AM/PM

    df = pd.read_excel(EXCEL_FILE)
    df = pd.concat([df, pd.DataFrame([{
        "Name": name,
        "Date": date,
        "Time": time,
        "Status": status
    }])], ignore_index=True)
    df.to_excel(EXCEL_FILE, index=False)

    # Speak attendance
    if name == "Unknown":
        speak("Unknown face detected")

        # arduino command section
        command = "unknown"
        message = f"sent:,{date}|{time}({command})" 

        send_command = message.lower() + "\n"
        ser.write(send_command.encode('utf-8'))
        print(f"sent to arduino MCU => {send_command.strip()}")

    else:
        speak(f"Room access attendance for {name} recorded")
        
        # arduino command section
        command = "open"
        message = f"sent:{name},{date}|{time}({command})" 
        # message = "sent:" + name + "," + date +"|" + time + "(" + command + ")"
        send_command = message.lower() + "\n"
        ser.write(send_command.encode('utf-8'))
        print(f"sent to arduino MCU => {send_command.strip()}")

# ---------------- REAL-TIME FACE RECOGNITION ---------------- #
face_cascade = cv2.CascadeClassifier(
    cv2.data.haarcascades + "haarcascade_frontalface_default.xml"
)

cap = cv2.VideoCapture(0) # 0 -> opens inbuilt web cam for image capture
print("Face recognition started")

while True:  
    ret, frame = cap.read()

    ### Following below draws a frame dashboard with title: Room Attendance Dashboard ###
    overlay = frame.copy()
    cv2.rectangle(overlay, (0,0), 
                (frame.shape[1],50),
                (0,0,0), -1
    )  # full width
    alpha = 0.5
    cv2.addWeighted(overlay, alpha, frame, 1-alpha, 0, frame)
    cv2.putText(frame, "Room Attendance Dashboard", (10,35),
                cv2.FONT_HERSHEY_SIMPLEX, 1, (255,255,255), 2
    )
    ##################################################################

    if not ret:
        break

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    faces_detected = face_cascade.detectMultiScale(gray, 1.3, 5)

    for (x, y, w, h) in faces_detected:
        face_img = gray[y:y+h, x:x+w]
        face_img = cv2.resize(face_img, FACE_SIZE)
        label, confidence = model.predict(face_img)

        # Threshold for known/unknown
        if confidence < 80:
            name = label_map[label]         
            # following below logs data only when enter key is pressed
            if keyboard.is_pressed("enter"):
                log_attendance(name, "Image Present")
        else:
            name = "Unknown"
            # following below logs data only when enter key is pressed
            if keyboard.is_pressed("enter"): 
                log_attendance(name, "Image not present")

        cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
        cv2.putText(frame, name, (x, y-10),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 0), 2)

    cv2.imshow("Attendance System", frame)
    if cv2.waitKey(1) & 0xFF == ord("q"):
        print("\nProgram Terminated...")
        break

cap.release()
cv2.destroyAllWindows()

if 'ser' in locals() and ser and ser.is_open:
    ser.close()
    print("Serial port closed.")


