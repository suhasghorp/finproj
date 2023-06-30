#ifndef FINDATE_INCLUDE_FINDATE_DAYCOUNT_H_
#define FINDATE_INCLUDE_FINDATE_DAYCOUNT_H_

#include <optional>
#include "ChronoDate.h"
#include "Calendar.h"

enum class DayCountTypes {
  ZERO = 0 ,
  THIRTY_360_BOND = 1,
  THIRTY_E_360 = 2,
  THIRTY_E_360_ISDA = 3,
  THIRTY_E_PLUS_360 = 4,
  ACT_ACT_ISDA = 5,
  ACT_ACT_ICMA = 6,
  ACT_365F = 7,
  ACT_360 = 8,
  ACT_365L = 9,
  SIMPLE = 10
};

class DayCount {
 public:
  DayCount(DayCountTypes dcc_type);
  std::tuple<double,unsigned int, unsigned int> year_frac(const ChronoDate& dt1, const ChronoDate& dt2,
                                                          FrequencyTypes freq_type,
                                                          std::optional<bool> is_term_date = false,
                                                          const std::optional<ChronoDate> &dt3 = std::nullopt);
 private:
  DayCountTypes dcc_type_;
};

#endif//FINDATE_INCLUDE_FINDATE_DAYCOUNT_H_
