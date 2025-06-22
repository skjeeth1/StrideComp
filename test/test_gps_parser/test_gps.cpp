#include <Arduino.h>
#include <unity.h>
#include "gps_parser.h"

// Reference GPGLL: DDMM.MMMM
// Input: 4916.45 N => 49 deg + 16.45' = 49.27417 deg => 4927417 in fixed-point
// Input: 12311.12 W => -123 deg - 11.12' = -123.18533 deg => -12318533 in fixed-point

void test_valid_gpgll_north_east(void)
{
    const char *sentence = "$GPGLL,4916.45,N,12311.12,W,225444,A,*1D";
    GPSLocation loc;

    bool result = parseGPGLL(sentence, loc);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_INT_WITHIN(5, 4927417, loc.latitude); // within 0.00005 deg
    TEST_ASSERT_INT_WITHIN(5, -12318533, loc.longitude);
}

void test_invalid_sentence_type(void)
{
    const char *sentence = "$GPRMC,4916.45,N,12311.12,W";
    GPSLocation loc;

    bool result = parseGPGLL(sentence, loc);

    TEST_ASSERT_FALSE(result);
}

void test_invalid_format(void)
{
    const char *sentence = "$GPGLL,491,N,123,W";
    GPSLocation loc;

    bool result = parseGPGLL(sentence, loc);

    TEST_ASSERT_FALSE(result);
}

void setup()
{
    UNITY_BEGIN();
    RUN_TEST(test_valid_gpgll_north_east);
    RUN_TEST(test_invalid_sentence_type);
    RUN_TEST(test_invalid_format);
    UNITY_END();
}

void loop()
{
    // not used in test
}
