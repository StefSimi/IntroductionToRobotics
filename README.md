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

  
  ### [Fritzing Schematic](https://github.com/StefSimi/IntroductionToRobotics/blob/main/HW1_RGB.ino)
  ### ![Fritzing_schematic](https://github.com/StefSimi/IntroductionToRobotics/assets/98825330/903cf807-3c90-4ee5-84ae-f3ab0057d209)

  ### Real-life setup:
  ### ![Setup](https://github.com/StefSimi/IntroductionToRobotics/assets/98825330/43a87237-4806-4fee-b743-656864f8df84)
</details>


