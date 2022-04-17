#include<Wire.h>
int x;
void setup() {
  pinMode (13, OUTPUT);//Connect LED to pin 13
  Wire.begin(9);//9 here is the address(Mentioned even in the master board code) 
//  Wire.onReceive(receiveEvent());
  Serial.begin(9600);
}
//void receiveEvent(int bytes) {
//  x = Wire.read();//Receive value from master board
//  Serial.print(x);
//}
void loop() {
  x = Wire.read();//Receive value from master board
  Serial.print(x);
}
