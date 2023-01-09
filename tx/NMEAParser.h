#ifndef NMEA_PARSER_H
#define NMEA_PARSER_H

double parseDegrees(const char *term)
{
  uint32_t leftOfDecimal = (uint32_t)atol(term);
  uint16_t minutes = (uint16_t)(leftOfDecimal % 100);
  uint32_t multiplier = 10000000UL;
  uint32_t tenMillionthsOfMinutes = minutes * multiplier;

  uint16_t deg = (int16_t)(leftOfDecimal / 100);

  while (isdigit(*term))
    ++term;

  if (*term == '.')
    while (isdigit(*++term))
    {
      multiplier /= 10;
      tenMillionthsOfMinutes += (*term - '0') * multiplier;
    }

  uint32_t billionths = (5 * tenMillionthsOfMinutes + 1) / 3;
  return deg + billionths / 1000000000.0;;
}

class NMEAParser {
public:
    int headerptr_ = 0;
    const char* header = "GPGLL,";
    const int header_length_ = 6;
    int commacount_ = 0;
    String latitudestr_;
    String longitudestr_;
    char northorsouth_, eastorwest_, status_;

    double longitude_;
    double latitiude_;


public:
    bool feed(char c) {
        if (c == '\r') return false;
        Serial.print(c);
        if (headerptr_ == header_length_) {
            return processChar(c);
        } else {
            //update state
            if (c == header[headerptr_]) headerptr_++;
            else if (c == header[0]) headerptr_ = 1;
            else headerptr_ = 0;
            
            if (headerptr_ == header_length_) {
                latitudestr_ = "";
                longitudestr_ = "";
            }
        }
        return false;
    }

    bool isValid() {
        return status_ == 'A';
    }

    double lat() {return latitiude_;}
    double lng() {return longitude_;}
private:
    bool processChar(char c) {
        switch (c) {
        case ',':
            commacount_++;
            break;
        case '\n':
            //reset
            headerptr_ = 0;
            commacount_ = 0;
            if (!(status_ == 'A' || status_ == 'V')) return false;

            latitiude_ = parseDegrees(latitudestr_.c_str());
            if (northorsouth_ == 'S') latitiude_ *= -1;
            longitude_= parseDegrees(longitudestr_.c_str());
            if (eastorwest_ == 'W') longitude_ *= -1;
            return true;
        default:
            switch (commacount_) {
            case 0:
                latitudestr_.concat(c);
                break;
            case 1:
                northorsouth_ = c;
                break;
            case 2:
                longitudestr_.concat(c);
                break;
            case 3:
                eastorwest_ = c;
                break;
            case 5:
                status_ = c;
                break;
            }
        }
        return false;
    }
};

#endif
