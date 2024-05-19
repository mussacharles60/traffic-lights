#include <elapsedMillis.h>

#define TRIGGER_A 11
#define ECHO_A    12

#define TRIGGER_B 9
#define ECHO_B    10

#define TRIGGER_C 7
#define ECHO_C    8

#define GREEN_A   A4
#define YELLOW_A  A5
#define RED_A     A3

#define GREEN_B   A2
#define YELLOW_B  3
#define RED_B     2

#define GREEN_C   4
#define YELLOW_C  5
#define RED_C     6

int distance_A = 0;
int distance_B = 0;
int distance_C = 0;

uint8_t reading_round_A = 0;
uint8_t reading_round_B = 0;
uint8_t reading_round_C = 0;

int distance_A_1 = 0;
int distance_B_1 = 0;
int distance_C_1 = 0;

int distance_A_2 = 0;
int distance_B_2 = 0;
int distance_C_2 = 0;

int distance_A_3 = 0;
int distance_B_3 = 0;
int distance_C_3 = 0;

elapsedMillis alternate_timer;
elapsedMillis idle_timer;

uint8_t alternate_count = 0;
uint8_t idle_count = 0;

bool is_idle_state = false;

uint8_t ALTERNATE_TIMEOUT = 10; // time interval for traffic lights to stay on green or red (seconds)
uint8_t IDLE_TIMEOUT = 3; // timeout for the yellow lights (seconds)

uint8_t lights_state = 0; // for allowing which traffic lights to turn on

void setup() {
  Serial.begin(9600);

  pinMode(ECHO_A, INPUT);
  pinMode(ECHO_B, INPUT);
  pinMode(ECHO_C, INPUT);

  pinMode(TRIGGER_A, OUTPUT);
  pinMode(TRIGGER_B, OUTPUT);
  pinMode(TRIGGER_C, OUTPUT);

  pinMode(RED_A, OUTPUT);
  pinMode(RED_B, OUTPUT);
  pinMode(RED_C, OUTPUT);
  
  pinMode(YELLOW_A, OUTPUT);
  pinMode(YELLOW_B, OUTPUT);
  pinMode(YELLOW_C, OUTPUT);

  pinMode(GREEN_A, OUTPUT);
  pinMode(GREEN_B, OUTPUT);
  pinMode(GREEN_C, OUTPUT);

  digitalWrite(RED_A, 0);
  digitalWrite(RED_B, 0);
  digitalWrite(RED_C, 0);

  digitalWrite(YELLOW_A, 0);
  digitalWrite(YELLOW_B, 0);
  digitalWrite(YELLOW_C, 0);

  digitalWrite(GREEN_A, 0);
  digitalWrite(GREEN_B, 0);
  digitalWrite(GREEN_C, 0);

  Serial.println(F("START"));
}

void loop() {
  routines();
  readUltrasonicA();
  readUltrasonicB();
  readUltrasonicC();
}

void readUltrasonicA() {
  // Clears the trigPin
  digitalWrite(TRIGGER_A, 0);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TRIGGER_A, 1);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_A, 0);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration  = pulseIn(ECHO_A, 1);
  // Calculating the distance
  int d = duration * 0.034 / 2;
  if (reading_round_A == 0) {
    reading_round_A = 1;
    distance_A_1 = d;
  }
  else if (reading_round_A == 1) {
    reading_round_A = 2;
    distance_A_2 = d;
  }
  else if (reading_round_A == 2) {
    reading_round_A = 0;
    distance_A_3 = d;
    int dd = (distance_A_1 + distance_A_2 + distance_A_3) / 3;
    if (dd != distance_A) {
      distance_A = dd;
      Serial.print(F("Distance A: "));
      Serial.println(distance_A);
    }
  }
}

void readUltrasonicB() {
  digitalWrite(TRIGGER_B, 0);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_B, 1);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_B, 0);
  long duration  = pulseIn(ECHO_B, 1);
  int d = duration * 0.034 / 2;
  if (reading_round_B == 0) {
    reading_round_B = 1;
    distance_B_1 = d;
  }
  else if (reading_round_B == 1) {
    reading_round_B = 2;
    distance_B_2 = d;
  }
  else if (reading_round_B == 2) {
    reading_round_B = 0;
    distance_B_3 = d;
    int dd = (distance_B_1 + distance_B_2 + distance_B_3) / 3;
    if (dd != distance_B) {
      distance_B = dd;
      Serial.print(F("Distance B: "));
      Serial.println(distance_B);
    }
  }
}

void readUltrasonicC() {
  digitalWrite(TRIGGER_C, 0);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_C, 1);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_C, 0);
  long duration  = pulseIn(ECHO_C, 1);
  int d = duration * 0.034 / 2;
  if (reading_round_C == 0) {
    reading_round_C = 1;
    distance_C_1 = d;
  }
  else if (reading_round_C == 1) {
    reading_round_C = 2;
    distance_C_2 = d;
  }
  else if (reading_round_C == 2) {
    reading_round_C = 0;
    distance_C_3 = d;
    int dd = (distance_C_1 + distance_C_2 + distance_C_3) / 3;
    if (dd != distance_C) {
      distance_C = dd;
      Serial.print(F("Distance C: "));
      Serial.println(distance_C);
    }
  }
}

void routines() {
  if (alternate_timer > 1000 && !is_idle_state) {
    alternate_timer = 0;
    alternate_count = alternate_count + 1;
    if (alternate_count >= ALTERNATE_TIMEOUT) {
      alternate_count = 0;
      // time to alternate red and green lights
      is_idle_state = true;
      idle_timer = 0;
      setIdleState();
    }
    idle_timer = 0;
  }
  if (idle_timer >= 1000 && is_idle_state) {
    idle_timer = 0;
    idle_count = idle_count + 1;
    if (idle_count >= IDLE_TIMEOUT) {
      idle_count = 0;
      // time to turn off yellow lights
      is_idle_state = false;
      if (lights_state == 0) {
        lights_state = 1;
      } else {
        lights_state = lights_state + 1;
      }
      if (lights_state > 3) {
        lights_state = 1;
      }
      alternate_timer = 0;
      // switch lights
      switchLights();
    }
    alternate_timer = 0;
  }
}

void setIdleState() {
  digitalWrite(RED_A, 0);
  digitalWrite(RED_B, 0);
  digitalWrite(RED_C, 0);

  digitalWrite(YELLOW_A, 1);
  digitalWrite(YELLOW_B, 1);
  digitalWrite(YELLOW_C, 1);

  digitalWrite(GREEN_A, 0);
  digitalWrite(GREEN_B, 0);
  digitalWrite(GREEN_C, 0);
}

void switchLights() {
  switch (lights_state) {
    case 1: // A
      Serial.println(F("Lights: A"));
      digitalWrite(RED_A, 0);
      digitalWrite(RED_B, 1);
      digitalWrite(RED_C, 1);

      digitalWrite(YELLOW_A, 0);
      digitalWrite(YELLOW_B, 0);
      digitalWrite(YELLOW_C, 0);

      digitalWrite(GREEN_A, 1);
      digitalWrite(GREEN_B, 0);
      digitalWrite(GREEN_C, 0);
      break;
    case 2: // B
      Serial.println(F("Lights: B"));
      digitalWrite(RED_A, 1);
      digitalWrite(RED_B, 0);
      digitalWrite(RED_C, 1);

      digitalWrite(YELLOW_A, 0);
      digitalWrite(YELLOW_B, 0);
      digitalWrite(YELLOW_C, 0);

      digitalWrite(GREEN_A, 0);
      digitalWrite(GREEN_B, 1);
      digitalWrite(GREEN_C, 0);
      break;
    case 3: // C
      Serial.println(F("Lights: C"));
      digitalWrite(RED_A, 1);
      digitalWrite(RED_B, 1);
      digitalWrite(RED_C, 0);

      digitalWrite(YELLOW_A, 0);
      digitalWrite(YELLOW_B, 0);
      digitalWrite(YELLOW_C, 0);

      digitalWrite(GREEN_A, 0);
      digitalWrite(GREEN_B, 0);
      digitalWrite(GREEN_C, 1);
      break;
  }
}
