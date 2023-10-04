# Intel8051-Final-Project
A simple music player.

---

## Preview
![image](https://github.com/Ting-Xiao-Miaw/Intel8051-Final-Project/blob/main/.resource/8051.jpg)

## Demonstration
[Demonstration Video](https://youtube.com/shorts/D3YwLQlhVGg?feature=share)

## Enviroment
* Language: C
* Enviroment: 8051 OCD ICE

---

## Control
* The keyboard "0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B" is correpond to "DO, DO#, RE, RE#, MI, FA, FA#, SO, SO#, LA, LA#, SI".
* The button "C" increases the pitch by one octave. There are three levels: low, medium, and high. After reaching the highest level, it will revert back to the low pitch.
* The button "D" increases the pitch by a semitone. After reaching the highest point, it will decrease by one semitone.
* The button "E" start to record the tone you played. Press it again will end the record.
* The button "F" play the tone store in the record.

## Display
* The first digit in the 7-segment LED displays the current octave level.
* The second digit in the 7-segment LED displays the pitch currently playing.
* The third digit in the 7-segment LED displays the displacement of the semitone.
* The fourth digit in the 7-segment LED displays the mode of the music player. (normal -- 0, recording -- 1)
