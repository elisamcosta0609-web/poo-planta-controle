#ifndef AQUISICAO_HPP
#define AQUISICAO_HPP

#include <string>
#include <nlohmann/json.hpp>

class Aquisicao {
public:
    bool carregar(const std::string& caminho,
                  nlohmann::json& dados,
                  std::string& erro) const;

private:
    bool validar(const nlohmann::json& dados,
                 std::string& erro) const;
};

#endif
