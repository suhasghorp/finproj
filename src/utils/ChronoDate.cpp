#include "finproj/utils/ChronoDate.h"
#include <finproj/utils/Misc.h>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <ranges>

ChronoDate::ChronoDate(int year, unsigned month, unsigned day) :
                                                                 date_{ date::year{year} / date::month{month} / date::day{day} }
{
  if (!date_.ok())		// std::chrono member function to check if valid date
  {
    std::runtime_error e("ChronoDate constructor: Invalid date.");
    throw e;
  }

  reset_serial_date_();
}

ChronoDate::ChronoDate(const Date& ymd) : date_{ ymd }{
  if (!date_.ok()){	// std::chrono member function to check if valid date
    std::runtime_error e("ChronoDate constructor: Invalid year_month_day input.");
    throw e;
  }
  reset_serial_date_();
}

ChronoDate::ChronoDate(const std::string& str){
  auto splits = split(str,"/");
  unsigned int m = stoul(splits.at(0));
  unsigned int d = stoul(splits.at(1));
  int y = stoi(splits.at(2));
  date_ = std::chrono::year_month_day{ date::year{y} / date::month{m} / date::day{d} };
  reset_serial_date_();
}

ChronoDate::ChronoDate(const ChronoDate& cd) : date_{ cd.date_ }, serial_date_{cd.serial_date_} {}

ChronoDate::ChronoDate() :date_{ date::year(1970), date::month{1}, date::day{1} },
                           serial_date_{ 0 } { }

// chrono::year can be cast to int
// See https://en.cppreference.com/w/cpp/chrono/year/operator_int
int ChronoDate::year() const {
  return static_cast<int>(date_.year());
}

// chrono::month can be cast to unsigned
// Note this is different from chrono::year (int)
// See https://en.cppreference.com/w/cpp/chrono/month/operator_unsigned
unsigned ChronoDate::month() const {
  return static_cast<unsigned>(date_.month());
}

// chrono::day can be cast to unsigned
// Note this is different from chrono::year (int)
// See https://en.cppreference.com/w/cpp/chrono/day/operator_unsigned
unsigned ChronoDate::day() const {
  return static_cast<unsigned>(date_.day());
}

int ChronoDate::serial_date() const {
  //	return date::sys_days(date_).time_since_epoch().count();
  return serial_date_;
}

std::chrono::year_month_day ChronoDate::ymd() const {
  return date_;
}

ChronoDate ChronoDate::add_years(double rhs_years) const {
  int mmi = static_cast<int>(rhs_years * 12.0d);
  int ddi = static_cast<int>((rhs_years * 12.0d - mmi) * gdays_in_month);
  Date new_date = date_ + date::months(mmi);
  if (!new_date.ok()){
    new_date = new_date.year() / new_date.month() / 28;
  }
  ChronoDate new_dt{new_date};
  new_dt = new_dt.add_days(ddi);

  // Only possible error case is if month is February
  // and the result is day = 29 in a non-leap year.

  return new_dt;
}

unsigned ChronoDate::third_wed_of_month() const {
   Date new_date{date_.year() / date_.month()/std::chrono::Wednesday[3]};
   if (!new_date.ok()){
    throw std::runtime_error("thord wed of month is not right");
   }
   return static_cast<unsigned>(new_date.day());
}

ChronoDate ChronoDate::next_cds_date(int mm) const {
   auto next_date = add_months(mm);
   auto y = static_cast<int >(next_date.year());
   unsigned int m = next_date.month();
   auto d = static_cast<unsigned >(next_date.day());
   unsigned int d_cds = 20;
   auto y_cds = y;
   unsigned int m_cds = 999;
   if (m == 12 and d >= 20) {
    m_cds = 3;
    y_cds = y + 1;
   } else if (m == 10 || m == 11 || m == 12){
    m_cds = 12;
   } else if (m == 9 && d >= 20){
    m_cds = 12;
   } else if (m == 7 || m == 8 || m == 9){
    m_cds = 9;
   } else if (m == 6 && d >= 20){
    m_cds = 9;
   } else if (m == 4 || m == 5 || m == 6){
    m_cds = 6;
   } else if (m == 3 && d >= 20){
    m_cds = 6;
   } else if (m == 1 || m == 2 || m == 3){
    m_cds = 3;
   }
   ChronoDate cds_date{y_cds,m_cds,d_cds};
   return cds_date;
}

ChronoDate ChronoDate::next_imm_date() const {
   auto m = date_.month();
   auto m_imm = m;
   auto d = static_cast<unsigned >(date_.day());
   auto y = static_cast<int >(date_.year());
   auto y_imm = y;

   if (m == std::chrono::December &&  d >= third_wed_of_month()){
    m_imm = std::chrono::March;
    y_imm += 1;
   } else if (m == std::chrono::October || m == std::chrono::November || m == std::chrono::December){
    m_imm = std::chrono::December;
   } else if (m == std::chrono::September && d >= third_wed_of_month()){
    m_imm = std::chrono::December;
   } else if (m == std::chrono::July || m == std::chrono::August || m == std::chrono::September){
    m_imm = std::chrono::September;
   } else if (m == std::chrono::June && d >= third_wed_of_month()){
    m_imm = std::chrono::September;
   } else if (m == std::chrono::April || m == std::chrono::May || m == std::chrono::June){
    m_imm = std::chrono::June;
   } else if (m == std::chrono::March && d >= third_wed_of_month()){
    m_imm = std::chrono::June;
   } else if (m == std::chrono::January || m == std::chrono::February || m == std::chrono::March){
    m_imm = std::chrono::March;
   }
   ChronoDate d_temp{date::year{y_imm} / date::month{m_imm} / date::day{1}};
   auto third = d_temp.third_wed_of_month();
   ChronoDate imm{date::year{y_imm} / date::month{m_imm} / date::day{third}};
   return imm;
}

ChronoDate ChronoDate::add_weeks(int rhs_weeks) const {
  Date new_date{};
  for (int w = 1; w <= rhs_weeks; ++w) {
    new_date = date::sys_days(date_) + date::weeks(w);// Naively attempt the addition
  }
  // If the date is invalid, it is because the
  // result is an invalid end-of-month:
  if (!new_date.ok()){
    new_date = date_.year() / date_.month() / date::day{ days_in_month() };
  }
  return {new_date};
}

ChronoDate ChronoDate::add_months(int rhs_months) const {
  Date new_date = date_ + date::months(rhs_months);    // Naively attempt the addition
  // If the date is invalid, it is because the
  // result is an invalid end-of-month:
  if (!new_date.ok()){
    new_date = date_.year() / date_.month() / date::day{ days_in_month() };
  }
  return {new_date};
}

ChronoDate ChronoDate::add_weekdays(int num_days) const {
  auto positive_num_days = num_days > 0 ? true:false;
  num_days = fabs(num_days);
  auto num_days_left = num_days;
  ChronoDate end_date {*this };
  while (num_days_left > 0){
    if (positive_num_days){
      end_date = end_date.add_days(1);
    } else {
      end_date = end_date.add_days(-1);
    }
    if (!end_date.is_weekend()){
      num_days_left -= 1;
    }
  }
  return end_date;
}

ChronoDate ChronoDate::add_days(int rhs_days) const {
  // Note that adding days is handled differently, per Howard Hinnant's Stack Overflow comments.
  // See https://stackoverflow.com/questions/62734974/how-do-i-add-a-number-of-days-to-a-date-in-c20-chrono

  Date new_date = date::sys_days(date_) + date::days(rhs_days);

  if (!new_date.ok())
  {
    std::runtime_error e("ChronoDate::ChronoDate::addDays(.): resulting date out of range.");
    throw e;
  }

  return {new_date};
}

ChronoDate ChronoDate::add_tenor(const std::string& tenor) const {
  ChronoDate dt{};
  std::string tenor_copy{tenor};
  PeriodTypes period_type;
  for_each(tenor_copy.begin(), tenor_copy.end(), [](char& in){ in = ::toupper(in); });
  char last = tenor_copy.back();
  std::string first = tenor_copy.substr(0, tenor_copy.size()-1);
  int num_periods{};
  if (tenor_copy == "ON"){
    period_type = PeriodTypes::DAYS;
    num_periods = 1;
  } else if (tenor_copy == "TN"){
    period_type = PeriodTypes::DAYS;
    num_periods = 1;
  } else if (last == 'D'){
    period_type = PeriodTypes::DAYS;
    num_periods = stoi(first);
  } else if (last == 'W'){
    period_type = PeriodTypes::WEEKS;
    num_periods = stoi(first);
  } else if (last == 'M'){
    period_type = PeriodTypes::MONTHS;
    num_periods = stoi(first);
  } else if (last == 'Y'){
    period_type = PeriodTypes::YEARS;
    num_periods = stoi(first);
  } else {
    throw std::runtime_error("Unknown tenor provided");
  }
  if (period_type == PeriodTypes::DAYS){
    dt = add_days(num_periods);
  }
  if (period_type == PeriodTypes::WEEKS){
    dt = add_weeks(num_periods);
  }
  if (period_type == PeriodTypes::MONTHS){
    dt = add_months(num_periods);
  }
  if (period_type == PeriodTypes::YEARS){
    dt = add_years(num_periods);
  }
  return dt;
}

bool ChronoDate::is_weekend() const {
  date::weekday wd{ date::sys_days(date_) };
  unsigned wdn{ wd.iso_encoding() }; // Mon =  1, ..., Sat = 6, Sun = 7
  if (wdn > 5)
    return true;
  return false;
}

date::weekday ChronoDate::weekday() const {
  return date::sys_days(date_);
}

int ChronoDate::operator - (const ChronoDate& rhs) const {
  // By using the serial dates, we can avoid
  // these function calls:
  // return (date::sys_days(date_).time_since_epoch()
  //	- date::sys_days(rhs.date_).time_since_epoch()).count();
  return this->serial_date_ - rhs.serial_date_;
}

bool ChronoDate::operator == (const ChronoDate& rhs) const {
  return date_ == rhs.date_;
}

std::strong_ordering ChronoDate::operator <=> (const ChronoDate& rhs) const {
  return date_ <=> rhs.date_;
}


bool ChronoDate::is_eom() const {
  return date_ == date_.year() / date_.month() / date::last;
}

ChronoDate ChronoDate::eom() const {
  return {Date{date_.year()/date_.month()/date::last}};
}


unsigned ChronoDate::days_in_month() const {
  // The code below is an adaption of the algorithm on the
  // std::chrono GitHub site:  https://howardhinnant.github.io/date_algorithms.html#last_day_of_month

  unsigned m = static_cast<unsigned>(date_.month());
  std::array<unsigned, 12> normal_end_dates{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  return (m != 2 || !date_.year().is_leap() ? normal_end_dates[m - 1] : 29);
}

bool ChronoDate::leap_year() const {
  return date_.year().is_leap();
}

bool ChronoDate::is_last_day_of_feb() const {
  if (date_.month() == std::chrono::February && date_ == date_.year()/date_.month()/std::chrono::last){
    return true;
  }
  return false;
}

void ChronoDate::reset_serial_date_() {
  serial_date_ = date::sys_days(date_).time_since_epoch().count();
}

// This is a 'friend' of the ChronoDate class; also
// requires separate export keyword even though the
// class declaration is export(ed).
std::ostream& operator << (std::ostream& os, const ChronoDate& rhs) {
  std::chrono::year_month_day t = rhs.ymd();
  return os << static_cast<unsigned>(t.month()) << "/" << static_cast<unsigned>(t.day()) << "/" << static_cast<int>(t.year());
}


