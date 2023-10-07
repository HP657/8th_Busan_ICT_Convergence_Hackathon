import serial
import time
from gtts import gTTS
from playsound import playsound
from keras.models import load_model
import cv2
import numpy as np

model = load_model(r"keras_model.h5", compile=False) #모델 가져오기

with open(r"labels.txt", "r") as file:
    class_names = [line.strip() for line in file]

ser = serial.Serial('COM6', 9600)  #시리얼 연결 주소

camera = cv2.VideoCapture(1)

previous_time = 0

while True:
    # 현재 시간을 가져옴
    current_time = time.time()
    
    # Grab the web camera's image.
    ret, image = camera.read()

        # Resize the raw image into (224-height,224-width) pixels
    image = cv2.resize(image, (224, 224), interpolation=cv2.INTER_AREA)

        # Show the image in a window
    cv2.imshow("Webcam Image", image)
    
    # 이전에 인식한 시간에서 5분가 지났는지 확인
    if current_time - previous_time >= 10:
        

        # Make the image a numpy array and reshape it to the model's input shape.
        image = np.asarray(image, dtype=np.float32).reshape(1, 224, 224, 3)

        # Normalize the image array
        image = (image / 127.5) - 1

        # Predict the model
        prediction = model.predict(image)
        index = np.argmax(prediction)
        class_name = class_names[index]
        confidence_score = prediction[0][index]

        # Print prediction and confidence score
        print("너의 상태는 : ", class_name , str(np.round(confidence_score * 100))[:-2], "%")
        if class_name[0] == '0':
            message = "안전"
            output_path = r'sample0.mp3'
            class_name = '0'
            ser.write(class_name.encode())
        elif class_name[0] == '1':
            message = "경고"
            output_path = r'sample1.mp3'
            class_name = '1'
            ser.write(class_name.encode())
        else:
            message = "인식 불가"
            output_path = r'sample.mp3'
            ser.write(class_name.encode())
        # TTS로 음성 파일 생성 및 재생
        playsound(output_path)
        
        # 현재 시간을 이전에 인식한 시간으로 업데이트
        previous_time = current_time

    # Listen to the keyboard for presses.
    keyboard_input = cv2.waitKey(1)

    # 27 is the ASCII for the esc key on your keyboard.
    if keyboard_input == ord('q'):  # Press 'q' to exit
        break

camera.release()
cv2.destroyAllWindows()
