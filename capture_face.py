import cv2
import os
import pyttsx3
from time import sleep
import keyboard

# ----------------------
# CONFIG
# ----------------------
KNOWN_DIR = "known_faces"
FACE_SIZE = (200, 200)
CAPTURE_COUNT = 10  # images per person 
                    # recommended to capture more face images (20 to 30) for better face detection accuracy
os.makedirs(KNOWN_DIR, exist_ok=True)

# ----------------------
# SETUP SPEECH ENGINE
# ----------------------
engine = pyttsx3.init()
engine.setProperty("rate", 150)

def speak(text):
    engine.say(text)
    engine.runAndWait()

# ----------------------
# CAPTURE NEW USER FACES
# ----------------------
print("\n##### Python program for capturing face images for training #####")
person_name = input("\nEnter your name: ").strip()
person_folder = os.path.join(KNOWN_DIR, person_name)
os.makedirs(person_folder, exist_ok=True)


cap = cv2.VideoCapture(0) # 0 for in-built web cam, 1 for external web cam

count = 0 # to store number of image captured

print(f"Capturing images for {person_name}.\nPress 'ctrl c' to terminate program.")

face_cascade = cv2.CascadeClassifier(
    cv2.data.haarcascades + "haarcascade_frontalface_default.xml"
)

while count < CAPTURE_COUNT:
    ret, frame = cap.read()
    cv2.imshow("Capture Faces", frame)

    if not ret:
        break

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    faces = face_cascade.detectMultiScale(gray, 1.3, 5)

    for (x, y, w, h) in faces:
        face_img = gray[y:y+h, x:x+w]
        face_img = cv2.resize(face_img, FACE_SIZE)
        path = os.path.join(person_folder, f"{count}.jpg")
        cv2.imwrite(path, face_img)
        print("Saved:", path)
        speak("Face image captured")

        # cv2.imshow("Capture Faces", frame)
        sleep(0.3) # Brief 1 second delay to view images during capture
        keyboard.wait("enter") # press the enter key to capture various positions of the face image as you change direction
        count += 1

    # if cv2.waitKey(1) & 0xFF == ord("q"):
    #     break

cap.release()
cv2.destroyAllWindows()
print("\nImage Capture Complete, Program Terminated !!")