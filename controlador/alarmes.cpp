#include "include/alarmes.hpp"

using json = nlohmann::json;

json Alarmes::avaliar(
    bool nivel_disponivel,
    double nivel,
    bool temperatura_disponivel,
    double temperatura
) const {
    json alarmes = json::array();

    if (!nivel_disponivel) {
        alarmes.push_back({
            {"codigo", "FALHA_NIVEL"},
            {"mensagem", "Falha na leitura do sensor de nivel."},
            {"severidade", "critica"}
        });
    } else {
        if (nivel < 30.0) {
            alarmes.push_back({
                {"codigo", "NIVEL_BAIXO"},
                {"mensagem", "Nivel abaixo de 30%."},
                {"severidade", "aviso"}
            });
        }

        if (nivel > 80.0) {
            alarmes.push_back({
                {"codigo", "NIVEL_ALTO"},
                {"mensagem", "Nivel acima de 80%."},
                {"severidade", "aviso"}
            });
        }
    }

    if (!temperatura_disponivel) {
        alarmes.push_back({
            {"codigo", "FALHA_TEMPERATURA"},
            {"mensagem", "Falha na leitura do sensor de temperatura."},
            {"severidade", "critica"}
        });
    } else if (temperatura > 35.0) {
        alarmes.push_back({
            {"codigo", "TEMPERATURA_ALTA"},
            {"mensagem", "Temperatura acima de 35 graus Celsius."},
            {"severidade", "critica"}
        });
    }

    return alarmes;
}
