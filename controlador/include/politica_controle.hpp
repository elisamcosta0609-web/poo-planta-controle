#ifndef POLITICA_CONTROLE_HPP
#define POLITICA_CONTROLE_HPP

struct ComandoAtuadores {
    double bomba;
    double aquecedor;
};

class PoliticaControle {
public:
    bool calcular(double nivel,
                  double temperatura,
                  double bomba_atual,
                  double aquecedor_atual,
                  ComandoAtuadores& resultado) const;
};

#endif
