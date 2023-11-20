# IntroductionToRobotics

This repository serves as a comprehensive record of my journey through the 'Introduction to Robotics' course during my third year at the Faculty of Mathematics and Computer Science, University of Bucharest. It encompasses a collection of weekly homework assignments, two course projects, and the potential inclusion of personal robotics projects.

# Homework

<details>
  <summary>
      <h2>Homework 1: Controlling an RGB LED using 3 potentiometers</h2>
  </summary>
  <br>
  
  ### Task:
  #### Use a separate potentiometer for controlling each color of the RGB LED: Red,Green, and Blue. This control must leverage digital electronics. Specifically, you need to read the potentiometer’s value with Arduino and then write a mapped value to the LED pins.

 <details>
   <summary>
     <h3>Parts used:
   </summary>
    -Arduino UNO board <br>
    -Breadboard <br>
    -1 RGB LED <br>
    -3 Potentiometers <br>
    -3 220Ω Resistors <br>
    -Wires as needed <br>
   
 </details>

  ### [Code](https://github.com/StefSimi/IntroductionToRobotics/blob/main/HW1_RGB.ino)
  ### [Video Showcase](https://www.youtube.com/shorts/qgwaxqFKWgs)

  
  ### Fritzing Schematic
  ### ![Fritzing schematic](https://github.com/StefSimi/IntroductionToRobotics/assets/98825330/15ec6250-83c8-41b5-8002-4444f1e41fcb)

  ### Real-life setup:
  ### ![Setup](https://github.com/StefSimi/IntroductionToRobotics/assets/98825330/e38f9298-f36c-462c-ab02-1cd9aef39add)
</details>
<br>
<details>
  <summary>
        <h2>Homework 2: Elevator <s>wannabe</s></h2>
  </summary>
  <br>
  
  ### Task:
  #### Design a control system that simulates an elevator using the Arduino platform:
  -<b>LEDs</b>: Every red LED represents a unique floor. When the elevator is on a floor, its LED will light up. There is an additional blue LED that represents the elevator’s operational state. Specifically, it will blink if the elevator is moving, and remain lit up when the elevator is stationary;<br>
  -<b>Buttons</b>: The assignment specified that there need to be 3 buttons that represent call buttons from each floor. However, I implemented the code in such a way so that it supports any number of buttons (the circuit I made has 5 for demonstrational purposes). Debouncing has also been implemented;
  -<b>Buzzer</b>: I added fancy sounds for door closing and opening, as well as elevator music.<br>
  -<b>State Change & Timers</b>:If the elevator is already at the desired floor, pressing the button for that floor has no effect. Otherwise, after a button press, the elevator waits for the doors to close (represented by a sound effect), and then moves to the corresponding floor, by visiting (not stopping) each floor along the way, such as a real life elevator (it does not simply teleport to the desired floor). Additionally, when the elevator receives an input, and then receives another that would take it to a floor that is closer to the elevator than the initial input (but maintains the same direction of movement), it will go to the second input first, and then go to the floor with the initial one. This is demonstrated in the video showcase;

  
  <details>
   <summary>
     <h3>Parts used:
   </summary>
    -Arduino UNO board <br>
    -2 Breadboards <br>
    -6 LEDs <br>
    -6 220Ω Resistors <br>
    -1 Active buzzer <br>
    -1 100Ω Resistor <br>
    -3 Buttons <br>
    -Wires as needed <br>
 </details>


       
  ### [Code](https://github.com/StefSimi/IntroductionToRobotics/blob/main/HW2_Elevator.ino)
  ### [Video](https://www.youtube.com/watch?v=B9fQ2LgqtxA)

  
  ### [Fritzing Schematic](https://github.com/StefSimi/IntroductionToRobotics/blob/main/HW2_Elevator_Fritz.fzz)
  ### ![Fritzing_schematic](https://github.com/StefSimi/IntroductionToRobotics/assets/98825330/903cf807-3c90-4ee5-84ae-f3ab0057d209)

  ### Real-life setup:
  ### ![Setup](https://github.com/StefSimi/IntroductionToRobotics/assets/98825330/43a87237-4806-4fee-b743-656864f8df84)
</details>

<br>
<details>
  <summary>
    <h2>Homework 3: 7 Segment Display drawing</h2>
  </summary>
  <br>

  ### Task:
  #### Use a joystick to control the position of the display and draw on the 7 Segment Display.
  -<b>Joystick</b>: Implementing multi-directional movement detection for the joystick can be challenging. The implementation is rigorously explained in the code;<br>
  -<b>Multi-function button</b>: When the button is pressed for a short time, the segment that we are currently on will toggle between on and off. When the button is pressed for a longer time, all of the segments turn off, and the position of the current segment is set back to DP;<br>
  -<b>Interrupts</b>: Using interrupts to implement debouncing for a button that has multiple functions, depending on how long it is pressed, leads to very ugly code. Furthermore, the interrupt code is not very well optimized for multiple quick button presses, which is exactly what we do with the button in this homework, so it only exists as a proof of concept;<br>
  -<b>Blinking</b>: The current segment should blink, wheter it is lit up or not;
  -<b>Movement</b>: Travelling between segments should feel very fluid. Here is a table detailing the movement from each segment: <br>
  
  ### ![Movement](https://github.com/StefSimi/IntroductionToRobotics/assets/98825330/301387ee-e002-42f1-95ee-a23de47f8b8c)

<details>
   <summary>
     <h3>Parts used:
   </summary>
    -Arduino UNO board <br>
    -1 Breadboard <br>
    -1 common anone 7 Segment Display <br>
    -8 220Ω Resistors <br>
    -1 Joystick <br>
    -Wires as needed <br>
 </details>

  ### [Code](https://github.com/StefSimi/IntroductionToRobotics/blob/main/HW3_7SD.ino)
  ### [Video](https://www.youtube.com/watch?v=xi_0ALrA4u4)

  
  ### [Fritzing Schematic](https://github.com/StefSimi/IntroductionToRobotics/blob/main/HW3_7SD_Fritz.fzz)
  ### ![Fritzing schematic](https://github.com/StefSimi/IntroductionToRobotics/assets/98825330/c57788c7-7c40-43af-95c8-b550cce67666)



  ### Real-life setup:
  ### ![Setup](https://github.com/StefSimi/IntroductionToRobotics/assets/98825330/7353b3ea-74f8-41fd-8b70-ff5566e538a8)

  
</details>

<br>
<details>
  <summary>
    <h2>Homework 4: Stopwatch Timer</h2>
  </summary>
  <br>

  ### Task:
  #### Implement a Stopwatch Timer using a 4 Digit 7 Segment Display
  
  -<b>Buttons</b>: The circuit has 3 buttons: 1 for Starting/ Pausing the stopwatch, 1 for resetting it, and the last one for saving lap times and cycling through them when the stopwatch has just been reset. Aditionally, when the stopwatch is in lap viewing mode and the reset button is pressed, all lap times will be deleted. Debouncing has been implemented for each button;<br>
  -<b>Interrupts</b>: In order to maximise precision, debouncing has been implemented on both the Start/ Pause and Lap buttons. Aditionally, when the timer is paused, the remaining time until the stopwach is incremented again is saved, so there is no lost time;<br>
  -<b>Shift Register</b>: The 4 Digit 7 Segment Display has 12 total pins. If we connect it directly to the Arduino UNO board, it will take up most of its digital pins. However, we can use the 74HC595 Shift Register in order to free more than half of those pins up;<br>
  -<b>Remote Control</b>: I attached an IR sensor to the circuit, so now the stopwatch can be controlled remotely. Aditionally, I added some more features to the remote:<br>
  -By using the fast forward/ backward buttons, you can cycle through your saved laps in both directions;<br>
  -You can use the numbers on the remote to input a starting time when the stopwatch is in reset mode;<br>
  -The 100+ and 200+ buttons can be used to add that amount of time to the starting time when the stopwatch is in reset mode;

<details>
   <summary>
     <h3>Parts used:
   </summary>
    -Arduino UNO board <br>
    -2 Breadboards <br>
    -1 common cathode 4 Digit 7 Segment Display <br>
    -8 220Ω Resistors <br>
    -1 74HC595 Shift Register <br>
    -1 IR Sensor <br>
    -1 Remote <br>
    -Wires as needed <br>
 </details>


  ### [Code](https://github.com/StefSimi/IntroductionToRobotics/blob/main/HW4_Stopwatch.ino)
  ### [Video](https://www.youtube.com/watch?v=4mRAwRTBGtM)

  
  ### [Fritzing Schematic](https://github.com/StefSimi/IntroductionToRobotics/blob/main/HW4_Stopwatch_Fritz.fzz)
  ### ![Fritzing schematic](https://github.com/StefSimi/IntroductionToRobotics/assets/98825330/31fea2db-03b8-4645-9925-44e00c36dd61)



  ### Real-life setup:
  ### ![Setup](https://github.com/StefSimi/IntroductionToRobotics/assets/98825330/bf6fb46a-377d-41a3-9a9c-946564108e40)

  
</details>


<br>
<details>
  <summary>
    <h2>Homework 5: Environment Logger</h2>
  </summary>
  <br>

  ### Task:
  #### Implement a pseudo-smart Environment Monitor and Logger that saves its settings in the Arduino's EEPROM
  
-<b>Serial input</b>: The monitoring system operates through a sophisticated menu that takes input via Serial Communication. Given the menu's complexity, featuring various submenus, precise control variables are essential. Additionally, we must implement effective error-handling procedures to manage instances where user-input values could potentially interfere with our sensors. <br>
-<b>EEPROM</b>: The Arduino's EEPROM has a limited number of write cycles before potential failure. Consequently, careful consideration is necessary when determining what data to write and when to perform these write operations to ensure optimal utilization and longevity of the EEPROM. <br>
-<b>Sensors</b>: The HC-SR04 Ultrasonic Sensor produces seemingly random values. To obtain accurate results, it is essential to carefully process these values, account for the speed of sound, and interpret results only after comprehensive data refinement. <br>

  
  
  

<details>
   <summary>
     <h3>Parts used:
   </summary>
    -Arduino UNO board <br>
    -1 Breadboard <br>
    -RGB LED <br>
    -3 330Ω Resistors <br>
    -1 Photocell<br>
    -1 10KΩ Resistor <br>
    -1 HC-SR04 Ultrasonic Sensor <br>
    -Wires as needed <br>
 </details>


  ### [Code](https://github.com/StefSimi/IntroductionToRobotics/blob/main/HW5_Environment_Logger.ino)
  ### [Video](https://www.youtube.com/watch?v=x8HZ40uMmbk)

  
  ### [Fritzing Schematic](https://github.com/StefSimi/IntroductionToRobotics/blob/main/HW5_Environment_Logger_Fritz.fzz)
  ### ![Fritzing schematic](https://github.com/StefSimi/IntroductionToRobotics/assets/98825330/3bded13a-59be-4b10-8dac-0c7f729eb038)



  ### Real-life setup:
  ### ![Setup](https://github.com/StefSimi/IntroductionToRobotics/assets/98825330/d1f0365a-7e09-4ede-8a69-e9e48112b3f7)

  
</details>
