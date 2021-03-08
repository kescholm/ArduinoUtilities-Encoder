#include <Arduino.h>
#include <unity.h>

#include "ArdEncoder.h"

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

// ArdEncoder
// ------

// Decode value from counts
static void test_encoder_decode(void) {
    // number of steps to test
    const uint32_t test_steps = 200U;
    // counts from sensor
    const int32_t counts[200] = {};
    // expected value
    const double expected_values[200] = {};

    // encoder object
    ArdEncoder encoder;
    // bit depth of counter
    uint8_t counter_resolution_bits = 8;
    // counts per revolution
    uint32_t counts_per_rev = 128;
    // value per revolution
    const double value_per_rev = 360.0;
    // set parameters
    int result = ard_encoder_set(&encoder, counter_resolution_bits, counts_per_rev, value_per_rev);
    TEST_ASSERT_EQUAL(0, result);
    // reset count and value to zero
    ard_encoder_reset(&encoder, 0, 0.0);

    // run
    for (uint32_t k = 0; k < test_steps; ++k) {
        // run decode to get sensor value
        const double actual = ard_encoder_decode(&encoder, counts[k]);
        TEST_ASSERT_EQUAL_FLOAT(expected_values[k], actual);
    }
}

// Encode counts from value
static void test_encoder_encode(void) {
    // number of steps to test
    const uint32_t test_steps = 200U;
    // input values
    const double values[200] = {};
    // encoded counts
    const int32_t expected_delta_counts[200] = {};

    // encoder object
    ArdEncoder encoder;
    // bit depth of counter
    uint8_t counter_resolution_bits = 8;
    // counts per revolution
    uint32_t counts_per_rev = 128;
    // value per revolution
    const double value_per_rev = 360.0;
    // set parameters
    int result = ard_encoder_set(&encoder, counter_resolution_bits, counts_per_rev, value_per_rev);
    TEST_ASSERT_EQUAL(0, result);
    // reset count and value to zero
    ard_encoder_reset(&encoder, 0, 0.0);

    // run
    for (uint32_t k = 0; k < test_steps; ++k) {
        // run encode to get change in counts
        const int32_t actual = ard_encoder_encode(&encoder, values[k]);
        TEST_ASSERT_EQUAL_INT32(expected_delta_counts[k], actual);
    }
}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();  // IMPORTANT LINE!

    // Run Tests
    // ---------

    RUN_TEST(test_encoder_decode);
    RUN_TEST(test_encoder_encode);

    // Done
    // ----

    UNITY_END();
}

void loop() {
    // never reached
}
