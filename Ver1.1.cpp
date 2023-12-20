#include <SPI.h>
#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_CircuitPlayground.h>

int brightVal = 3;
int PixelVal = -1;

//unit: meters per second squared
volatile float X, Y, Z; //3-dimensional data
int red = 0;  int green = 0;  int blue = 0;

// //below are large amounts of raw data for "man-powered machine learning"
float rawDataX[16] = {0};  //initialize data set of 10 measurements of X, Y and Z
float rawDataY[16] = {0};  
float rawDataZ[16] = {0};  
  

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
  int num = 0;
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


void setup() {
  Serial.begin(9600); //basically always this freq

  CircuitPlayground.begin();  //todo: need to get rid of this need of lib
  CircuitPlayground.setAccelRange(LIS3DH_RANGE_4_G);  //max detect 4G, prof does not like high range anyways
                                    //candidate vals:
                                      //2, 4, 8 and 16G
  CircuitPlayground.clearPixels();
  CircuitPlayground.setBrightness(brightVal);  
}


void loop() {

  X = filterData(CircuitPlayground.motionX(), rawDataX);
  Y = filterData(CircuitPlayground.motionY(), rawDataY);
  Z = filterData(CircuitPlayground.motionZ(), rawDataZ);

  // Serial.println(tiltSin(X,Y));
  // Serial.print(X);  Serial.print("  ");  Serial.print(Y);  Serial.print(" ");  Serial.print(Z);
  // Serial.println();



  tiltPixel(X,Y,Z);


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