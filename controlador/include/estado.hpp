#ifndef ESTADO_HPP
#define ESTADO_HPP

#include <string>

class Estado {
public:
    std::string sessao;

    int ciclo = 0;
    int ultimo_confirmado = 0;

    bool tem_pendente = false;
    int id_pendente = 0;

    double bomba_pendente = 0.0;
    double aquecedor_pendente = 0.0;

    bool mesmo_ciclo(const std::string& nova_sessao,
                     int novo_ciclo) const;

    void atualizar_ciclo(const std::string& nova_sessao,
                         int novo_ciclo);

    void confirmar_comando(int novo_id);

    void preparar_comando(int novo_id,
                          double bomba,
                          double aquecedor);

    void limpar_pendente();

    bool carregar(const std::string& caminho);
    bool salvar(const std::string& caminho) const;
};

#endif
