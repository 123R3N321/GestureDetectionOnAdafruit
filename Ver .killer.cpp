#include <SPI.h>
#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_CircuitPlayground.h>

int brightVal = 2;  //brightness setting
int PixelVal = -1;  //deciding which light to turn on
int seq[1000] = {-1};  //a voting system, num -1 to 9, -1 all off, 9 all on
char state = 'f'; //current state determined by vote of sequence 100
char mode = 'n';  //mode: n-normal, s-sequential
int timer = 0;  //very dumb timer

//unit: meters per second squared
volatile float X, Y, Z; //3-dimensional data
int red = 0;  int green = 0;  int blue = 0;

// //below are large amounts of raw data for "man-powered machine learning"
float rawDataX[16] = {0};  //initialize data set of 16 measurements of X, Y and Z
float rawDataY[16] = {0};  
float rawDataZ[16] = {0};  
  
float filterData(float data, float arr[10]){
float avg = 0;
    for(int i=0; i<16; i++){
      avg += arr[i];
      arr[i-1] = arr[i];
    }
    avg += data;
    avg/=16;

    arr[15] = data;
    return avg;
}

void seqUpdate(){
  int data = -1;

  if(abs(X)>3 || abs(Y)>3){ //only when fruit not laying flat
    if(X<0 && abs(Y)<3){  //tilted s
      Serial.print("down  ");
      data=0;
      if(Y<-0.8){
      Serial.print("right ");
      data=1;
      }
      if(Y>0.8){
        Serial.print("left  ");
        data=9;

      }
      Serial.println();
    }


    if(Y<0 && abs(X)<3){  //tilted d
      Serial.println("right");
                            data=2;
        if(X<-0.8){//s a little
        data = 1;

        }
        if(X>0.8){//w
          data = 3;
        }


    }


    if(X>0 && abs(Y)<3){  //tilted w
      Serial.print("up  ");
            data=5;

      if(Y<-0.8){
      Serial.print("right ");
      data=4;
      }
      if(Y>1.2){
        Serial.print("left  ");
        data=6;

      }
      Serial.println();
    }


    if(Y>0 && abs(X)<3){  //tilted a
      Serial.println("left");
                            data=7;
        if(X>0.8){//w a little
          data=6;
        }
        if(X<-0.8){
          data=8;
        }

    }
  }else{
  if(Z>0){
  Serial.println("flat");
  data=-1;
  }else{
  Serial.println("flipped flat");
  data = -1;
  }


}



  seq[(timer)%999]=seq[((timer)%999)+1];  //better sequential update
    seq[999] = data;
}

int avg(int lower, int higher){ //inc lower, exc higher
  int sum=0;
  for (int i =lower; i<higher;i++){
    sum+=seq[i];
  }
  return sum/(higher-lower);
}
void stateUpdate(){ //3 man council
int seq1 = avg(0,333);
int seq2 = avg(333,666);
int seq3 = avg(666,999);
// Serial.print(seq1);
// Serial.print('  ');
// Serial.print(seq2);
// Serial.print('  ');
// Serial.print(seq3);
// Serial.print('  ');
if(seq1<2 && seq2 <=6 && seq3 >=6){
  state = 'A';  //clockwise sequence
}else if(seq1>=6 && seq2 >=6 && seq3 <2){
  state = 'B';  //anticlockwise
}else if(seq1<2 && seq2 >=6 && seq3 <2){
  state = 'C';  //left-right-left sequence
}else if(seq1>=6 && seq2 <2 && seq3 >=6){
  state = 'D';  //right-left-right
}else{
  state = 'f';
}
}

void reset(){ //clear acceleration reading
  X = 0;  Y = 0;  Z = 0;
rawDataX[10] = {0};   rawDataY[10] = {0};   rawDataZ[10] = {0}; 
}

void measure3D(int filter){ //3 dimentional measurement
for (int i=0; i<filter; i++) {  
    X += CircuitPlayground.motionX() / filter;
    Y += CircuitPlayground.motionY() / filter;
    Z += CircuitPlayground.motionZ() / filter;
    delay(1); //max resolution
  }
}

char orient(int filter){  //only detect current board orientation for stationary situation, prints always
for (int i=0; i<filter; i++) {  
    X += CircuitPlayground.motionX() / filter;
    Y += CircuitPlayground.motionY() / filter;
    Z += CircuitPlayground.motionZ() / filter;
    delay(1); //max resolution
  }
  return (X == max(max(X, Y), Z)) ? 'X' : (Y == max(max(X, Y), Z)) ? 'Y' : 'Z';

}




void rawMeasure(){  //measure and print three dim on one line

  //measure 
  for (int i=0; i<100; i++) {  
    rawDataX[i] = CircuitPlayground.motionX();
    rawDataY[i] = CircuitPlayground.motionY();
    rawDataZ[i] = CircuitPlayground.motionZ();
    delay(1); //max resolution
  }
  //print
      Serial.print("X: ");
  for (int i=0; i<100; i++) {  
    Serial.print(rawDataX[i]);
    Serial.print("  ");
  }

  delay(1000);

}


void detectOrientChange(){  //only prints out one letter when orientation changes
    static char prevOrient = ' ';  //a random initial state of non x y z
  char currOrient = orient(16);
  if(currOrient != prevOrient){
    Serial.println(currOrient);
    delay(100);
  }
  prevOrient = currOrient;
}

void display3D(int filter){
  measure3D(filter);
  Serial.print(X);
  Serial.print("  ");
  Serial.print(Y);
  Serial.print("  ");
  Serial.print(Z);
  Serial.print("  ");
  Serial.println();
}
  
float tiltSin(float x, float y){ //of sine
return (y/sqrt(x*x+y*y)); 
}


int tiltPixel(float x, float y, float z){ //here use wasd as directions
  if(abs(x)>3 || abs(y)>3){ //only when fruit not laying flat
    if(x<0 && abs(y)<3){  //tilted s
      Serial.print("down  ");
      PixelVal=0;
      if(y<-0.8){
      Serial.print("right ");
      }
      if(y>0.8){
        Serial.print("left  ");

      }
      Serial.println();
    }


    if(y<0 && abs(x)<3){  //tilted d
      Serial.println("right");
                            PixelVal=2;


    }


    if(x>0 && abs(y)<3){  //tilted w
      Serial.print("up  ");
            PixelVal=4;

      if(y<-0.8){
      Serial.print("right ");
      }
      if(y>0.8){
        Serial.print("left  ");

      }
      Serial.println();
    }


    if(y>0 && abs(x)<3){  //tilted a
      Serial.println("left");
                            PixelVal=7;

    }
    return 1;
  }
  if(z>0){
  Serial.println("flat");
  PixelVal=-1;
  }else{
  Serial.println("flipped flat");
  }

  return 0;

}

void modedetect(){
  if(mode=='n'){
    if(CircuitPlayground.rightButton()){
      mode = 's';
      Serial.println("mode toggled to Sequential");
      PixelVal = -1;
    }}else if(mode=='s'){
    if(CircuitPlayground.leftButton() ){
      mode = 'n';
      Serial.println("mode toggled to Normal");
      PixelVal = -1;
    }
  }
}


void timerStart(){
  if(mode=='s'){
    timer+=1;
  }
  if(timer>1000){
    timer=0;
  }
}

int seqControl(float x, float y, float z){
  seqUpdate();
  if(state=='A'){
 PixelVal = timer/100;
 Serial.println(seq[300]);
  }else if(state=='B'){
    PixelVal = 10 - timer/100;
  }else if(state=='C'){
    PixelVal = 5 - (timer/100)%5;
  }else if(state=='D'){
    PixelVal = 5 + (timer/100)%5;
  }
  else{ //implied state f
    PixelVal = -1;
  }
stateUpdate();
}





void setup(){
  Serial.begin(9600); //basically always this freq

  CircuitPlayground.begin();  //todo: need to get rid of this need of lib
  CircuitPlayground.setAccelRange(LIS3DH_RANGE_4_G);  //max detect 4G, prof does not like high range anyways
                                    //candidate vals:
                                      //2, 4, 8 and 16G
  CircuitPlayground.clearPixels();
  CircuitPlayground.setBrightness(brightVal);  
}


void loop(){
  timerStart();
  // Serial.println(timer);

  X = filterData(CircuitPlayground.motionX(), rawDataX);
  Y = filterData(CircuitPlayground.motionY(), rawDataY);
  Z = filterData(CircuitPlayground.motionZ(), rawDataZ);

  // Serial.println(tiltSin(X,Y));
  // Serial.print(X);  Serial.print("  ");  Serial.print(Y);  Serial.print(" ");  Serial.print(Z);
  // Serial.println();
  modedetect();

  if(mode == 'n'){
  // tiltPixel(X,Y,Z);
  
  }else if(mode == 's'){
    seqControl(X,Y,Z);
  }


  red = map(X,0,10,0,255);
  green = map(Y,0,10,0,255);
  blue = map(Z,0,10,0,255);

  // for (int i=0; i<10; ++i) {
  //   CircuitPlayground.setPixelColor(i, red, green, blue);
  // }

    if(PixelVal==-1){
    CircuitPlayground.clearPixels();
    }else{
    CircuitPlayground.setPixelColor(PixelVal, red, green, blue);
    }


  delay(1);   
}