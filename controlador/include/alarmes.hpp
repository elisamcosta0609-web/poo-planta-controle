#ifndef ALARMES_HPP
#define ALARMES_HPP

#include <nlohmann/json.hpp>

class Alarmes {
public:
    nlohmann::json avaliar(
        bool nivel_disponivel,
        double nivel,
        bool temperatura_disponivel,
        double temperatura
    ) const;
};

#endif
