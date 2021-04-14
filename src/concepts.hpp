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
    ->same_as<bool>;
};

template <typename T>
concept io_digital_pin = digital_writeable<T>&& digital_readable<T>&& requires(const T& t) {
    {t.setInputMode()};
    {t.setOutputMode()};
};
