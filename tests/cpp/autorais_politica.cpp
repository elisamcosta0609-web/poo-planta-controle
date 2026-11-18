#include "include/politica_controle.hpp"

#include <cmath>
#include <iostream>
#include <limits>

int main() {
    PoliticaControle politica;
    ComandoAtuadores resultado{0.0, 0.0};

    // 1. Ligar os dois atuadores.
    if (!politica.calcular(40.0, 25.0, 0.0, 0.0, resultado) ||
        resultado.bomba != 100.0 ||
        resultado.aquecedor != 100.0) {
        std::cerr << "Falha ao ligar os atuadores.\n";
        return 1;
    }

    // 2. Desligar os dois atuadores.
    if (!politica.calcular(60.0, 35.0, 100.0, 100.0, resultado) ||
        resultado.bomba != 0.0 ||
        resultado.aquecedor != 0.0) {
        std::cerr << "Falha ao desligar os atuadores.\n";
        return 2;
    }

    // 3. Manter os dois ligados na faixa intermediária.
    if (!politica.calcular(50.0, 30.0, 100.0, 100.0, resultado) ||
        resultado.bomba != 100.0 ||
        resultado.aquecedor != 100.0) {
        std::cerr << "Falha ao manter os atuadores ligados.\n";
        return 3;
    }

    // 4. Manter os dois desligados na faixa intermediária.
    if (!politica.calcular(50.0, 30.0, 0.0, 0.0, resultado) ||
        resultado.bomba != 0.0 ||
        resultado.aquecedor != 0.0) {
        std::cerr << "Falha ao manter os atuadores desligados.\n";
        return 4;
    }

    // 5. Fronteira inferior do nível: 45 deve ligar a bomba.
    if (!politica.calcular(45.0, 35.0, 0.0, 0.0, resultado) ||
        resultado.bomba != 100.0 ||
        resultado.aquecedor != 0.0) {
        std::cerr << "Falha na fronteira inferior do nivel.\n";
        return 5;
    }

    // 6. Fronteira superior do nível: 55 deve desligar a bomba.
    if (!politica.calcular(55.0, 25.0, 100.0, 0.0, resultado) ||
        resultado.bomba != 0.0 ||
        resultado.aquecedor != 100.0) {
        std::cerr << "Falha na fronteira superior do nivel.\n";
        return 6;
    }

    // 7. Fronteira inferior da temperatura: 28 deve ligar o aquecedor.
    if (!politica.calcular(60.0, 28.0, 0.0, 0.0, resultado) ||
        resultado.bomba != 0.0 ||
        resultado.aquecedor != 100.0) {
        std::cerr << "Falha na fronteira inferior da temperatura.\n";
        return 7;
    }

    // 8. Fronteira superior da temperatura: 32 deve desligar o aquecedor.
    if (!politica.calcular(40.0, 32.0, 0.0, 100.0, resultado) ||
        resultado.bomba != 100.0 ||
        resultado.aquecedor != 0.0) {
        std::cerr << "Falha na fronteira superior da temperatura.\n";
        return 8;
    }

    // 9. Nível baixo liga somente a bomba.
    if (!politica.calcular(40.0, 35.0, 0.0, 0.0, resultado) ||
        resultado.bomba != 100.0 ||
        resultado.aquecedor != 0.0) {
        std::cerr << "Falha no controle independente da bomba.\n";
        return 9;
    }

    // 10. Temperatura baixa liga somente o aquecedor.
    if (!politica.calcular(60.0, 25.0, 0.0, 0.0, resultado) ||
        resultado.bomba != 0.0 ||
        resultado.aquecedor != 100.0) {
        std::cerr << "Falha no controle independente do aquecedor.\n";
        return 10;
    }

    // 11. Rejeitar nível fora dos limites.
    resultado = {12.0, 34.0};
    if (politica.calcular(150.0, 30.0, 0.0, 0.0, resultado) ||
        resultado.bomba != 12.0 ||
        resultado.aquecedor != 34.0) {
        std::cerr << "Falha ao rejeitar nivel invalido ou preservar resultado.\n";
        return 11;
    }

    // 12. Rejeitar temperatura fora dos limites.
    if (politica.calcular(50.0, -1.0, 0.0, 0.0, resultado)) {
        std::cerr << "Falha: aceitou temperatura invalida.\n";
        return 12;
    }

    // 13. Rejeitar atuação atual fora dos limites.
    if (politica.calcular(50.0, 30.0, 101.0, 0.0, resultado)) {
        std::cerr << "Falha: aceitou bomba atual invalida.\n";
        return 13;
    }

    if (politica.calcular(50.0, 30.0, 0.0, -1.0, resultado)) {
        std::cerr << "Falha: aceitou aquecedor atual invalido.\n";
        return 14;
    }

    // 14. Rejeitar valores não finitos.
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double infinito = std::numeric_limits<double>::infinity();

    if (politica.calcular(nan, 30.0, 0.0, 0.0, resultado)) {
        std::cerr << "Falha: aceitou nivel NaN.\n";
        return 15;
    }

    if (politica.calcular(50.0, infinito, 0.0, 0.0, resultado)) {
        std::cerr << "Falha: aceitou temperatura infinita.\n";
        return 16;
    }

    std::cout << "Testes de PoliticaControle aprovados.\n";
    return 0;
}
