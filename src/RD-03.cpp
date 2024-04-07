/**
 * Ai-Thinker Rd-03 Library:
 *
 * This library allows me to use the Ai-Thinker Rd-03 sensor in a very basic way.
 */

#include "RD-03.h"

/**
 * @brief Constructor for the RD03 class.
 * 
 * This constructor initializes an instance of the RD03 class with the provided HardwareSerial object.
 * It sets the 'with_pins' flag to false, indicating that no pins are configured initially.
 * Similarly, the 'start_reading' flag is set to false, indicating that reading from UART has not started yet.
 * The parameter 'hsUart' specifies the HardwareSerial object to be used for communication.
 * 
 * @param hsUart Pointer to the HardwareSerial object to be used for communication.
 */
RD03::RD03(HardwareSerial *hsUart) 
{
  with_pins = false;
  start_reading = false;
  HS = hsUart;
}


/**
 * @brief Constructor for the RD03 class with specified RX and TX pins.
 * 
 * This constructor initializes an instance of the RD03 class with the provided RX and TX pins,
 * along with the specified HardwareSerial object for communication.
 * It sets the 'with_pins' flag to true, indicating that pins are configured for communication.
 * The 'start_reading' flag is set to false, indicating that reading from UART has not started yet.
 * The parameters 'rx' and 'tx' specify the RX and TX pins respectively.
 * The parameter 'hsUart' specifies the HardwareSerial object to be used for communication.
 * 
 * @param rx The RX pin for UART communication.
 * @param tx The TX pin for UART communication.
 * @param hsUart Pointer to the HardwareSerial object to be used for communication.
 */
RD03::RD03(uint8_t rx, uint8_t tx, HardwareSerial *hsUart) 
{
    with_pins = true;
    start_reading = false;
    rx_pin = rx;
    tx_pin = tx;
    HS = hsUart;
}


/**
 * @brief Initializes the RD03 module and sets its parameters.
 * 
 * This method initializes the RD03 module and configures its parameters such as minimum and maximum distances,
 * and delay time for detection. It also configures the UART communication settings based on whether pins
 * are configured or not.
 * 
 * @param _min The minimum detection distance to be set (0-15, each unit represents 70 cm).
 * @param _max The maximum detection distance to be set (0-15, each unit represents 70 cm).
 * @param _delay The delay time for detection (0-30, each unit represents 1 second).
 *  
 * Note: The minimum detection distances specified here are assigned
 * to variables within the class and are not directly configured in the sensor.
 */
void RD03::begin(uint8_t _min, uint8_t _max, uint8_t _delay) 
{
    if (not with_pins)
    {
        HS->begin(LD2461_DEFAULT_BAUD);
    }
    else
    {
        HS->begin(LD2461_DEFAULT_BAUD, SERIAL_8N1, rx_pin, tx_pin);
    }

    _max = (_max > 15) ? 15 : (_max < 0) ? 0 : _max;
    _min = (_min > 15) ? 15 : (_min < 0) ? 0 : _min;
    _delay = (_delay > 30) ? 30 : (_delay < 0) ? 0 : _delay;

    // set_min_distance[PARAMETER_VALUE] = _min;
    set_max_distance[PARAMETER_VALUE] = _max;
    set_delay_time[PARAMETER_VALUE] = _delay;
    min_detection_distance = _min;
    max_detection_distance = _max;
    disappear_delay = _delay;

    delay(100);
    HS->write(open_command_mode, sizeof(open_command_mode));
    delay(150);
    HS->write(set_min_distance, sizeof(set_min_distance));
    delay(150);
    HS->write(set_max_distance, sizeof(set_max_distance));
    delay(150);
    HS->write(set_delay_time, sizeof(set_delay_time));
    delay(150);
    HS->write(reporting_mode, sizeof(reporting_mode));
    delay(150);
    HS->write(close_command_mode, sizeof(close_command_mode));
    delay(150);
}


/**
 * @brief Reads data from the UART and processes it.
 * 
 * This method reads data from the UART and processes it to identify frames.
 * It continuously reads data until the 'start_reading' flag is set to false.
 * Upon identifying the start and end frames, it extracts information such as
 * presence of someone and distance from the received data.
 * 
 * Note: This method should be called repeatedly to continuously read and process UART data.
 * It is recommended to call this method at least every 100 milliseconds to ensure timely processing of data.
 */
void RD03::read() 
{
    start_reading = true;
    serial_data.size = 0;
    serial_data.frame_start = 0;
    while (start_reading) 
    {
        if (HS->available()) 
        {
            uint8_t c = HS->read();
            serial_data.buffer[serial_data.size] = c;
            serial_data.size ++;
            switch(c) 
            {
                case FH_LAST: 
                {
                    uint8_t fh_size = sizeof(FRAME_HEADER);
                    if(serial_data.size >= fh_size && memcmp(serial_data.buffer+serial_data.size-fh_size, FRAME_HEADER, fh_size) == 0) 
                    {
                    serial_data.frame_start = serial_data.size - fh_size;
                    }
                    break;  
                }
                case FE_LAST: 
                {
                    uint8_t packet_size;
                    uint8_t fe_size = sizeof(FRAME_END);
                    uint8_t min_packet_size = sizeof(FRAME_HEADER) + fe_size + 5;
                    if(serial_data.size >= min_packet_size && memcmp(serial_data.buffer+serial_data.size-fe_size, FRAME_END, fe_size) == 0) 
                    {
                        if (serial_data.buffer[serial_data.frame_start + FH_0] == FRAME_HEADER[0] && serial_data.buffer[serial_data.frame_start + FH_1] == FRAME_HEADER[1] &&
                        serial_data.buffer[serial_data.frame_start + FH_2] == FRAME_HEADER[2] && serial_data.buffer[serial_data.frame_start + 3] == FRAME_HEADER[FH_3])
                        {
                        someone = serial_data.buffer[serial_data.frame_start + SOMEONE];
                        distance = serial_data.buffer[serial_data.frame_start + TARGET_RANGE_0];
                        }     
                        serial_data.size = 0;
                        serial_data.frame_start = 0;
                        start_reading = false;
                    }
                }
            }
        }
    }
}


/**
 * @brief Checks if someone is detected within the specified distance range.
 * 
 * This method checks if someone is detected within the distance range configured for the RD03 module.
 * It returns true if someone is detected within the specified distance range, otherwise false.
 * 
 * @return true if someone is detected within the configured distance range.
 * @return false if no one is detected within the configured distance range.
 */
bool RD03::getSomeone() 
{
    return (someone > 0 && distance >= (min_detection_distance * 70) && distance <= (max_detection_distance * 70));
}