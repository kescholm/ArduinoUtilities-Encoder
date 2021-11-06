
/**
 * @file       ArdEncoder.h
 * @author     Kyle Escholm
 * @brief      Counter encoding and decoding
 *
 * @details
 *
 * See group @ref ArdEncoder
 *
 */

#ifndef ARD_ENCODER_H
#define ARD_ENCODER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @defgroup   ArdEncoder Incremental encoder
 * @brief      Counter encoding and decoding
 *
 * @{
 *
 */

enum eArdEncoderResult
{
    kArdEncoderSuccess = 0,
    kArdEncoderResolutionIsZero,
    kArdEncoderResolutionTooHigh
};

/**
 * @brief Encoder state
 *
 */
class ArdEncoder
{
   public:
    ArdEncoder() : m_value(0.0), m_count(0), m_value_per_rev(0.0), m_counts_per_rev(0), m_counter_limit(0) {}

    /**
     * @brief Set encoder parameters
     *
     * @param counter_resolution Bit depth of counter (max 32)
     * @param counts_per_rev Number of counts per recvolution
     * @param value_per_rev scaled units per revolution
     * @return eArdEncoderResult
     */
    eArdEncoderResult SetParameters(const uint8_t counter_resolution_bits, const uint32_t counts_per_rev,
                                    const double value_per_rev);

    /**
     * @brief Reset encoder to value and counts
     *
     * @param count reset counter counts
     * @param value reset scaled value
     */
    void Reset(const int32_t count, const double value);

    /**
     * @brief Reset encoder absolute value. Counts set to scaled value without any offset
     *
     * @param value reset scaled value
     */
    void ResetValue(const double value);

    /**
     * @brief Get value in scaled units from counter update
     *
     * @param count new input from counter
     * @return double scaled value
     */
    double Decode(const int32_t count);

    /**
     * @brief encode value to number of counts
     *
     * @param value value to encode
     * @return int32_t change in counts from last encode update
     */
    int32_t Encode(const double value);

   private:
    /**
     * @brief Current encoder value
     *
     */
    double m_value;
    /**
     * @brief Current encoder count
     *
     */
    int32_t m_count;
    /**
     * @brief Scaled units per revolution
     *
     */
    double m_value_per_rev;
    /**
     * @brief Encoder counts per revolution
     *
     */
    uint32_t m_counts_per_rev;
    /**
     * @brief incremental encoder limit based on counter bit depth
     *
     */
    int32_t m_counter_limit;
};

/**
 * @}
 */

inline eArdEncoderResult ArdEncoder::SetParameters(const uint8_t counter_resolution_bits, const uint32_t counts_per_rev,
                                                   const double value_per_rev)
{
    eArdEncoderResult result = kArdEncoderSuccess;
    if (counter_resolution_bits == 0)
    {
        result = kArdEncoderResolutionIsZero;
    }
    else if (counter_resolution_bits > 32)
    {
        result = kArdEncoderResolutionTooHigh;
    }
    else
    {
        m_value = 0.0;
        m_count = 0;
        m_value_per_rev = value_per_rev;
        m_counts_per_rev = counts_per_rev;

        if (counter_resolution_bits == 32)
        {
            m_counter_limit = INT32_MIN;
        }
        else if (counter_resolution_bits == 16)
        {
            m_counter_limit = INT16_MIN;
        }
        else if (counter_resolution_bits == 8)
        {
            m_counter_limit = INT8_MIN;
        }
        else
        {
            m_counter_limit = -1;
            for (uint8_t i = 0; i < counter_resolution_bits - 1; i++)
            {
                m_counter_limit *= 2;
            }
        }
    }

    return result;
}

inline void ArdEncoder::Reset(const int32_t count, const double value)
{
    m_count = count;
    m_value = value;
}

inline void ArdEncoder::ResetValue(const double value)
{
    m_value = value;
    m_count = static_cast<int32_t>(m_value * static_cast<double>(m_counts_per_rev) / m_value_per_rev);
}

inline double ArdEncoder::Decode(const int32_t count)
{
    // change in count
    int32_t delta = count - m_count;
    if (delta < m_counter_limit)
    {
        delta = delta - 2 * m_counter_limit;
    }
    else if (delta > -m_counter_limit)
    {
        delta = delta + 2 * m_counter_limit;
    }
    // save count
    m_count = count;
    // update value
    m_value += (static_cast<double>(delta) * m_value_per_rev) / static_cast<double>(m_counts_per_rev);

    return m_value;
}

inline int32_t ArdEncoder::Encode(const double value)
{
    // update absolute counts
    const int32_t count = static_cast<int32_t>(value * static_cast<double>(m_counts_per_rev) / m_value_per_rev);
    // change in count
    const int32_t delta = count - m_count;
    // save count
    m_count = count;
    // update value
    m_value = value;

    return delta;
}

#endif
