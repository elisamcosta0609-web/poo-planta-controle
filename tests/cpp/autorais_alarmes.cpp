#include "include/alarmes.hpp"

#include <iostream>

int main() {
    Alarmes alarmes;

    // 1. Nível abaixo de 30%.
    auto resultado = alarmes.avaliar(true, 25.0, true, 25.0);

    if (resultado.size() != 1 ||
        resultado[0]["codigo"] != "NIVEL_BAIXO") {
        std::cerr << "Falha no alarme de nivel baixo.\n";
        return 1;
    }

    // 2. Nível acima de 80%.
    resultado = alarmes.avaliar(true, 90.0, true, 25.0);

    if (resultado.size() != 1 ||
        resultado[0]["codigo"] != "NIVEL_ALTO") {
        std::cerr << "Falha no alarme de nivel alto.\n";
        return 2;
    }

    // 3. Temperatura acima de 35 graus.
    resultado = alarmes.avaliar(true, 50.0, true, 40.0);

    if (resultado.size() != 1 ||
        resultado[0]["codigo"] != "TEMPERATURA_ALTA") {
        std::cerr << "Falha no alarme de temperatura alta.\n";
        return 3;
    }

    // 4. Falha no sensor de nível.
    resultado = alarmes.avaliar(false, 0.0, true, 25.0);

    if (resultado.size() != 1 ||
        resultado[0]["codigo"] != "FALHA_NIVEL") {
        std::cerr << "Falha no alarme do sensor de nivel.\n";
        return 4;
    }

    // 5. Falha no sensor de temperatura.
    resultado = alarmes.avaliar(true, 50.0, false, 0.0);

    if (resultado.size() != 1 ||
        resultado[0]["codigo"] != "FALHA_TEMPERATURA") {
        std::cerr << "Falha no alarme do sensor de temperatura.\n";
        return 5;
    }

    // 6. Nenhum alarme em condições normais.
    resultado = alarmes.avaliar(true, 50.0, true, 25.0);

    if (!resultado.empty()) {
        std::cerr << "Falha: gerou alarme em condicoes normais.\n";
        return 6;
    }

    // 7. Dois alarmes simultâneos.
    resultado = alarmes.avaliar(true, 20.0, true, 40.0);

    if (resultado.size() != 2) {
        std::cerr << "Falha nos alarmes simultaneos.\n";
        return 7;
    }

    // 8. Verificar severidade.
    resultado = alarmes.avaliar(true, 20.0, true, 25.0);

    if (resultado[0]["severidade"] != "aviso") {
        std::cerr << "Falha na severidade do alarme de nivel.\n";
        return 8;
    }

    resultado = alarmes.avaliar(true, 50.0, true, 40.0);

    if (resultado[0]["severidade"] != "critica") {
        std::cerr << "Falha na severidade do alarme de temperatura.\n";
        return 9;
    }

    std::cout << "Testes de Alarmes aprovados.\n";
    return 0;
}
