#pragma once

# include "common.hpp"

class UUID
{
    private:
        static const char* hex_chars;
        static bool seeded;

        static void seed_random() {
            if (!seeded) {
                srand(static_cast<unsigned int>(time(0)));
                seeded = true;
            }
        }

        static char random_hex_char() {
            return hex_chars[rand() % 16];
        }

        static std::string random_hex_string(int length) {
            std::string result;
            for (int i = 0; i < length; ++i) {
                result += random_hex_char();
            }
            return result;
        }

    public:
        static std::string generate() {
            seed_random();

            std::ostringstream oss;
            oss << random_hex_string(8) << "-"
                << random_hex_string(4) << "-"
                << "4" << random_hex_string(3) << "-"
                << hex_chars[(rand() % 4) + 8] << random_hex_string(3) << "-"
                << random_hex_string(12);

            return oss.str();
        }
};
