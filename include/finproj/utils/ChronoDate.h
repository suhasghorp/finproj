#ifndef FINDATE__CHRONODATE_H_
#define FINDATE__CHRONODATE_H_

#include <chrono>
#include <compare>
#include <exception>
#include <array>
#include <string>


namespace date = std::chrono;
using Date = std::chrono::year_month_day;

enum class PeriodTypes {
  DAYS = 1,
  WEEKS = 2,
  MONTHS = 3,
  YEARS = 4
};

class ChronoDate {
 public:
  ChronoDate(int year, unsigned month, unsigned day);
  explicit ChronoDate(const Date& ymd);
  ChronoDate(const ChronoDate& cd);
  ChronoDate& operator = (const ChronoDate& chronodate2);
  explicit ChronoDate(const std::string& str);
  ChronoDate();

  // Accessors:
  [[nodiscard]] int year() const;
  [[nodiscard]] unsigned month() const;
  [[nodiscard]] unsigned day() const;
  [[nodiscard]] int serial_date() const;
  [[nodiscard]] date::year_month_day ymd() const;

  // return a new ChronoDate object:
  [[nodiscard]] ChronoDate add_years(double rhs_years) const ;
  [[nodiscard]] ChronoDate add_months(int rhs_months) const ;
  [[nodiscard]] ChronoDate add_weeks(int rhs_weeks) const;
  [[nodiscard]] ChronoDate add_weekdays(int num_days) const;
  [[nodiscard]] ChronoDate add_days(int rhs_days) const ;
  [[nodiscard]] ChronoDate eom() const ;
  [[nodiscard]] unsigned third_wed_of_month() const ;
  [[nodiscard]] ChronoDate next_imm_date() const ;
  [[nodiscard]] ChronoDate next_cds_date(int mm = 0) const ;
  [[nodiscard]] ChronoDate add_tenor(const std::string& tenor) const ;

  // Operators

  int operator - (const ChronoDate& rhs) const;
  bool operator == (const ChronoDate& rhs) const;
  std::strong_ordering operator <=> (const ChronoDate& rhs) const;

  // Check state:
  [[nodiscard]] bool is_eom() const;
  [[nodiscard]] unsigned days_in_month() const;
  [[nodiscard]] bool leap_year() const;
  [[nodiscard]] bool is_weekend() const;
  [[nodiscard]] date::weekday weekday() const;
  [[nodiscard]] bool is_last_day_of_feb() const;

  // friend operator so that we can output date details with cout
  friend std::ostream& operator << (std::ostream& os, const ChronoDate& rhs);

 private:
  date::year_month_day date_;
  int serial_date_;

  void reset_serial_date_();
};

#endif//FINDATE__CHRONODATE_H_
