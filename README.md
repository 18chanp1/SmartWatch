# HeartBreaker
Project description: our project builds an internet-connected fitness tracker that will detect a person’s workout intensity while they are running. 

- The device will give guidance to the user when they are underexerting or overexerting, and provide information about their workout through buzzer.
- The tracker communicates with the server and database through Wifi connection.                     
- All data, such as speed or current position, sent by the device will be stored in the database and shown on a web page.                      
- The device will be powered by a 12V battery pack (rechargeable).
- Targer users: Health gurus, people who wants to track their progress during a workout, people who want some extra motivation while working out

Contributions:
- Web interface 
  - Login and registration for users, session information for each user
  - Summary of all workout statistics
  - Upload workout routes that can be selected on the DE1 (Uploads)
  - View all current completed workouts (Workouts) and statistics
  - Share and view other users’ workouts on interactive map
  - HTTPS

- Server
  - Implementing a hashing function to encrypt users’ password
  - Interact / provide data  from MongoDB database to the DE1 through HTTP endpoints
  
- Wifi Module
  - Obtain data from the web server through http endpoints
  - Decode data and send data over UART to DE1
  - Convert pixel data to binary
  - Send completed workout data back to the server
  
- Touchscreen
  - User interface, used to collect user's input for the FSM on the VGA

- VGA
  - Obtain binary information over UART from Wi-Fi module
  - Decode and display map image
  - Control a FSM to switch between different views on the touchscreen to be displayed

- Accelerometer
  - Perform mathematical computations on acceleration data
  - Provide distance, speed input information about the workouts
  - Transfer workout data to DE1 over UART



