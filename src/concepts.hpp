#pragma once

#include <type_traits.hpp>

template <class T, class U>
inline constexpr bool is_same_v = std::is_same<T, U>::value;

template<typename T>
inline constexpr bool is_integral_v = std::is_integral<T>::value;

template<typename T>
inline constexpr bool is_signed_v = std::is_signed<T>::value;

template <class T, class U>
concept same_as_impl = is_same_v<T, U>;  // exposition only

template <class T, class U>
concept same_as = same_as_impl<T, U>&& same_as_impl<U, T>;

template <typename From, typename To>
inline constexpr bool is_convertible_v = std::is_convertible<From, To>::value;

template <typename T>
using add_rvalue_reference_t = typename std::add_rvalue_reference<T>::type;

template <typename From, typename To>
concept convertible_to = is_convertible_v<From, To>
    && requires(add_rvalue_reference_t<From> (&f)()) {
        static_cast<To>(f());
    };

template<typename T>
concept integral = is_integral_v<T>;

template<typename T>
concept signed_integral = integral<T> && is_signed_v<T>;

template<typename T>
concept unsigned_integral = integral<T> && !signed_integral<T>;

template <typename T>
concept digital_writeable = requires(const T& t) {
    {t.digitalWrite(bool{})};
};

template <typename T>
concept digital_readable = requires(const T& t) {
    { t.digitalRead() }
    ->convertible_to<bool>;
};

template <typename T>
concept io_digital_pin = digital_writeable<T>&& digital_readable<T>&& requires(const T& t) {
    {t.setInputMode()};
    {t.setOutputMode()};
};


template <typename T>
concept pwm_pin = requires(const T& t) {
    {t.setPWM((unsigned char){})};
    {t.setupPWM()};
};

template <typename T>
concept analog_readable = requires(const T& t) {
    {t.setupAnalogRead()};
    {t.analogRead() } -> convertible_to<unsigned short>;
};
