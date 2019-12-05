// SOME BACKGROUND since you probably should understand the sensors somewhat first
//There are echo sensors that send a signal with a trigger (all the variables w a T) and receive the reflected signal (all the E's)
//There are infrared sensors which only have an input, and it is denoted w IR
int E_FRONT_LEFT = 9; 
int T_FRONT_LEFT = 8;

int E_FRONT_RIGHT= 12;
int T_FRONT_RIGHT = 13;




int IR_MID_LEFT = A2;
int IR_MID_RIGHT = A3;
int IR_BACK = A4;

//So above is me declaring all of the pins for the ultrasonic sensor (trigger and echo thing), and the infra red sensor. The variable names denote the position of the sensor the the bot
//Below is pretty much the same thing for the two motors, except the motors have to pins, one to reverse and one to go forward
//another background thing is that the pins the motors are connected to allow us to give a range of voltages as opposed to on or off (PWM pins) 

int motor_LEFT_FORWARD = 2; 
int motor_LEFT_REVERSE = 3;
int motor_RIGHT_FORWARD = 10;
int motor_RIGHT_REVERSE = 11;
int motor_LEFT_SPEED = 5;
int motor_RIGHT_SPEED = 6;


//These velocity variables simply tell each motor how fast to go (negative values imply going in reverse), and it goes from -1 to 1
// these are just some constants that I use a lot, and the duration variable is used to decipher the sensor values
int  max_distance = 75, lock_on_distance = 20, attack_distance = 10, duration; 


//So there are two IR sensors in the front of our robot, 2 in the middle and one in the back, we do this in case our robot is pushing the opponent off stage, 
//we dont want our robot to reverse if it sees a line
//So in essence, when the robot is attacking we ignore the front two sensors and use the middle two, otherwise we do the opposite (the back sensor is always used though)
//This boolean simply tells us which pair on sesnors we are using
//false IMPLIES WE WILL USE THE FRONT SENSORS, true MEANS MIDDLE SENSORS

//so this pretty much takes all of the ultrasonic sensors and deciphers them, and puts all the distances in an array
//--YOUSEF--I changed the return type, and the type of the DISTANCE var, because you want a pointer to an array, but you are trying to put that into an integer
int ECHO[2]={E_FRONT_LEFT, E_FRONT_RIGHT};
int TRIG[2]={T_FRONT_LEFT, T_FRONT_RIGHT};
int IR[5] = {IR_MID_LEFT, IR_MID_RIGHT, IR_BACK};
int MOTOR[6] = {motor_LEFT_FORWARD, motor_LEFT_REVERSE, motor_RIGHT_FORWARD, motor_RIGHT_REVERSE, motor_LEFT_SPEED, motor_RIGHT_SPEED};
float VELOCITY[2] = {0,0};

int* ECHO_DECIPHER(int*,int*,int*);
bool* INFR_DECIPHER(int*, bool*);

int ATTACK(float DISTANCE_ARRAY[2]);
int LOCK_ON(float DISTANCE_ARRAY[2]);
int PATROL(float DISTANCE_ARRAY[2]);
int DEFEND(bool DETECTION_ARRAY[3]);
void SET_VELOCITY();

float DISTANCE[4] = {0,0};
bool DETECTION[5] ={LOW,LOW,LOW};
 //this does something similar but all the values are boolean (basically it sees a white line or not)

//Now to be able to manipulate and access all of these variables more effeciently (and elegantly), i've put pretty much everything into an array


void setup() {
//below im simply setting all of our outputs and inputs to their respective variables
    pinMode(E_FRONT_LEFT, INPUT);
    pinMode(E_FRONT_RIGHT, INPUT);

    pinMode(IR_MID_LEFT, INPUT);
    pinMode(IR_MID_RIGHT, INPUT);
    pinMode(IR_BACK, INPUT);
    
    pinMode(T_FRONT_LEFT, OUTPUT);
    pinMode(T_FRONT_RIGHT, OUTPUT);
    
    pinMode(motor_LEFT_FORWARD, OUTPUT);
    pinMode(motor_LEFT_REVERSE, OUTPUT);
    pinMode(motor_RIGHT_FORWARD, OUTPUT);
    pinMode(motor_RIGHT_REVERSE, OUTPUT);
    pinMode(motor_LEFT_SPEED, OUTPUT);
    pinMode(motor_RIGHT_SPEED, OUTPUT);
    Serial.begin(9600);
    //ECHO_DECIPHER(ECHO, TRIG, DISTANCE);
    INFR_DECIPHER(IR, DETECTION);

    //delay(5000); //so now we have all the initial values set and we have stored the positioning of the other robot, this all happens as soon as the robot turns on, afterwards the rules say we must wait 5 seconds before starting
}

void loop() {
  //--YOUSEF-- you aren't passing in all of the arguments for the function, I'm not sure where the rest come from so IDK what to put in here
  MODE_SELECT(DETECTION, DISTANCE);//this function is pretty much the brains of the whole operation, and it decides when to attack, defend etc
  //--YOUSEF-- when calling the functions, you don't need to put the type so i just removed the ints before all of your arrays, and their sizes
  //--YOUSEF-- this might be wrong but I don't think you wanted to pass in the last element of each array
  //ECHO_DECIPHER(ECHO, TRIG, DISTANCE);//after it decides what to do, it will take new data with the same functions we used before and changes them
 INFR_DECIPHER(IR, DETECTION);
}
int* ECHO_DECIPHER(int ECHO_ARRAY[2], int TRIG_ARRAY[2], float DISTANCE_ARRAY[2]){//So this function is pretty much just taking readings and storing them in an array

  //--YOUSEF-- I'm assuming these calculations are correct, I get what ur doing I just don't know the numbers lol
  //--YOUSEF-- IDK if this makes sense, but since you are using sound waves you might run into doppler effect type problems, especially since
  //--YOUSEF-- You are tracking a moving target, like if it is moving towards you it may seem closer, or moving away it may seem further. 
  //--YOUSEF-- Probably doesn't matter because distance is so close but something to keep in mind maybe.
  for(int i =0; i<2; i++){//using loop to access all array
    digitalWrite(TRIG_ARRAY[i], LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_ARRAY[i], HIGH);//sends a signal
    delayMicroseconds(10);//waits
    digitalWrite(TRIG_ARRAY[i], LOW);//turns off signal
    duration = pulseIn(ECHO_ARRAY[i], HIGH);//reads signal
    DISTANCE_ARRAY[i]= duration*0.017;//converts signal to distance
  }
} 
bool* INFR_DECIPHER(int IR_ARRAY[3], bool DETECTION_ARRAY[3]){
  //true IMPLIES WHITE LINE DETECTED
  for(int i = 0; i<3; i++){//similar to above func but has 5 values and they are boolean
    //--YOUSEF--a little more eleagant way to put this
    DETECTION_ARRAY[i] = digitalRead(IR_ARRAY[i]) == LOW;
  }
}

//--YOUSEF-- when you call this you only pass in the first two arguments, but not the rest, IDK if i should remove or not,
//--YOUSEF-- you could either take these out and just use the global variables by name (i think that is correct), or pass the global variables in as arguments.
int MODE_SELECT(bool DETECTION_ARRAY[5], float DISTANCE_ARRAY[4]){
    int minimum = 1000;
    //--YOUSEF-- Most of this looks good, one big problem though is that you are passing in IR_DorA into each of the functions,
    //--YOUSEF-- because you want the attack functions to set its value for the DEFEND function, but when you edit it in each of the functions,
    //--YOUSEF-- it doesn't change its value outside the function, you should make a global variable that holds IR_DorA that each function can edit
    //--YOUSEF-- also "offense" is spelt offence dumb nigga
    for(int i = 0; i<2; i++){
      if(DISTANCE_ARRAY[i]<minimum){
        minimum=DISTANCE_ARRAY[i];//If we dont detect anything near we go into patrol mode
      }
    }
      //--YOUSEF-- I changed the 2nd argument of the else if below because if DISTANCE == lock_on_distance,
      //--YOUSEF-- it would go straight to attack which is not what I think you want to happen (changed < to <=)
      if(minimum>lock_on_distance){
        PATROL(DISTANCE_ARRAY);
        
  
      }
      else if(minimum<=lock_on_distance && minimum >= attack_distance){//If we see a robot in range, we will try to lock on using the lock on function
        LOCK_ON(DISTANCE_ARRAY);
      }
      else if(minimum<attack_distance){
        ATTACK(DISTANCE_ARRAY);//if the robot is really close we go for the attack function
      }
      for(int i = 0; i<3; i++){
        if(DETECTION_ARRAY[i]==true){//after we check the offensive options we will check of we are near the edge of the ring
           DEFEND(DETECTION_ARRAY);//if we are we have to invoke the defense function
        }
     }
     
 
     return 0;
  }


int ATTACK(float DISTANCE_ARRAY[2]){
  int target, minimum = 1000;
  float target_ARRAY[2];
  for(int i = 0; i<2; i++){
    if(DISTANCE_ARRAY[i]<attack_distance){ //Checks which side the opponent is withing attack distance of, and stores those values in the target array
      target_ARRAY[i]= DISTANCE_ARRAY[i]; //if the side is within attack range, we store this distance in the target_array
    }
    else{
      target_ARRAY[i] = 0;//if a side is not within range its target values is zero
    }
  }
  for(int i = 0; i<2;i++){
    if(target_ARRAY[i]<minimum && target_ARRAY[i]!=0){
      minimum = target_ARRAY[i];
      target = i; //finding which sensor is closest to the opponent
      
    }
  }



  /*--YOUSEF--
    In the infallible words of Terry A Davis:
    "
    The most powerful statement in the c language is the switch statement ...
    The best statement in the C language is the switch statement...
    The CIA wants you to think its nothing more than if then else,
    the CIA wants you to think its the same as if else, its not.
    "
    all this to say, good use of the switch statement
  */
  switch(target){//we will move depending on which sensor is closest to the robot
    case 0://FRONT LEFT
    if(target_ARRAY[1]==0){//if only the left front sensor sees something then will move forward and to the left
      VELOCITY[0] = 0.75;
      VELOCITY[1] = 1;
    }
    else{//if both front sensors see something then it will move left proportional the the ratio of thetwo distances (will move slighly left in this case
      VELOCITY[0] = (target_ARRAY[0]/target_ARRAY[1]);
      VELOCITY[1] = 1;
    }
    break;
   case 1:
   if(target_ARRAY[0]==0){//similar except to the right
      VELOCITY[0] = 1;
      VELOCITY[1] = 0.75;
    }
   else{
      VELOCITY[0] = 1;
      VELOCITY[1] = (target_ARRAY[1]/target_ARRAY[0]);
    }
    break;
  SET_VELOCITY(); //use our set velocity function to change the velocity based on what the switch case says
  //--YOUSEF-- as mentioned above, this won't change its value outside of this function
   //also telling our robot that we are in attack mode and to use the middle two IR sensors
}
}


int LOCK_ON(float DISTANCE_ARRAY[2]){ 
 
  // This function is exactly the same as the above one excpt it will move slower towards the target
  int target, target_ARRAY[2], minimum = 10000;
  for(int i = 0; i<2; i++){
    if(DISTANCE_ARRAY[i]<lock_on_distance){ 
      target_ARRAY[i]= DISTANCE_ARRAY[i];
    }
    else{
      target_ARRAY[i] = 0;
    }
  }
    for(int i = 0; i<2;i++){
    if(target_ARRAY[i]<minimum && target_ARRAY[i]!=0){
      minimum = target_ARRAY[i];
      target = i;
    }
  }
  switch(target){
    case 0://FRONT LEFT
    if(target_ARRAY[1]==0){
      VELOCITY[0] = 0.5;
      VELOCITY[1] = 0.75;
      
    }
    else{
      VELOCITY[0] = (target_ARRAY[0]/target_ARRAY[1])*0.75;
      VELOCITY[1] = 0.75;
    }
    break;
   case 1:
     if(target_ARRAY[0]==0){
        VELOCITY[0] = 0.75;
        VELOCITY[1] = 0.5;
      }
     else{
        VELOCITY[0] = 0.75;
        VELOCITY[1] = (target_ARRAY[1]/target_ARRAY[0])*0.75;
      }
    break;

  SET_VELOCITY();
}
}
int PATROL(float DISTANCE_ARRAY[2]){
  //similarly this moves even slower except it has an extra case where it turns around if it detects nothing
  int target = 2, target_ARRAY[2], minimum = 10000;
  for(int i = 0; i<2; i++){
    if(DISTANCE_ARRAY[i]<max_distance){ //finds the position of the target, and creates array wish is easier to use the velocity function with, it also tells our robot we are no longer in attack mode at the bottom
      target_ARRAY[i]= DISTANCE_ARRAY[i];
    }
    else{
      target_ARRAY[i] = 0;
    }
  }
  for(int i = 0; i<2;i++){
    if(target_ARRAY[i]<minimum && target_ARRAY[i]!=0){
      minimum = target_ARRAY[i];
      target = i;
    }
  }
  switch(target){
    case 0://FRONT LEFT
    if(target_ARRAY[1]==0){
      VELOCITY[0] = 0.3;
      VELOCITY[1] = 0.5;
    }
    else{
      VELOCITY[0] = (target_ARRAY[0]/target_ARRAY[1])*0.5;
      VELOCITY[1] = 0.5;
    }
    break;
   case 1:
   if(target_ARRAY[0]==0){
      VELOCITY[0] = 0.5;
      VELOCITY[1] = 0.3;
    }
   else{
      VELOCITY[0] = 0.5;
      VELOCITY[1] = (target_ARRAY[1]/target_ARRAY[0])*0.5;
    }
    break;
   case 2:
      VELOCITY[0] = -0.5;
      VELOCITY[1] = 0.5;
      break;
  }
  SET_VELOCITY();
}
int DEFEND(bool DETECTION_ARRAY[3]){//Only gets invoked if we are near a white line

 
  
  if(DETECTION_ARRAY[0] == true && DETECTION_ARRAY[2] == true){ //if the front left sensor and the back sensor see a line, we will move forward and to the right
      VELOCITY[0] = 1;
      VELOCITY[1] = 0.8;
  }
  else if(DETECTION_ARRAY[1]== true &&DETECTION_ARRAY[2] == true){//similar except to the right
      VELOCITY[0] = 0.8;
      VELOCITY[1] = 1;
  }
  else if(DETECTION_ARRAY[0] == true && DETECTION_ARRAY[1] == true){
      VELOCITY[0] = -1;
      VELOCITY[1] = -1;
  }
  else if(DETECTION_ARRAY[0] == true){//if only the left sensor sees something, we will turn back and to the right so the side sensor can hopefully see something
      VELOCITY[0] = -1;
      VELOCITY[1] = -0.25;
  }
  else if(DETECTION_ARRAY[1] == true){//similar expect to the left
      VELOCITY[0] = -0.25;
      VELOCITY[1] = -1;
  }
  else if(DETECTION_ARRAY[2] == true){//if our back is to the line then we must drive forward
      VELOCITY[0] = 1;
      VELOCITY[1] = 1;
  }

  Serial.print(VELOCITY[0]);
  Serial.print(" ");
  Serial.println(VELOCITY[1]);
  SET_VELOCITY();
  //delay a little to give our robot time to complete this motion
}
void SET_VELOCITY(){

   if(VELOCITY[0]<0){
    digitalWrite(MOTOR[0],HIGH);
    digitalWrite(MOTOR[1], LOW);
    analogWrite(MOTOR[4], int(abs(VELOCITY[0])*255));
  }
  else{
    digitalWrite(MOTOR[1],HIGH);
    digitalWrite(MOTOR[0], LOW);
    analogWrite(MOTOR[4],int(VELOCITY[0]*255));
  }

  if(VELOCITY[1]<0){
    digitalWrite(MOTOR[2],HIGH);
    digitalWrite(MOTOR[3], LOW);
    analogWrite(MOTOR[5], int(abs(VELOCITY[1])*255));
  }
  else{
    digitalWrite(MOTOR[3],HIGH);
    digitalWrite(MOTOR[2], LOW);
    analogWrite(MOTOR[5],int(VELOCITY[1]*255));
  }
}
