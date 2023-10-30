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

  
  ### Fritzing Schematic:
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
  #### Design a control system that simulates a 3-floor elevator using the Arduino platform:
  <br>
  -<b>LED Indicators</b>: Each of the 3 LEDs should represent one of the 3 floors. The LED corresponding to the current floor should light up.  Additionally, another LED should represent the elevator’s operational state.  It should blink when the elevator is moving and remain static when stationary;<br>
  -<b>Buttons</b>: Implement 3 buttons that represent the call buttons from the 3 floors. When pressed, the elevator should simulate movement towardsthe floor after a short interval (2-3 seconds);<br>
  -<b>Buzzer</b>: The buzzer should sound briefly during the following scenarios:<br>1. Elevator arriving at the desired floor (something resembling a ”cling”); <br>2. Elevator doors closing and movement;<br>
  -<b>State Change & Timers</b>:If the elevator is already at the desired floor, pressing the button for that floor should have no effect. Otherwise, after a button press, the elevator should "wait for the doors to close" and then "move" to the corresponding floor. If  the  elevator  is  in  movement, it should either do nothing or it should stack its decision (get to the first programmed floor, open the doors, wait, close them and then go to thenext desired floor).<br>
  -<b>Debounce</b>:Remember to implement debounce for the buttons to avoidunintentional repeated button presses.

  #### Personal comments:
  Besides the required tasks, I implemented the code in such a way so that it supports any number of buttons (the circuit I made has 5), I added fancy sounds for door closing and opening, as well as elevator music.<br>
  Additionally, when the elevator receives an input, and then receives another that would take it to a floor that is closer to the elevator than the initial input (but maintains the same direction of movement), it will go to the second input first, and then go to the floor with the initial one. This is demonstrated in the video showcase;

  <details>
   <summary>
     <h3>Parts used:
   </summary>
    -Arduino UNO board <br>
    -2 Breadboards <br>
    -6 LEDs <br>
    -6 220Ω Resistors <br>
    -1 Active buzzer <br>
    -1 100Ω resistors <br>
    -3 Buttons <br>
    -Wires as needed <br>
 </details>


       
  ### [Code](https://github.com/StefSimi/IntroductionToRobotics/blob/main/HW2_Elevator.ino)
  ### [Video](https://www.youtube.com/watch?v=B9fQ2LgqtxA)

  
  ### Fritzing Schematic:
  ### ![Fritzing_schematic](https://github.com/StefSimi/IntroductionToRobotics/assets/98825330/903cf807-3c90-4ee5-84ae-f3ab0057d209)

  ### Real-life setup:
  ### ![Setup](https://github.com/StefSimi/IntroductionToRobotics/assets/98825330/43a87237-4806-4fee-b743-656864f8df84)
</details>


