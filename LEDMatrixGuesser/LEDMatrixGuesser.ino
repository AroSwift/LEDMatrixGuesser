// Definitely define the row and column number of the matrix
// before runtime because this is needed for setting the 2D array.
const int num_rows = 3;
const int num_cols = 3;

// A class to manage a LED matrix
class LedMatrix {
  int matrix[num_rows][num_cols];
  int row_pent_pin;
  int col_pent_pin;
  int button_pin;
  int current_row = 0;
  int current_col = 0;
  int hidden_row;
  int hidden_col;

  public:
    // Constructor
    LedMatrix(int matrix_pins[num_rows][num_cols], int row_pin, int col_pin, int btn_pin) {
      // Set the potentiometer pins
      row_pent_pin = row_pin;
      col_pent_pin = col_pin;
      
      // Set the button pin and mode
      button_pin = btn_pin;
      pinMode(button_pin, INPUT);
      
      // Copy matrix_pins to matrix
      memcpy(*matrix, *matrix_pins, sizeof(int)*num_rows*num_cols);
    }

    // Initialize random seed during setup
    void init_hidden_values() {
      // Initialize a random seed for the random generator first
      randomSeed(analogRead(A5));
      // Then, generate a hidden row and column
      this->set_hidden();
    }

    // Set a new random row and column to "hide"
    void set_hidden() {
      // Set a random row and col between 0 and the given value-1
      hidden_row = random(num_rows);
      hidden_col = random(num_cols);
    }

    // Flash the LEDs on and off 3 times. Note: the arduino
    // will do nothing else while this proceeds (as desired)
    void flash_leds() {
      // Set the default LED on/off count and set the LED to start on
      int on_off_count = 0;
      boolean led_on = true;
      // Iterate through an on/off cycle 3 times for a total of 6 on and off states
      while(on_off_count < 6) {
        // Iterate over all the rows and columns of the LED matrix
        for(int i = 0; i < num_rows; i++) {
          for(int j = 0; j < num_cols; j++) {
            // When the LED should turn on
            if(led_on) {
              // Turn on the LED
              digitalWrite(matrix[i][j], HIGH);
            } else { // Otherwise, the LED should turn off
              // Turn off the LED
              digitalWrite(matrix[i][j], LOW);
            }
          }
        }
        // Flip the LED state (off becomes on and visa versa)
        led_on = !led_on;
        // Increment the on/off count
        on_off_count++;
        // Wait 0.5 seconds between flashing on then off
        delay(500);
      }

      // Set the LED on for the current row and column
      digitalWrite(matrix[current_row][current_col], HIGH);
    }

    // Read the potentiometer and map it to a row and column in the matrix.
    // Then, turn on an LED to display that position.
    void maintain_led_position() {
      // Read the row and col potentiometer values
      int row_pent_value = analogRead(row_pent_pin);
      int col_pent_value = analogRead(col_pent_pin);

      // Map the analog values to a value between 0-2
      row_pent_value = map(row_pent_value, 0, 1023, 0, 2);
      col_pent_value = map(col_pent_value, 0, 1023, 0, 2);

      // When the current row or col is not equal to the mapped potentiometer value
      if(current_row != row_pent_value || current_col != col_pent_value) {
        // Turn off the previous LED
        digitalWrite(matrix[current_row][current_col], LOW);
        // Update the current row and col
        current_row = row_pent_value;
        current_col = col_pent_value;
        // Turn on the new LED
        digitalWrite(matrix[current_row][current_col], HIGH);
      }
    }

    // Check when the button is pressed. When the user's selection matched a hidden 
    // row/column, celebrate with LED flashing. Regardless, get a new hidden row and column.
    void check_selection() {
      // Read a button's state
      boolean btn_state = digitalRead(button_pin);
      
      // When the button is pressed
      if (btn_state == HIGH) {
        // And when the current row/column pair is equal to the hidden row/column pair
        if(current_row == hidden_row && current_col == hidden_col) {
          // Flash the leds on and off
          this->flash_leds();
        } // Otherwise, the user chose wrong so no LED celebration

        // Get a new hidden row and column
        this->set_hidden();
      }
    }

    // Update LED matrix
    void update() {
      // Maintain the current LED position
      this->maintain_led_position();
      // Watch for a button press
      this->check_selection();
      
      // Add a slight delay to prevent the proclivity of potentiometer misreadings. Issue: rapid 
      // LED flickering that occurs when the potentiometer value slightly and randomly 
      // fluctuates at a range that becomes more noticable when mapped to a range between 0-2.
      delay(50);
    }
    
  // end public area
};


// Set the LED pins required for the matrix
int led_pins_matrix[num_rows][num_cols] = {
  { 2, 3, 4 },
  { 5, 6, 7 },
  { 8, 9, 10 }
};

// Initialize a LED 3x3 matrix
LedMatrix matrix(led_pins_matrix, A0, A1, 11);

void setup() {
  // Seed a random sequence then initialize the hidden row and column
  matrix.init_hidden_values();
}

void loop() {
  matrix.update();
}
