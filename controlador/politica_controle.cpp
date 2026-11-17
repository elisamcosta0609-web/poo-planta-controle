#include "include/politica_controle.hpp"

#include <cmath>

bool PoliticaControle::calcular(double nivel,
                                double temperatura,
                                double bomba_atual,
                                double aquecedor_atual,
                                ComandoAtuadores& resultado) const {
    if (!std::isfinite(nivel) ||
        !std::isfinite(temperatura) ||
        !std::isfinite(bomba_atual) ||
        !std::isfinite(aquecedor_atual)) {
        return false;
    }

    if (nivel < 0.0 || nivel > 100.0 ||
        temperatura < 0.0 || temperatura > 100.0 ||
        bomba_atual < 0.0 || bomba_atual > 100.0 ||
        aquecedor_atual < 0.0 || aquecedor_atual > 100.0) {
        return false;
    }

    resultado.bomba = bomba_atual;
    resultado.aquecedor = aquecedor_atual;

    if (nivel <= 45.0) {
        resultado.bomba = 100.0;
    } else if (nivel >= 55.0) {
        resultado.bomba = 0.0;
    }

    if (temperatura <= 28.0) {
        resultado.aquecedor = 100.0;
    } else if (temperatura >= 32.0) {
        resultado.aquecedor = 0.0;
    }

    return true;
}
