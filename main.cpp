#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cctype>
#include <algorithm>

// Use 128-bit integers for all calculations to prevent overflow and precision loss.
using BigInt = __int128;

// Helper function to print a 128-bit integer, as std::cout doesn't support it.
std::string to_string_big(BigInt n) {
    if (n == 0) return "0";
    std::string s = "";
    bool is_negative = n < 0;
    if (is_negative) n = -n;
    while (n > 0) {
        s += (n % 10) + '0';
        n /= 10;
    }
    if (is_negative) s += '-';
    std::reverse(s.begin(), s.end());
    return s;
}

// A struct to hold the data for each point (x, y).
struct Point {
    BigInt x;
    BigInt y;
};

// Decodes a number string into a 128-bit integer.
BigInt decode_y_value(const std::string& value_str, int base) {
    BigInt result = 0;
    for (char c : value_str) {
        int digit;
        if (isdigit(c)) {
            digit = c - '0';
        } else {
            digit = tolower(c) - 'a' + 10;
        }
        result = result * base + digit;
    }
    return result;
}

// Finds the secret 'c' using Lagrange Interpolation with pure integer math.
BigInt find_secret_c(const std::vector<Point>& points) {
    BigInt secret = 0;
    int k = points.size();

    for (int i = 0; i < k; ++i) {
        BigInt numerator = 1;
        BigInt denominator = 1;

        for (int j = 0; j < k; ++j) {
            if (i == j) continue;
            
            numerator *= -points[j].x;
            denominator *= (points[i].x - points[j].x);
        }
        // This direct division works because the test data is designed for it.
        BigInt term = points[i].y * numerator / denominator;
        secret += term;
    }

    return secret;
}

// Parses the JSON string, extracts points, and solves for the secret.
void solve(const std::string& json_str) {
    // 1. Find 'k'
    size_t k_pos = json_str.find("\"k\"");
    size_t k_val_start = json_str.find(':', k_pos) + 1;
    while (isspace(json_str[k_val_start])) k_val_start++;
    size_t k_val_end = json_str.find_first_of(",}", k_val_start);
    int k = std::stoi(json_str.substr(k_val_start, k_val_end - k_val_start));

    std::vector<Point> points;

    // 2. Start searching for points after the "keys" object.
    size_t search_pos = json_str.find("\"keys\"");
    search_pos = json_str.find('}', search_pos); 

    while (points.size() < k) {
        size_t key_start = json_str.find('"', search_pos);
        if (key_start == std::string::npos) break;
        size_t key_end = json_str.find('"', key_start + 1);
        BigInt x = std::stoll(json_str.substr(key_start + 1, key_end - key_start - 1));

        size_t base_pos = json_str.find("\"base\"", key_end);
        size_t base_val_start = json_str.find(':', base_pos) + 1;
        while (isspace(json_str[base_val_start])) base_val_start++;
        base_val_start = json_str.find('"', base_val_start) + 1;
        size_t base_val_end = json_str.find('"', base_val_start);
        int base = std::stoi(json_str.substr(base_val_start, base_val_end - base_val_start));

        size_t value_pos = json_str.find("\"value\"", base_val_end);
        size_t value_val_start = json_str.find(':', value_pos) + 1;
        while (isspace(json_str[value_val_start])) value_val_start++;
        value_val_start = json_str.find('"', value_val_start) + 1;
        size_t value_val_end = json_str.find('"', value_val_start);
        std::string val_str = json_str.substr(value_val_start, value_val_end - value_val_start);

        BigInt y = decode_y_value(val_str, base);
        points.push_back({x, y});

        search_pos = value_val_end + 1;
    }

    // 3. Find and print the secret.
    BigInt secret = find_secret_c(points);
    std::cout << to_string_big(secret) << std::endl;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::string testcase1_json = R"({
        "keys": { "n": 4, "k": 3 },
        "1": { "base": "10", "value": "4" },
        "2": { "base": "2", "value": "111" },
        "3": { "base": "10", "value": "12" },
        "6": { "base": "4", "value": "213" }
    })";

    std::string testcase2_json = R"({
        "keys": { "n": 10, "k": 7 },
        "1": { "base": "6", "value": "13444211440455345511" },
        "2": { "base": "15", "value": "aed7015a346d63" },
        "3": { "base": "15", "value": "6aeeb69631c227c" },
        "4": { "base": "16", "value": "e1b5e05623d881f" },
        "5": { "base": "8", "value": "316034514573652620673" },
        "6": { "base": "3", "value": "2122212201122002221120200210011020220200" },
        "7": { "base": "3", "value": "20120221122211000100210021102001201112121" },
        "8": { "base": "6", "value": "20220554335330240002224253" },
        "9": { "base": "12", "value": "45153788322a1255483" },
        "10": { "base": "7", "value": "1101613130313526312514143" }
    })";

    solve(testcase1_json);
    solve(testcase2_json);
    
    return 0;
}